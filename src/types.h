#pragma once

typedef unsigned int u4;
typedef unsigned short u2;
typedef unsigned char u1;

typedef long long i8;
typedef unsigned long long u8;

typedef int i4;
typedef short i2;

typedef float f4;
typedef double f8;

typedef u1 jchar;
typedef u2 jshort;
typedef i4 jint;
typedef f4 jfloat;

//#define ASSERT(x) assert(x)

//BE is a Big Endian
#define LOINT64(I8) (u4)(I8 &0xFFFFFFFF)
#define HIINT64(I8) (u4)(I8 >> 32)
#define getu4(p) (u4)((u4)((p)[0])<< 24 & 0xFF000000 | (u4)((p)[1])<< 16 & 0x00FF0000 | (u4)((p)[2])<<8 & 0x0000FF00| (u4)((p)[3]) & 0x000000FF) //int
#define getu2(ptr) getu2_func(((u1 **)&ptr));

u2 getu2_func(u1 ** ptr);

#define MAKEI8(i4high, i4low) (i8) (((i8)i4high) << 32 | (i8)i4low)

//getf4 depents on geti4 : carefull when changing...
#define geti4(p) (i4)( (u4)((p)[0])<<24 | (u4)((p)[1])<<16 | (u4)((p)[2])<<8 | (u4)((p)[3]) )
#define geti2(p) (i2)(((p)[0]<<8)|(p)[1])

#define ASSERT assert