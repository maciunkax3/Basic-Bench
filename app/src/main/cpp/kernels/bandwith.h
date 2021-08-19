#pragma once

const char *bandwithType = R"===(
#pragma OPENCL EXTENSION cl_khr_fp16 : enable
#ifdef Float1
typedef float Type;
#endif
#ifdef Float2
typedef float2 Type;
#endif

#ifdef Float4
typedef float4 Type;
#endif

#ifdef Float8
typedef float8 Type;
#endif

#ifdef Float16
typedef float16 Type;
#endif

#ifdef Double1
typedef double Type;
#endif
#ifdef Double2
typedef double2 Type;
#endif

#ifdef Double4
typedef double4 Type;
#endif

#ifdef Double8
typedef double8 Type;
#endif

#ifdef Double16
typedef double16 Type;
#endif

#ifdef Int1
typedef int Type;
#endif
#ifdef Int2
typedef int2 Type;
#endif

#ifdef Int4
typedef int4 Type;
#endif

#ifdef Int8
typedef int8 Type;
#endif

#ifdef Int16
typedef int16 Type;
#endif

#ifdef Half1
typedef half Type;
#endif
#ifdef Half2
typedef half2 Type;
#endif

#ifdef Half4
typedef half4 Type;
#endif

#ifdef Half8
typedef half8 Type;
#endif

#ifdef Half16
typedef half16 Type;
#endif
__kernel void readType(__global Type *dst, __global Type *src){
    uint gid = get_global_id(0);
    dst[gid] = src[gid];
}
)===";
