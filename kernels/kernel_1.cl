typedef float4 xzy;

__kernel void kernel_1(__global xzy* _posBO,
                       __global xzy* _velBO)
{
    int gid = get_global_id(0);

    _posBO[gid] += _velBO[gid];
}