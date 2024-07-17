__kernel void n_squared(
    float timestep,
    float minimumSqDistance,
    __global float* particleBuffer,
    __local float4* posBlock)
{
    const float4 timestep_vector = (float4)(timestep, timestep, timestep, 0.0f);
    int globalId = get_global_id(0);
    int localId = get_local_id(0); // ID in this workgroup of this item
    int n = get_global_size(0);
    int nWorkgroupItems = get_local_size(0);
    int nWorkgroups = n / nWorkgroupItems;

    // Calculate correct indices for position and velocity
    int posIndex = 7 * globalId;
    int velIndex = posIndex + 4;

    // Load particle position and velocity
    float4 position = (float4)(particleBuffer[posIndex], particleBuffer[posIndex+1], particleBuffer[posIndex+2], particleBuffer[posIndex+3]);
    float4 velocity = (float4)(particleBuffer[velIndex], particleBuffer[velIndex+1], particleBuffer[velIndex+2], 0.0f);
    float4 acceleration = (float4)(0.0f, 0.0f, 0.0f, 0.0f);

    for(int workgroupIndex = 0; workgroupIndex < nWorkgroups; workgroupIndex++) { // for each workgroup
        // Calculate the correct index for the workgroup particle
        int wgPosIndex = 7 * (workgroupIndex * nWorkgroupItems + localId);
        
        // Load position into local memory
        posBlock[localId] = (float4)(
            particleBuffer[wgPosIndex], 
            particleBuffer[wgPosIndex+1],
            particleBuffer[wgPosIndex+2],
            particleBuffer[wgPosIndex+3]
        ); // cache one particle position
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

    // Update position and velocity
    position += timestep*velocity + 0.5f*timestep*timestep*acceleration;
    velocity += timestep*acceleration;

    // Write updated position back to the buffer
    particleBuffer[posIndex] = position.x;
    particleBuffer[posIndex+1] = position.y;
    particleBuffer[posIndex+2] = position.z;

    // Write updated velocity back to the buffer
    particleBuffer[velIndex] = velocity.x;
    particleBuffer[velIndex+1] = velocity.y;
    particleBuffer[velIndex+2] = velocity.z;
}