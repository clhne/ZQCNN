#include <stdio.h>
#include <omp.h>
#include "zq_cnn_base_operator.h"
#include "../ZQ_CNN_CompileConfig.h"
#include "zq_cnn_depthwise_convolution_nchwc.h"
#if __ARM_NEON
#include <arm_neon.h>
#else
#if defined(__GNUC__)
#if ZQ_CNN_USE_SSETYPE >= ZQ_CNN_SSETYPE_SSE
#include <smmintrin.h>
#endif
#if ZQ_CNN_USE_SSETYPE >= ZQ_CNN_SSETYPE_AVX
#include <x86intrin.h>
#endif
#elif defined(_WIN32)
#if ZQ_CNN_USE_SSETYPE >= ZQ_CNN_SSETYPE_SSE
#include <mmintrin.h> //MMX  
#include <xmmintrin.h> //SSE(include mmintrin.h)  
#include <emmintrin.h> //SSE2(include xmmintrin.h)  
#include <pmmintrin.h> //SSE3(include emmintrin.h)  
#include <tmmintrin.h>//SSSE3(include pmmintrin.h)  
#include <smmintrin.h>//SSE4.1(include tmmintrin.h)  
#include <nmmintrin.h>//SSE4.2(include smmintrin.h)  
#endif 
#if ZQ_CNN_USE_SSETYPE >= ZQ_CNN_SSETYPE_AVX
#include <wmmintrin.h>//AES(include nmmintrin.h)  
#include <immintrin.h>//AVX(include wmmintrin.h)  
#include <intrin.h>//(include immintrin.h)  
#endif
#endif
#endif //__ARM_NEON

