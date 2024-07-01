__kernel void n_squared(
    float timestep,
    float minimumSqDistance,
    __global float4* posBO,
    __global float4* velBO,
    __local float4* posBlock)
{
    const float4 timestep_vector = (float4)(timestep, timestep, timestep, 0.0f);
    int globalId = get_global_id(0);
    int localId = get_local_id(0);
    int n = get_global_size(0);
    int nLocal = get_local_size(0);
    int nBlocks = n / nLocal;

    float4 position = posBO[globalId];
    float4 velocity = velBO[globalId];
    float4 acceleration = (float4)(0.0f, 0.0f, 0.0f, 0.0f);

    for(int iBlock = 0; iBlock < nBlocks; iBlock++) { // for each block
        posBlock[localId] = posBO[iBlock*nLocal+localId]; // cache one particle position
        work_group_barrier(CLK_LOCAL_MEM_FENCE); // wait for others in work group
        for(int i = 0; i < nLocal; i++) { // for all cached particle positions in work group
            float4 cached_pos = posBlock[i];
            float4 distance = cached_pos - position;
            float inverse_r = rsqrt(distance.x*distance.x + distance.y*distance.y + distance.z*distance.z + minimumSqDistance);
            float force = cached_pos.w*inverse_r*inverse_r*inverse_r;
            acceleration += force * distance;
        }
        work_group_barrier(CLK_LOCAL_MEM_FENCE);
    }

    position += timestep*velocity + 0.5f*timestep*timestep*acceleration;
    velocity += timestep*acceleration;
    posBO[globalId] = position;
    velBO[globalId] = velocity;
}