/*
 * image.c
 *
 *  Created on: 11 janv. 2022
 *      Author: hugob
 */

#include "tft.h"
#include "config.h"

// 'regucolo', 61x57px
static const int16_t TFT_img[] = {
		// 'regucolo', 40x37px
		0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xf7ff, 0xf7ff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff,
		0xffff, 0xffff, 0xf7ff, 0xffff, 0xffff, 0xffff, 0xffbf, 0xffdf, 0xffff, 0xf7df, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff,
		0xffff, 0xffbe, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffdf, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff,
		0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffde, 0xffff, 0xffff, 0xf7ff, 0xf7ff, 0xe79e, 0xffff, 0xffff,
		0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xf7bf, 0xffff, 0xffff, 0xffff, 0xffff, 0xffdf, 0xffff, 0xffff, 0xffff, 0xffdf, 0xffff,
		0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff,
		0xffff, 0xffff, 0xffff, 0xf7ff, 0xdfff, 0x7d75, 0xc73d, 0xefdf, 0xffdf, 0xffdf, 0xffff, 0xf7ff, 0xffff, 0xffff, 0xdfff, 0xcfde,
		0xefff, 0xefde, 0xffff, 0xffdf, 0xffdf, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff,
		0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xf7ff, 0xdfff, 0x8e9a, 0x5535, 0x6555, 0xdfff,
		0xffff, 0xffdf, 0xffff, 0xffff, 0xf7ff, 0xe7ff, 0x6d75, 0x5d55, 0xd7ff, 0xefff, 0xffff, 0xffdf, 0xffdf, 0xffff, 0xffff, 0xffdf,
		0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff,
		0xffff, 0xffff, 0xe7ff, 0xc7ff, 0x4d56, 0x3d35, 0x4d75, 0x6595, 0xe7ff, 0xffdf, 0xffdf, 0xf7ff, 0xe7ff, 0xa73c, 0x4534, 0x4535,
		0x8699, 0xdfff, 0xf7bf, 0xffdf, 0xffff, 0xffdf, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff,
		0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffdf, 0xf7ff, 0xc7df, 0x5555, 0x3d36, 0x3597, 0x3576, 0x4555,
		0xa6ba, 0xf7ff, 0xffff, 0xefff, 0xa71b, 0x4d34, 0x3d96, 0x3555, 0x4534, 0x969a, 0xefff, 0xffbf, 0xffff, 0xffff, 0xffff, 0xffff,
		0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff,
		0xffff, 0xe7ff, 0x7e79, 0x4596, 0x3576, 0x3577, 0x2d76, 0x3555, 0x5535, 0xc7be, 0xf7ff, 0xefff, 0x5db6, 0x3574, 0x3576, 0x3576,
		0x4596, 0x5535, 0xd7ff, 0xf7ff, 0xffff, 0xffff, 0xffdf, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff,
		0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xefff, 0x9679, 0x4555, 0x3576, 0x2d35, 0x35b7, 0x2d77, 0x3596,
		0x4555, 0x6555, 0xd7df, 0xaedb, 0x4555, 0x35b6, 0x2d56, 0x3597, 0x3555, 0x4555, 0x867a, 0xe7ff, 0xefdf, 0xffff, 0xffff, 0xffff,
		0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xf7ff,
		0xc7df, 0x5555, 0x3d55, 0x3576, 0x2d76, 0x2d97, 0x2d97, 0x3597, 0x3555, 0x4d55, 0x9efc, 0xc7ff, 0x4d76, 0x3d96, 0x2d35, 0x3596,
		0x2d76, 0x3d96, 0x4534, 0xb7be, 0xf7ff, 0xffff, 0xffbe, 0xffff, 0xffff, 0xffff, 0xfffe, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff,
		0xffdf, 0xffbf, 0xffff, 0xffdf, 0xffff, 0xffff, 0xf7ff, 0xe7ff, 0x7618, 0x3d34, 0x3d76, 0x3576, 0x2d96, 0x2db7, 0x2d97, 0x2d76,
		0x2d76, 0x3d75, 0x4d55, 0xa79f, 0x86dc, 0x4515, 0x3d76, 0x3596, 0x2d76, 0x3596, 0x3534, 0x65d6, 0xefff, 0xffdf, 0xffff, 0xffff,
		0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffdf, 0xffbf, 0xffdf, 0xffdf, 0xffdf, 0xffdf, 0xffdf, 0xffff, 0xf7ff, 0xefff, 0xa6fb,
		0x4534, 0x3555, 0x4596, 0x3556, 0x3577, 0x2d97, 0x2d96, 0x2d96, 0x3596, 0x3576, 0x3515, 0x55b7, 0xbfff, 0x6db8, 0x4596, 0x2d55,
		0x2d56, 0x35b7, 0x3dd6, 0x4d54, 0xe7ff, 0xffff, 0xffdf, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffdf, 0xffbf, 0xffdf,
		0xffff, 0xffff, 0xffff, 0xffdf, 0xffff, 0xefff, 0xc7be, 0x5514, 0x3d75, 0x3596, 0x3d55, 0x3555, 0x3577, 0x2d76, 0x2d96, 0x2db6,
		0x2d76, 0x3596, 0x3576, 0x3d36, 0x9f1e, 0xbfff, 0x4555, 0x3d96, 0x3597, 0x2d55, 0x3575, 0x4513, 0xe7ff, 0xffff, 0xffbe, 0xffff,
		0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffdf, 0xffff, 0xf7ff, 0xe7ff, 0xdfbf, 0xf7ff, 0xf7ff, 0xf7ff, 0xe7ff, 0x9f3c, 0x4554,
		0x3596, 0x2d76, 0x3555, 0x3d96, 0x3577, 0x2d77, 0x35b7, 0x2d76, 0x3596, 0x3576, 0x2d76, 0x3d76, 0x5d76, 0xb7ff, 0x65d7, 0x3d35,
		0x3d56, 0x3d76, 0x3d96, 0x5d95, 0xe7ff, 0xffff, 0xffff, 0xffff, 0xffdf, 0xffdf, 0xffff, 0xffff, 0xffff, 0xffff, 0xf7ff, 0xd7df,
		0x85d7, 0x6d14, 0xdfff, 0xefff, 0xefff, 0xcfdf, 0x4d14, 0x3534, 0x3576, 0x2d76, 0x3576, 0x3d97, 0x2d56, 0x3597, 0x35b7, 0x2d56,
		0x3596, 0x2d55, 0x35b6, 0x3555, 0x4534, 0x65d7, 0xafbf, 0x5535, 0x3d36, 0x3d56, 0x76fb, 0x6595, 0xe7df, 0xffff, 0xffff, 0xf7fe,
		0xffdf, 0xffdf, 0xffff, 0xf7ff, 0xf7ff, 0xf7ff, 0xdfff, 0x8e79, 0x44f3, 0x5554, 0x75d6, 0xd7ff, 0xdfff, 0x6d56, 0x4d96, 0x3575,
		0x3596, 0x3596, 0x3597, 0x3597, 0x2d55, 0x3597, 0x3d77, 0x3d56, 0x3556, 0x3596, 0x35b5, 0x35b5, 0x3595, 0x4555, 0xc7ff, 0x9efd,
		0x4d77, 0x3d35, 0x7edb, 0x6d55, 0xf7ff, 0xffff, 0xffff, 0xf7ff, 0xffbf, 0xffdf, 0xffff, 0xffff, 0xf7ff, 0xefff, 0xc7ff, 0x4d14,
		0x3d54, 0x3d54, 0x4d34, 0xb7ff, 0xc7ff, 0x54f4, 0x3d35, 0x3576, 0x3576, 0x3576, 0x2d76, 0x3597, 0x2d76, 0x3597, 0x3536, 0x3d57,
		0x3d77, 0x3576, 0x3595, 0x2d95, 0x2d95, 0x3d75, 0x869a, 0xcfff, 0x5515, 0x7e9b, 0x6d76, 0xcfbf, 0xf7ff, 0xffff, 0xf7fe, 0xffff,
		0xffff, 0xffff, 0xffdf, 0xffff, 0xffff, 0xd7bf, 0x65b7, 0x3d76, 0x3555, 0x45b6, 0x3d55, 0x4d75, 0xb7df, 0x75d8, 0x34f4, 0x3d97,
		0x3576, 0x3576, 0x3576, 0x3576, 0x3576, 0x3576, 0x3576, 0x3576, 0x3576, 0x3556, 0x3d76, 0x3576, 0x3576, 0x3575, 0x4d76, 0xafbf,
		0x85f8, 0x8db7, 0xd79e, 0xf7ff, 0xffff, 0xffff, 0xffff, 0xffff, 0xf7ff, 0xffff, 0xffff, 0xffff, 0xf7ff, 0x9619, 0x3d35, 0x3596,
		0x2d76, 0x2d55, 0x3595, 0x3d55, 0x6df8, 0xbfff, 0x55b7, 0x3535, 0x3576, 0x3577, 0x3576, 0x3576, 0x3576, 0x3596, 0x2d96, 0x2d96,
		0x3597, 0x3556, 0x3d56, 0x3d56, 0x3556, 0x3575, 0x3555, 0x6e59, 0xcfdf, 0xe7df, 0xf7ff, 0xffff, 0xffff, 0xffff, 0xffdf, 0xffff,
		0xffff, 0xffff, 0xffff, 0xf7ff, 0xd7ff, 0x5d35, 0x3535, 0x2d96, 0x35b7, 0x2d76, 0x3596, 0x4596, 0x4514, 0xb7ff, 0x977e, 0x4555,
		0x3d56, 0x3576, 0x3576, 0x3576, 0x2d96, 0x2d96, 0x3576, 0x3576, 0x3597, 0x2d56, 0x3576, 0x3d97, 0x3556, 0x3d76, 0x3575, 0x4555,
		0xd7ff, 0xefff, 0xf7ff, 0xf7ff, 0xffff, 0xffff, 0xffdf, 0xffff, 0xffff, 0xffff, 0xf7ff, 0xefff, 0x8eba, 0x4534, 0x3db7, 0x2d76,
		0x2db7, 0x2d96, 0x2d55, 0x3d76, 0x4555, 0x65f8, 0xb7ff, 0x4d56, 0x3d56, 0x3576, 0x3576, 0x3596, 0x2d96, 0x2d96, 0x3576, 0x3576,
		0x2d76, 0x2d76, 0x2d96, 0x3597, 0x3d56, 0x3d76, 0x3db6, 0x4534, 0xc7df, 0xefff, 0xf7ff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff,
		0xfffe, 0xffff, 0xf7ff, 0xc7be, 0x4d75, 0x2d54, 0x35b7, 0x2d76, 0x2d96, 0x2d76, 0x3576, 0x3555, 0x3d55, 0x4555, 0x973d, 0x6df8,
		0x3d56, 0x3577, 0x3576, 0x3576, 0x2d96, 0x2d96, 0x3576, 0x3576, 0x2d76, 0x35b7, 0x2d96, 0x2d76, 0x3d76, 0x3556, 0x3596, 0x4555,
		0xb73c, 0xf7ff, 0xf7ff, 0xffff, 0xffff, 0xffdf, 0xffff, 0xffff, 0xfffe, 0xffff, 0xefff, 0x967a, 0x3d54, 0x2d96, 0x2d76, 0x3597,
		0x35b7, 0x2d76, 0x3d97, 0x3556, 0x3d76, 0x4d55, 0x7e38, 0xa77e, 0x3d56, 0x3577, 0x3576, 0x3576, 0x3596, 0x3596, 0x3576, 0x3576,
		0x3596, 0x3597, 0x2d76, 0x2d76, 0x3597, 0x3576, 0x2d76, 0x4555, 0x9618, 0xf7ff, 0xf7ff, 0xf7df, 0xffff, 0xffff, 0xffdf, 0xffff,
		0xfffe, 0xf7ff, 0xefff, 0x7db7, 0x4575, 0x35d7, 0x3576, 0x3577, 0x3597, 0x2d56, 0x3577, 0x3536, 0x3d76, 0x4555, 0x6db7, 0xc7ff,
		0x3d56, 0x3577, 0x3576, 0x3576, 0x3576, 0x3576, 0x2d96, 0x2d96, 0x3596, 0x2d76, 0x3596, 0x2d76, 0x35b7, 0x2d96, 0x3596, 0x4535,
		0x8dd7, 0xf7ff, 0xf7ff, 0xffff, 0xffff, 0xffff, 0xffdf, 0xffff, 0xfffe, 0xf7fe, 0xefff, 0x7d97, 0x3d35, 0x2d76, 0x3577, 0x3556,
		0x2d56, 0x3577, 0x3577, 0x3d77, 0x3d56, 0x5df8, 0x6db6, 0xbfff, 0x3d56, 0x3577, 0x3576, 0x3576, 0x3576, 0x3576, 0x2d96, 0x2d96,
		0x3596, 0x2d55, 0x3d97, 0x3576, 0x2d96, 0x2d96, 0x3d96, 0x4d35, 0xa659, 0xf7ff, 0xffff, 0xffff, 0xffdf, 0xffff, 0xffff, 0xffff,
		0xfffe, 0xf7ff, 0xefff, 0x8df9, 0x4d36, 0x3556, 0x3576, 0x3596, 0x35b7, 0x2d56, 0x3577, 0x3d77, 0x3d56, 0x6e5a, 0x8659, 0xb7ff,
		0x3d56, 0x3576, 0x3576, 0x3576, 0x3576, 0x3576, 0x2d96, 0x2d96, 0x2d76, 0x3576, 0x3d97, 0x3576, 0x2d76, 0x3596, 0x5e9a, 0x5535,
		0xbefb, 0xf7ff, 0xffff, 0xffff, 0xffff, 0xffdf, 0xffff, 0xffff, 0xfffe, 0xffff, 0xf7ff, 0xbf3e, 0x5515, 0x4556, 0x3576, 0x2d76,
		0x2d75, 0x35b7, 0x2d76, 0x3556, 0x6ebb, 0x5556, 0x8ebb, 0x7659, 0x3d56, 0x3576, 0x3576, 0x3576, 0x3576, 0x3576, 0x3596, 0x2d96,
		0x3596, 0x3596, 0x2d35, 0x3d97, 0x3576, 0x3d75, 0x6ebb, 0x54f4, 0xcf7d, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff,
		0xffff, 0xffff, 0xf7ff, 0xe7ff, 0x8619, 0x44f4, 0x3534, 0x3595, 0x35b6, 0x2d75, 0x3575, 0x4dd7, 0x977e, 0x5d56, 0xbfff, 0x5575,
		0x3d56, 0x3576, 0x3576, 0x3576, 0x3576, 0x3576, 0x3576, 0x3576, 0x2d76, 0x3596, 0x3596, 0x3555, 0x4556, 0x6e59, 0x6e59, 0x6555,
		0xe7ff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xf7ff, 0xefff, 0xd7ff, 0x6535, 0x5dd6, 0x3d34,
		0x3d74, 0x3d75, 0x4595, 0x6659, 0x6db7, 0xc7ff, 0x973d, 0x4514, 0x3d76, 0x3576, 0x3576, 0x3576, 0x3596, 0x3576, 0x3576, 0x3576,
		0x3db7, 0x2d55, 0x3596, 0x3d76, 0x65f8, 0xc7ff, 0x5534, 0x6d34, 0xefff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff,
		0xffdf, 0xffff, 0xffff, 0xf7ff, 0xefff, 0xcfdf, 0x6555, 0x5514, 0x5554, 0x44f3, 0x5554, 0x6596, 0xaf3d, 0xb79f, 0x4d14, 0x3d55,
		0x3576, 0x3576, 0x3576, 0x3576, 0x3596, 0x3576, 0x3576, 0x3576, 0x2d56, 0x3596, 0x3d96, 0x4575, 0x9f5e, 0xaf7e, 0x5cf3, 0xbf5d,
		0xf7ff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xf7df, 0xf7ff, 0xefff, 0xe7ff, 0xcfff,
		0xb77d, 0xbf9e, 0xcfff, 0xdfff, 0xd7ff, 0x6cf4, 0x5576, 0x3d75, 0x3576, 0x3596, 0x3576, 0x3576, 0x3576, 0x3576, 0x3576, 0x3576,
		0x3597, 0x3576, 0x3d55, 0x6e7a, 0x9f3d, 0x5cf3, 0x9e9a, 0xe7ff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff,
		0xffdf, 0xffdf, 0xffff, 0xffff, 0xfffe, 0xffdf, 0xffff, 0xffff, 0xf7ff, 0xefff, 0xe7ff, 0xf7ff, 0xf7ff, 0x8db7, 0x4514, 0x3575,
		0x3596, 0x3596, 0x3576, 0x3576, 0x3576, 0x3576, 0x2d77, 0x3577, 0x3d97, 0x3535, 0x5e19, 0xafff, 0x5d55, 0x6d34, 0xd7ff, 0xefff,
		0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffdf, 0xffff, 0xfffe, 0xfffe, 0xffff, 0xffdf, 0xffdf,
		0xffff, 0xffff, 0xffff, 0xffff, 0xf7ff, 0xe7ff, 0x96fb, 0x4555, 0x3d75, 0x3576, 0x3576, 0x3576, 0x3576, 0x3576, 0x2d97, 0x2d96,
		0x3556, 0x4597, 0x6e19, 0x5d55, 0x6555, 0xcfff, 0xe7ff, 0xf7ff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff,
		0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xf7ff, 0xf7ff, 0xdfdf, 0x8e18,
		0x4d55, 0x3d55, 0x3d76, 0x3596, 0x2d76, 0x2d96, 0x2db7, 0x3596, 0x3555, 0x4d96, 0x5515, 0x7596, 0xc77d, 0xefff, 0xf7ff, 0xffff,
		0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff,
		0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xefff, 0xc7ff, 0x5d96, 0x3cd3, 0x4d96, 0x3d35, 0x3d35, 0x4596, 0x3d35,
		0x5576, 0x4cf4, 0x9edb, 0xe7ff, 0xefff, 0xf7ff, 0xffff, 0xffdf, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff,
		0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xf7df, 0xffff, 0xf7ff,
		0xefff, 0xdfff, 0xbf3d, 0x9e59, 0x8df8, 0x7db7, 0x7dd7, 0x8e39, 0x9e7a, 0xd7df, 0xefff, 0xefff, 0xffff, 0xffff, 0xffdf, 0xffdf,
		0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff,
		0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xf7df, 0xffff, 0xffff, 0xf7ff, 0xefff, 0xefff, 0xefff, 0xefff, 0xefff,
		0xefff, 0xf7ff, 0xf7ff, 0xffff, 0xffff, 0xffde, 0xffde, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff,
		0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffdf, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff,
		0xffff, 0xffde, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xf7ff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff,
		0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff
};

int16_t * TFT_get_image(){
	return &TFT_img;
}

