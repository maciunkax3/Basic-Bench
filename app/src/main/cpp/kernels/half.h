#pragma once

const char *half1Program = R"===(
#pragma OPENCL EXTENSION cl_khr_fp16 : enable
#define MAD_4(x, y)     x = mad(y, x, y);   y = mad(x, y, x);   x = mad(y, x, y);   y = mad(x, y, x);
#define MAD_16(x, y)    MAD_4(x, y);        MAD_4(x, y);        MAD_4(x, y);        MAD_4(x, y);
#define MAD_64(x, y)    MAD_16(x, y);       MAD_16(x, y);       MAD_16(x, y);       MAD_16(x, y);

__kernel void Half1(__global half *ptr, float _hp)
{
    half hp = (half)_hp;
    half x = hp;
    half y = (half)get_local_id(0);

    for(int i=0; i<128; i++)
    {
        MAD_16(x, y);
    }

    ptr[get_global_id(0)] = y;
}
)===";

const char *half2Program = R"===(
#pragma OPENCL EXTENSION cl_khr_fp16 : enable
#define MAD_4(x, y)     x = mad(y, x, y);   y = mad(x, y, x);   x = mad(y, x, y);   y = mad(x, y, x);
#define MAD_16(x, y)    MAD_4(x, y);        MAD_4(x, y);        MAD_4(x, y);        MAD_4(x, y);
#define MAD_64(x, y)    MAD_16(x, y);       MAD_16(x, y);       MAD_16(x, y);       MAD_16(x, y);

__kernel void Half2(__global half *ptr, float _hp)
{
    half hp = (half)_hp;
    half2 x = (half2)(hp, hp);
    half2 y = (half2)get_local_id(0);

    for(int i=0; i<64; i++)
    {
        MAD_16(x, y);
    }

    ptr[get_global_id(0)] = (y.S0) + (y.S1);
}
)===";

const char *half4Program = R"===(
#pragma OPENCL EXTENSION cl_khr_fp16 : enable
#define MAD_4(x, y)     x = mad(y, x, y);   y = mad(x, y, x);   x = mad(y, x, y);   y = mad(x, y, x);
#define MAD_16(x, y)    MAD_4(x, y);        MAD_4(x, y);        MAD_4(x, y);        MAD_4(x, y);
#define MAD_64(x, y)    MAD_16(x, y);       MAD_16(x, y);       MAD_16(x, y);       MAD_16(x, y);

__kernel void Half4(__global half *ptr, float _hp)
{
    half hp = (half)_hp;
    half4 x = (half4)(hp, hp, hp, hp);
    half4 y = (half4)get_local_id(0);

    for(int i=0; i<32; i++)
    {
        MAD_16(x, y);
    }

    ptr[get_global_id(0)] = (y.S0) + (y.S1) + (y.S2) + (y.S3);
}
)===";

const char *half8Program = R"===(
#pragma OPENCL EXTENSION cl_khr_fp16 : enable
#define MAD_4(x, y)     x = mad(y, x, y);   y = mad(x, y, x);   x = mad(y, x, y);   y = mad(x, y, x);
#define MAD_16(x, y)    MAD_4(x, y);        MAD_4(x, y);        MAD_4(x, y);        MAD_4(x, y);
#define MAD_64(x, y)    MAD_16(x, y);       MAD_16(x, y);       MAD_16(x, y);       MAD_16(x, y);

__kernel void Half8(__global half *ptr, float _hp)
{
    half hp = (half)_hp;
    half8 x = (half8)(hp, hp, hp, hp, hp, hp, hp, hp);
    half8 y = (half8)get_local_id(0);

    for(int i=0; i<16; i++)
    {
        MAD_16(x, y);
    }

    ptr[get_global_id(0)] = (y.S0) + (y.S1) + (y.S2) + (y.S3) + (y.S4) + (y.S5) + (y.S6) + (y.S7);
}
)===";

const char *half16Program = R"===(
#pragma OPENCL EXTENSION cl_khr_fp16 : enable
#define MAD_4(x, y)     x = mad(y, x, y);   y = mad(x, y, x);   x = mad(y, x, y);   y = mad(x, y, x);
#define MAD_16(x, y)    MAD_4(x, y);        MAD_4(x, y);        MAD_4(x, y);        MAD_4(x, y);
#define MAD_64(x, y)    MAD_16(x, y);       MAD_16(x, y);       MAD_16(x, y);       MAD_16(x, y);

__kernel void Half16(__global half *ptr, float _hp)
{
    half hp = (half)_hp;
    half16 x = (half16)(hp, hp, hp, hp, hp, hp, hp, hp, hp, hp, hp, hp, hp, hp, hp, hp);
    half16 y = (half16)get_local_id(0);

    for(int i=0; i<8; i++)
    {
        MAD_16(x, y);
    }

    ptr[get_global_id(0)] = (y.S0) + (y.S1) + (y.S2) + (y.S3) + (y.S4) + (y.S5) + (y.S6) + (y.S7) + (y.S8) + (y.S9) + (y.SA) + (y.SB) + (y.SC) + (y.SD) + (y.SE) + (y.SF);
}
)===";
