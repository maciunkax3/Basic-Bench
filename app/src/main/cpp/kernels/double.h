#pragma once

const char *double1Program = R"===(

#define MAD_4(x, y)     x = mad(y, x, y);   y = mad(x, y, x);   x = mad(y, x, y);   y = mad(x, y, x);
#define MAD_16(x, y)    MAD_4(x, y);        MAD_4(x, y);        MAD_4(x, y);        MAD_4(x, y);
#define MAD_64(x, y)    MAD_16(x, y);       MAD_16(x, y);       MAD_16(x, y);       MAD_16(x, y);

__kernel void Double1(__global double *ptr, double _dp)
{
    double dp = (double)_dp;
    double x = dp;
    double y = (double)get_local_id(0);

    for(int i=0; i<128; i++)
    {
        MAD_16(x, y);
    }

    ptr[get_global_id(0)] = y;
}
)===";

const char *double2Program = R"===(

#define MAD_4(x, y)     x = mad(y, x, y);   y = mad(x, y, x);   x = mad(y, x, y);   y = mad(x, y, x);
#define MAD_16(x, y)    MAD_4(x, y);        MAD_4(x, y);        MAD_4(x, y);        MAD_4(x, y);
#define MAD_64(x, y)    MAD_16(x, y);       MAD_16(x, y);       MAD_16(x, y);       MAD_16(x, y);

__kernel void Double2(__global double *ptr, double _dp)
{
    double dp = (double)_dp;
    double2 x = (double2)(dp, dp);
    double2 y = (double2)get_local_id(0);

    for(int i=0; i<64; i++)
    {
        MAD_16(x, y);
    }

    ptr[get_global_id(0)] = (y.S0) + (y.S1);
}
)===";

const char *double4Program = R"===(

#define MAD_4(x, y)     x = mad(y, x, y);   y = mad(x, y, x);   x = mad(y, x, y);   y = mad(x, y, x);
#define MAD_16(x, y)    MAD_4(x, y);        MAD_4(x, y);        MAD_4(x, y);        MAD_4(x, y);
#define MAD_64(x, y)    MAD_16(x, y);       MAD_16(x, y);       MAD_16(x, y);       MAD_16(x, y);

__kernel void Double4(__global double *ptr, double _dp)
{
    double dp = (double)_dp;
    double4 x = (double4)(dp, dp, dp, dp);
    double4 y = (double4)get_local_id(0);

    for(int i=0; i<32; i++)
    {
        MAD_16(x, y);
    }

    ptr[get_global_id(0)] = (y.S0) + (y.S1) + (y.S2) + (y.S3);
}
)===";

const char *double8Program = R"===(

#define MAD_4(x, y)     x = mad(y, x, y);   y = mad(x, y, x);   x = mad(y, x, y);   y = mad(x, y, x);
#define MAD_16(x, y)    MAD_4(x, y);        MAD_4(x, y);        MAD_4(x, y);        MAD_4(x, y);
#define MAD_64(x, y)    MAD_16(x, y);       MAD_16(x, y);       MAD_16(x, y);       MAD_16(x, y);

__kernel void Double8(__global double *ptr, double _dp)
{
    double dp = (double)_dp;
    double8 x = (double8)(dp, dp, dp, dp, dp, dp, dp, dp);
    double8 y = (double8)get_local_id(0);

    for(int i=0; i<16; i++)
    {
        MAD_16(x, y);
    }

    ptr[get_global_id(0)] = (y.S0) + (y.S1) + (y.S2) + (y.S3) + (y.S4) + (y.S5) + (y.S6) + (y.S7);
}
)===";

const char *double16Program = R"===(

#define MAD_4(x, y)     x = mad(y, x, y);   y = mad(x, y, x);   x = mad(y, x, y);   y = mad(x, y, x);
#define MAD_16(x, y)    MAD_4(x, y);        MAD_4(x, y);        MAD_4(x, y);        MAD_4(x, y);
#define MAD_64(x, y)    MAD_16(x, y);       MAD_16(x, y);       MAD_16(x, y);       MAD_16(x, y);

__kernel void Double16(__global double *ptr, double _dp)
{
    double dp = (double)_dp;
    double16 x = (double16)(dp, dp, dp, dp, dp, dp, dp, dp, dp, dp, dp, dp, dp, dp, dp, dp);
    double16 y = (double16)get_local_id(0);

    for(int i=0; i<8; i++)
    {
        MAD_16(x, y);
    }

    ptr[get_global_id(0)] = (y.S0) + (y.S1) + (y.S2) + (y.S3) + (y.S4) + (y.S5) + (y.S6) + (y.S7) + (y.S8) + (y.S9) + (y.SA) + (y.SB) + (y.SC) + (y.SD) + (y.SE) + (y.SF);
}
)===";
