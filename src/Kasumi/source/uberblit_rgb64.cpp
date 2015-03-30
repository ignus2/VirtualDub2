#include <stdafx.h>
#include "uberblit_rgb64.h"
#include <emmintrin.h>

void VDPixmapGen_X8R8G8B8_To_X16R16G16B16::Compute(void *dst0, sint32 y) {
	uint16 *dst = (uint16 *)dst0;
	const uint8 *src = (const uint8 *)mpSrc->GetRow(y, mSrcIndex);
	sint32 w = mWidth;

	VDCPUCleanupExtensions();

	for(sint32 i=0; i<w; ++i) {
		dst[0] = src[0]<<8;
		dst[1] = src[1]<<8;
		dst[2] = src[2]<<8;
		dst[3] = src[3]<<8;
		dst += 4;
		src += 4;
	}
}

void VDPixmapGen_X16R16G16B16_To_X32B32G32R32F::Compute(void *dst0, sint32 y) {
	VDCPUCleanupExtensions();

	const void* src0 = mpSrc->GetRow(y, mSrcIndex);

	float *dst = (float *)dst0;
	const uint16 *src = (const uint16 *)src0;
	sint32 w = mWidth;

	for(sint32 i=0; i<w; ++i) {
		uint16 r = src[2];
		uint16 g = src[1];
		uint16 b = src[0];
		src += 4;

	  dst[0] = r*mr;
	  dst[1] = g*mg;
	  dst[2] = b*mb;
	  dst[3] = 1.0f;
	  dst += 4;
	}
}

void VDPixmapGen_X32B32G32R32F_To_X16R16G16B16::Compute(void *dst0, sint32 y) {
	VDCPUCleanupExtensions();

	const void* src0 = mpSrc->GetRow(y, mSrcIndex);

	uint16 *dst = (uint16 *)dst0;
	const float *src = (const float *)src0;
	sint32 w = mWidth;

	for(sint32 i=0; i<w; ++i) {
		float r = src[0];
		float g = src[1];
		float b = src[2];
		src += 4;

	  dst[2] = uint16(r*mr);
	  dst[1] = uint16(g*mg);
	  dst[0] = uint16(b*mb);
	  dst[3] = 0;
	  dst += 4;
	}
}

void VDPixmapGen_X16R16G16B16_To_X8R8G8B8::Compute(void *dst0, sint32 y) {
	VDCPUCleanupExtensions();

	const void* src0 = mpSrc->GetRow(y, mSrcIndex);

	if((mWidth & 1)==0 && (size_t(dst0) & 0xF)==0) {
		__m128i *dst = (__m128i *)dst0;
		const __m128i *src = (const __m128i *)src0;
		sint32 w = mWidth/4;

		if(unorm_mode){
			__m128i mm = _mm_set_epi16(0,mr,mg,mb,0,mr,mg,mb);

			for(sint32 i=0; i<w; ++i) {
				__m128i c0 = _mm_load_si128(src);
				__m128i c1 = _mm_load_si128(src+1);
				c0 = _mm_mulhi_epu16(c0,mm);
				c1 = _mm_mulhi_epu16(c1,mm);
				__m128i v = _mm_packus_epi16(c0,c1);
				_mm_store_si128(dst,v);
				src+=2;
				dst++;
			}

		} else {

			for(sint32 i=0; i<w; ++i) {
				__m128i c0 = _mm_load_si128(src);
				__m128i c1 = _mm_load_si128(src+1);
				c0 = _mm_srli_epi16(c0,8);
				c1 = _mm_srli_epi16(c1,8);
				__m128i v = _mm_packus_epi16(c0,c1);
				_mm_store_si128(dst,v);
				src+=2;
				dst++;
			}
		}

	} else {

		uint32 *dst = (uint32 *)dst0;
		const uint16 *src = (const uint16 *)src0;
		sint32 w = mWidth;

		if(unorm_mode){
			for(sint32 i=0; i<w; ++i) {
				uint16 r = src[2];
				uint16 g = src[1];
				uint16 b = src[0];
				src += 4;

				if(r>ref_r) r=255; else r=(r*mr)>>16;
				if(g>ref_g) g=255; else g=(g*mg)>>16;
				if(b>ref_b) b=255; else b=(b*mb)>>16;

				uint32 ir = r << 16;
				uint32 ig = g << 8;
				uint32 ib = b;

				dst[i] = ir + ig + ib;
			}
		} else {
			for(sint32 i=0; i<w; ++i) {
				uint16 r = src[2];
				uint16 g = src[1];
				uint16 b = src[0];
				src += 4;

				uint32 ir = (r>>8) << 16;
				uint32 ig = (g>>8) << 8;
				uint32 ib = (b>>8);

				dst[i] = ir + ig + ib;
			}
		}
	}
}
