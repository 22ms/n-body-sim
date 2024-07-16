__kernel void n_squared(
    float timestep,
    float minimumSqDistance,
    __global float4* posBuffer,
    __global float4* velBuffer,
    __local float4* posBlock)
{
    const float4 timestep_vector = (float4)(timestep, timestep, timestep, 0.0f);
    int globalId = get_global_id(0);
    int localId = get_local_id(0); // ID in this workgroup of this item
    int n = get_global_size(0);
    int nWorkgroupItems = get_local_size(0);
    int nWorkgroups = n / nWorkgroupItems;

    float4 position = posBuffer[globalId];
    float4 velocity = velBuffer[globalId];
    float4 acceleration = (float4)(0.0f, 0.0f, 0.0f, 0.0f);

    for(int workgroupIndex = 0; workgroupIndex < nWorkgroups; workgroupIndex++) { // for each workgroup
        posBlock[localId] = posBuffer[workgroupIndex*nWorkgroupItems+localId]; // cache one particle position
        barrier(CLK_LOCAL_MEM_FENCE); // wait for others in work group
        for(int i = 0; i < nWorkgroupItems; i++) { // for each pos in work group
            float4 cached_pos = posBlock[i];
            float4 distance = cached_pos - position;
            float inverse_r = rsqrt(distance.x*distance.x + distance.y*distance.y + distance.z*distance.z + minimumSqDistance);
            float force = cached_pos.w*inverse_r*inverse_r*inverse_r;
            acceleration += force * distance;
        }
        barrier(CLK_LOCAL_MEM_FENCE);
    }

    position += timestep*velocity + 0.5f*timestep*timestep*acceleration;
    velocity += timestep*acceleration;
    posBuffer[globalId] = position;
    velBuffer[globalId] = velocity;
}