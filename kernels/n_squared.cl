__kernel void n_squared(
    float timestep,
    float minimum_sq_distance,
    __global float4* posBO,
    __global float4* velBO,
    __local float4* pblock)
{
    const float4 timestep_vector = (float4)(timestep, timestep, timestep, 0.0f);
    int global_id = get_global_id(0);
    int local_id = get_local_id(0);
    int n = get_global_size(0);
    int n_local = get_local_size(0);
    int n_blocks = n / n_local;

    float4 position = posBO[global_id];
    float4 velocity = velBO[global_id];
    float4 acceleration = (float4)(0.0f, 0.0f, 0.0f, 0.0f);

    for(int i_block = 0; i_block < n_blocks; i_block++) { // for each block
        pblock[local_id] = posBO[i_block*n_local+local_id]; // cache one particle position
        work_group_barrier(CLK_LOCAL_MEM_FENCE); // wait for others in work group
        for(int i = 0; i < n_local; i++) { // for all cached particle positions in work group
            float4 cached_pos = pblock[i];
            float4 distance = cached_pos - position;
            float inverse_r = rsqrt(distance.x*distance.x + distance.y*distance.y + distance.z*distance.z + minimum_sq_distance);
            float force = cached_pos.w*inverse_r*inverse_r*inverse_r;
            acceleration += force * distance;
        }
        work_group_barrier(CLK_LOCAL_MEM_FENCE);
    }

    position += timestep*velocity + 0.5f*timestep*timestep*acceleration;
    velocity += timestep*acceleration;
    posBO[global_id] = position;
    velBO[global_id] = velocity;
}