#if defined(__cplusplus) || defined(c_plusplus) 
extern "C" {
#endif

#if __ARM_NEON
#define zq_mm_load_ps vld1q_f32
#define zq_mm_store_ps vst1q_f32
#define zq_mm_add_ps vaddq_f32
#define zq_mm_min_ps vminq_f32
#define zq_mm_max_ps vmaxq_f32
#if ZQ_CNN_USE_FMADD128
#define zq_mm_fmadd_ps(A, B, C) vfmaq_f32(C, A, B)
#else
#define zq_mm_fmadd_ps(A, B, C) vaddq_f32(vmulq_f32(A, B), C)
#endif
#define zq_mm_mul_ps vmulq_f32
#define zq_mm_setzero_ps() vdupq_n_f32(0)
#define zq_mm_type float32x4_t
#define zq_base_type float
#define zq_mm_align_size 4
#define zq_final_sum_q (q[0]+q[1]+q[2]+q[3])

#define zq_cnn_depthwise_conv_no_padding_nchwc_general zq_cnn_depthwise_conv_no_padding_nchwc4_general
#define zq_cnn_depthwise_conv_no_padding_nchwc_kernel3x3 zq_cnn_depthwise_conv_no_padding_nchwc4_kernel3x3
#define zq_cnn_depthwise_conv_no_padding_nchwc_kernel2x2 zq_cnn_depthwise_conv_no_padding_nchwc4_kernel2x2

#define WITH_BIAS 0
#include "zq_cnn_depthwise_convolution_nchwc_raw.h"
#undef WITH_BIAS

#undef zq_cnn_depthwise_conv_no_padding_nchwc_general
#undef zq_cnn_depthwise_conv_no_padding_nchwc_kernel3x3
#undef zq_cnn_depthwise_conv_no_padding_nchwc_kernel2x2

#define zq_cnn_depthwise_conv_no_padding_nchwc_general zq_cnn_depthwise_conv_no_padding_nchwc4_general_with_bias
#define zq_cnn_depthwise_conv_no_padding_nchwc_kernel3x3 zq_cnn_depthwise_conv_no_padding_nchwc4_kernel3x3_with_bias
#define zq_cnn_depthwise_conv_no_padding_nchwc_kernel2x2 zq_cnn_depthwise_conv_no_padding_nchwc4_kernel2x2_with_bias

#define WITH_BIAS 1
#include "zq_cnn_depthwise_convolution_nchwc_raw.h"
#undef WITH_BIAS

#undef zq_cnn_depthwise_conv_no_padding_nchwc_general
#undef zq_cnn_depthwise_conv_no_padding_nchwc_kernel3x3
#undef zq_cnn_depthwise_conv_no_padding_nchwc_kernel2x2

#define zq_cnn_depthwise_conv_no_padding_nchwc_general zq_cnn_depthwise_conv_no_padding_nchwc4_general_with_bias_prelu
#define zq_cnn_depthwise_conv_no_padding_nchwc_kernel3x3 zq_cnn_depthwise_conv_no_padding_nchwc4_kernel3x3_with_bias_prelu
#define zq_cnn_depthwise_conv_no_padding_nchwc_kernel2x2 zq_cnn_depthwise_conv_no_padding_nchwc4_kernel2x2_with_bias_prelu

#define WITH_BIAS 1
#define WITH_PRELU 1
#include "zq_cnn_depthwise_convolution_nchwc_raw.h"
#undef WITH_BIAS
#undef WITH_PRELU

#undef zq_cnn_depthwise_conv_no_padding_nchwc_general
#undef zq_cnn_depthwise_conv_no_padding_nchwc_kernel3x3
#undef zq_cnn_depthwise_conv_no_padding_nchwc_kernel2x2

#undef zq_mm_load_ps
#undef zq_mm_store_ps
#undef zq_mm_add_ps
#undef zq_mm_min_ps
#undef zq_mm_max_ps
#undef zq_mm_fmadd_ps
#undef zq_mm_mul_ps
#undef zq_mm_setzero_ps
#undef zq_mm_type
#undef zq_base_type
#undef zq_mm_align_size
#undef zq_final_sum_q

#else

#if ZQ_CNN_USE_SSETYPE >= ZQ_CNN_SSETYPE_SSE
#define zq_mm_load_ps _mm_load_ps
#define zq_mm_store_ps _mm_store_ps
#define zq_mm_add_ps _mm_add_ps
#define zq_mm_min_ps _mm_min_ps
#define zq_mm_max_ps _mm_max_ps
#if ZQ_CNN_USE_FMADD128
#define zq_mm_fmadd_ps _mm_fmadd_ps
#else
#define zq_mm_fmadd_ps(A, B, C) _mm_add_ps(_mm_mul_ps(A, B), C)
#endif
#define zq_mm_mul_ps _mm_mul_ps
#define zq_mm_setzero_ps _mm_setzero_ps
#define zq_mm_type __m128
#define zq_base_type float
#define zq_mm_align_size 4
#define zq_final_sum_q (q[0]+q[1]+q[2]+q[3])


#define zq_cnn_depthwise_conv_no_padding_nchwc_general zq_cnn_depthwise_conv_no_padding_nchwc4_general
#define zq_cnn_depthwise_conv_no_padding_nchwc_kernel3x3 zq_cnn_depthwise_conv_no_padding_nchwc4_kernel3x3
#define zq_cnn_depthwise_conv_no_padding_nchwc_kernel2x2 zq_cnn_depthwise_conv_no_padding_nchwc4_kernel2x2

#define WITH_BIAS 0
#include "zq_cnn_depthwise_convolution_nchwc_raw.h"
#undef WITH_BIAS

#undef zq_cnn_depthwise_conv_no_padding_nchwc_general
#undef zq_cnn_depthwise_conv_no_padding_nchwc_kernel3x3
#undef zq_cnn_depthwise_conv_no_padding_nchwc_kernel2x2

#define zq_cnn_depthwise_conv_no_padding_nchwc_general zq_cnn_depthwise_conv_no_padding_nchwc4_general_with_bias
#define zq_cnn_depthwise_conv_no_padding_nchwc_kernel3x3 zq_cnn_depthwise_conv_no_padding_nchwc4_kernel3x3_with_bias
#define zq_cnn_depthwise_conv_no_padding_nchwc_kernel2x2 zq_cnn_depthwise_conv_no_padding_nchwc4_kernel2x2_with_bias

#define WITH_BIAS 1
#include "zq_cnn_depthwise_convolution_nchwc_raw.h"
#undef WITH_BIAS

#undef zq_cnn_depthwise_conv_no_padding_nchwc_general
#undef zq_cnn_depthwise_conv_no_padding_nchwc_kernel3x3
#undef zq_cnn_depthwise_conv_no_padding_nchwc_kernel2x2

#define zq_cnn_depthwise_conv_no_padding_nchwc_general zq_cnn_depthwise_conv_no_padding_nchwc4_general_with_bias_prelu
#define zq_cnn_depthwise_conv_no_padding_nchwc_kernel3x3 zq_cnn_depthwise_conv_no_padding_nchwc4_kernel3x3_with_bias_prelu
#define zq_cnn_depthwise_conv_no_padding_nchwc_kernel2x2 zq_cnn_depthwise_conv_no_padding_nchwc4_kernel2x2_with_bias_prelu

#define WITH_BIAS 1
#define WITH_PRELU 1
#include "zq_cnn_depthwise_convolution_nchwc_raw.h"
#undef WITH_BIAS
#undef WITH_PRELU

#undef zq_cnn_depthwise_conv_no_padding_nchwc_general
#undef zq_cnn_depthwise_conv_no_padding_nchwc_kernel3x3
#undef zq_cnn_depthwise_conv_no_padding_nchwc_kernel2x2

#undef zq_mm_load_ps
#undef zq_mm_store_ps
#undef zq_mm_add_ps
#undef zq_mm_min_ps
#undef zq_mm_max_ps
#undef zq_mm_fmadd_ps
#undef zq_mm_mul_ps
#undef zq_mm_setzero_ps
#undef zq_mm_type
#undef zq_base_type
#undef zq_mm_align_size
#undef zq_final_sum_q

#endif//ZQ_CNN_USE_SSETYPE >= ZQ_CNN_SSETYPE_SSE

#if ZQ_CNN_USE_SSETYPE >= ZQ_CNN_SSETYPE_AVX

#define zq_mm_load_ps _mm256_load_ps
#define zq_mm_store_ps _mm256_store_ps
#define zq_mm_add_ps _mm256_add_ps
#define zq_mm_min_ps _mm256_min_ps
#define zq_mm_max_ps _mm256_max_ps
#if ZQ_CNN_USE_FMADD256
#define zq_mm_fmadd_ps _mm256_fmadd_ps
#else
#define zq_mm_fmadd_ps(A, B, C) _mm256_add_ps(_mm256_mul_ps(A, B), C)
#endif
#define zq_mm_mul_ps _mm256_mul_ps
#define zq_mm_setzero_ps _mm256_setzero_ps
#define zq_mm_type __m256
#define zq_base_type float
#define zq_mm_align_size 8
#define zq_final_sum_q (q[0]+q[1]+q[2]+q[3]+q[4]+q[5]+q[6]+q[7])

#define zq_cnn_depthwise_conv_no_padding_nchwc_general zq_cnn_depthwise_conv_no_padding_nchwc8_general
#define zq_cnn_depthwise_conv_no_padding_nchwc_kernel3x3 zq_cnn_depthwise_conv_no_padding_nchwc8_kernel3x3
#define zq_cnn_depthwise_conv_no_padding_nchwc_kernel2x2 zq_cnn_depthwise_conv_no_padding_nchwc8_kernel2x2

#define WITH_BIAS 0
#include "zq_cnn_depthwise_convolution_nchwc_raw.h"
#undef WITH_BIAS

#undef zq_cnn_depthwise_conv_no_padding_nchwc_general
#undef zq_cnn_depthwise_conv_no_padding_nchwc_kernel3x3
#undef zq_cnn_depthwise_conv_no_padding_nchwc_kernel2x2

#define zq_cnn_depthwise_conv_no_padding_nchwc_general zq_cnn_depthwise_conv_no_padding_nchwc8_general_with_bias
#define zq_cnn_depthwise_conv_no_padding_nchwc_kernel3x3 zq_cnn_depthwise_conv_no_padding_nchwc8_kernel3x3_with_bias
#define zq_cnn_depthwise_conv_no_padding_nchwc_kernel2x2 zq_cnn_depthwise_conv_no_padding_nchwc8_kernel2x2_with_bias

#define WITH_BIAS 1
#include "zq_cnn_depthwise_convolution_nchwc_raw.h"
#undef WITH_BIAS

#undef zq_cnn_depthwise_conv_no_padding_nchwc_general
#undef zq_cnn_depthwise_conv_no_padding_nchwc_kernel3x3
#undef zq_cnn_depthwise_conv_no_padding_nchwc_kernel2x2

#define zq_cnn_depthwise_conv_no_padding_nchwc_general zq_cnn_depthwise_conv_no_padding_nchwc8_general_with_bias_prelu
#define zq_cnn_depthwise_conv_no_padding_nchwc_kernel3x3 zq_cnn_depthwise_conv_no_padding_nchwc8_kernel3x3_with_bias_prelu
#define zq_cnn_depthwise_conv_no_padding_nchwc_kernel2x2 zq_cnn_depthwise_conv_no_padding_nchwc8_kernel2x2_with_bias_prelu

#define WITH_BIAS 1
#define WITH_PRELU 1
#include "zq_cnn_depthwise_convolution_nchwc_raw.h"
#undef WITH_BIAS
#undef WITH_PRELU

#undef zq_cnn_depthwise_conv_no_padding_nchwc_general
#undef zq_cnn_depthwise_conv_no_padding_nchwc_kernel3x3
#undef zq_cnn_depthwise_conv_no_padding_nchwc_kernel2x2

#undef zq_mm_load_ps
#undef zq_mm_store_ps
#undef zq_mm_add_ps
#undef zq_mm_min_ps
#undef zq_mm_max_ps
#undef zq_mm_fmadd_ps
#undef zq_mm_mul_ps
#undef zq_mm_setzero_ps
#undef zq_mm_type
#undef zq_base_type
#undef zq_mm_align_size
#undef zq_final_sum_q
#endif //ZQ_CNN_USE_SSETYPE >= ZQ_CNN_SSETYPE_AVX

#endif //__ARM_NEON


#if defined(__cplusplus) || defined(c_plusplus) 
}
#endif