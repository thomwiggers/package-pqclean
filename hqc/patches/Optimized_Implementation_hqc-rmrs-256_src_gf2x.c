--- upstream/Optimized_Implementation/hqc-rmrs-256/src/gf2x.c
+++ upstream-patched/Optimized_Implementation/hqc-rmrs-256/src/gf2x.c
@@ -7,42 +7,31 @@
 #include "parameters.h"
 #include <stdint.h>
 #include <string.h>
-#include <stdio.h>
 #include <immintrin.h>
 
 
-#define VEC_N_ARRAY_SIZE_VEC CEIL_DIVIDE(PARAM_N, 256) /*!< The number of needed vectors to store PARAM_N bits*/
-#define WORD 64
-#define LAST64 (PARAM_N >> 6)
-
 //Parameters for Toom-Cook and UB_Karatsuba
 #define T_TM3R_3W (PARAM_N_MULT / 3)
 #define T_TM3R (PARAM_N_MULT + 384)
-#define tTM3R ((T_TM3R) / WORD)
-#define T_TM3R_3W_256 ((T_TM3R_3W + 128) / (4 * WORD))
+#define tTM3R ((T_TM3R) / 64)
+#define T_TM3R_3W_256 ((T_TM3R_3W + 128) / (256))
 #define T_TM3R_3W_64 (T_TM3R_3W_256 << 2)
 
 #define T_5W 4096
 #define T_5W_256 (T_5W >> 8)
 
 #define T2_5W_256 (2 * T_5W_256)
-#define t5 (5 * T_5W / WORD)
+#define t5 (5 * T_5W / 64)
 
-uint64_t a1_times_a2[VEC_N_256_SIZE_64 << 1];
-uint64_t tmp_reduce[VEC_N_ARRAY_SIZE_VEC << 2];
-__m256i *o256 = (__m256i *) tmp_reduce;
-uint64_t bloc64[PARAM_OMEGA_R]; // Allocation with the biggest possible weight
-uint64_t bit64[PARAM_OMEGA_R]; // Allocation with the biggest possible weight
-
-inline static void reduce(uint64_t *o, uint64_t *a);
-inline static void karat_mult_1(__m128i *C, __m128i *A, __m128i *B);
-inline static void karat_mult_2(__m256i *C, __m256i *A, __m256i *B);
-inline static void karat_mult_4(__m256i *C, __m256i *A, __m256i *B);
-inline static void karat_mult_8(__m256i *C, __m256i *A, __m256i *B);
-inline static void karat_mult_16(__m256i *C, __m256i *A, __m256i *B);
-inline static void karat_mult5(__m256i *C, __m256i *A, __m256i *B);
-inline static void divide_by_x_plus_one_256(__m256i *in, __m256i *out, int32_t size);
-static void toom_3_mult(uint64_t *Out, const uint64_t *A, const uint64_t *B);
+static inline void reduce(uint64_t *o, const __m256i *a);
+static inline void karat_mult_1(__m128i *C, const __m128i *A, const __m128i *B);
+static inline void karat_mult_2(__m256i *C, const __m256i *A, const __m256i *B);
+static inline void karat_mult_4(__m256i *C, const __m256i *A, const __m256i *B);
+static inline void karat_mult_8(__m256i *C, const __m256i *A, const __m256i *B);
+static inline void karat_mult_16(__m256i *C, const __m256i *A, const __m256i *B);
+static inline void karat_mult5(__m256i *C, const __m256i *A, const __m256i *B);
+static inline void divide_by_x_plus_one_256(__m256i *in, __m256i *out, int32_t size);
+static void toom_3_mult(uint64_t *Out, const aligned_vec_t *A, const aligned_vec_t *B);
 
 
 /**
@@ -53,40 +42,39 @@
  * @param[out] o Pointer to the result
  * @param[in] a Pointer to the polynomial a(x)
  */
-inline static void reduce(uint64_t *o, uint64_t *a) {
+static inline void reduce(uint64_t *o, const __m256i *a256) {
+	size_t i, i2;
 	__m256i r256, carry256;
-	__m256i *a256 = (__m256i *) a;
-	static const int32_t dec64 = PARAM_N & 0x3f;
-	static const int32_t d0 = WORD - dec64;
-	int32_t i, i2;
-
-	for (i = LAST64 ; i < (PARAM_N >> 5) - 4 ; i += 4) {
-		r256 = _mm256_lddqu_si256((__m256i const *) (& a[i]));
-		r256 = _mm256_srli_epi64(r256, dec64);
-		carry256 = _mm256_lddqu_si256((__m256i const *) (& a[i + 1]));
-		carry256 = _mm256_slli_epi64(carry256, d0);
+	__m256i *o256 = (__m256i *)o;
+	const uint64_t *a64 = (const uint64_t *)a256;
+	uint64_t r, carry;
+
+  i2 = 0;
+	for (i = (PARAM_N >> 6); i < (PARAM_N >> 5) - 4 ; i += 4) {
+		r256 = _mm256_lddqu_si256((const __m256i *) (& a64[i]));
+		r256 = _mm256_srli_epi64(r256, PARAM_N&63);
+		carry256 = _mm256_lddqu_si256((const __m256i *) (& a64[i + 1]));
+		carry256 = _mm256_slli_epi64(carry256, (-PARAM_N)&63);
 		r256 ^= carry256;
-		i2 = (i - LAST64) >> 2;
-		o256[i2] = a256[i2] ^ r256;
+		_mm256_storeu_si256(&o256[i2], a256[i2] ^ r256);
+		i2 += 1;
 	}
 
-	i = i - LAST64;
-
-	for (; i < LAST64 + 1 ; i++) {
-		uint64_t r = a[i + LAST64] >> dec64;
-		uint64_t carry = a[i + LAST64 + 1] << d0;
+	i = i - (PARAM_N >> 6);
+	for (; i < (PARAM_N >> 6) + 1 ; i++) {
+		r = a64[i + (PARAM_N >> 6)] >> (PARAM_N&63);
+		carry = a64[i + (PARAM_N >> 6) + 1] << ((-PARAM_N)&63);
 		r ^= carry;
-		tmp_reduce[i] = a[i] ^ r;
+		o[i] = a64[i] ^ r;
 	}
 
-	tmp_reduce[LAST64] &= RED_MASK;
-	memcpy(o, tmp_reduce, VEC_N_SIZE_BYTES);
+	o[PARAM_N >> 6] &= RED_MASK;
 }
 
 
 
 /**
- * @brief Compute C(x) = A(x)*B(x) 
+ * @brief Compute C(x) = A(x)*B(x)
  * A(x) and B(x) are stored in 128-bit registers
  * This function computes A(x)*B(x) using Karatsuba
  *
@@ -94,7 +82,7 @@
  * @param[in] A Pointer to the polynomial A(x)
  * @param[in] B Pointer to the polynomial B(x)
  */
-inline static void karat_mult_1(__m128i *C, __m128i *A, __m128i *B) {
+static inline void karat_mult_1(__m128i *C, const __m128i *A, const __m128i *B) {
 	__m128i D1[2];
 	__m128i D0[2], D2[2];
 	__m128i Al = _mm_loadu_si128(A);
@@ -102,7 +90,7 @@
 	__m128i Bl = _mm_loadu_si128(B);
 	__m128i Bh = _mm_loadu_si128(B + 1);
 
-	// Computation of Al.Bl=D0 
+	//	Compute Al.Bl=D0
 	__m128i DD0 = _mm_clmulepi64_si128(Al, Bl, 0);
 	__m128i DD2 = _mm_clmulepi64_si128(Al, Bl, 0x11);
 	__m128i AAlpAAh = _mm_xor_si128(Al, _mm_shuffle_epi32(Al, 0x4e));
@@ -111,7 +99,7 @@
 	D0[0] = _mm_xor_si128(DD0, _mm_unpacklo_epi64(_mm_setzero_si128(), DD1));
 	D0[1] = _mm_xor_si128(DD2, _mm_unpackhi_epi64(DD1, _mm_setzero_si128()));
 
-	//	Computation of Ah.Bh=D2
+	//	Compute Ah.Bh=D2
 	DD0 = _mm_clmulepi64_si128(Ah, Bh, 0);
 	DD2 = _mm_clmulepi64_si128(Ah, Bh, 0x11);
 	AAlpAAh = _mm_xor_si128(Ah, _mm_shuffle_epi32(Ah, 0x4e));
@@ -120,11 +108,10 @@
 	D2[0] = _mm_xor_si128(DD0, _mm_unpacklo_epi64(_mm_setzero_si128(), DD1));
 	D2[1] = _mm_xor_si128(DD2, _mm_unpackhi_epi64(DD1, _mm_setzero_si128()));
 
-	// Computation of AlpAh.BlpBh=D1
-	// initialisation of AlpAh and BlpBh
-	__m128i AlpAh = _mm_xor_si128(Al, Ah);
-	__m128i BlpBh = _mm_xor_si128(Bl, Bh);
-
+	// Compute AlpAh.BlpBh=D1
+	// Initialisation of AlpAh and BlpBh
+	__m128i AlpAh = _mm_xor_si128(Al,Ah);
+	__m128i BlpBh = _mm_xor_si128(Bl,Bh);
 	DD0 = _mm_clmulepi64_si128(AlpAh, BlpBh, 0);
 	DD2 = _mm_clmulepi64_si128(AlpAh, BlpBh, 0x11);
 	AAlpAAh = _mm_xor_si128(AlpAh, _mm_shuffle_epi32(AlpAh, 0x4e));
@@ -133,9 +120,8 @@
 	D1[0] = _mm_xor_si128(DD0, _mm_unpacklo_epi64(_mm_setzero_si128(), DD1));
 	D1[1] = _mm_xor_si128(DD2, _mm_unpackhi_epi64(DD1, _mm_setzero_si128()));
 
-	// Computation of C
+	// Final comutation of C
 	__m128i middle = _mm_xor_si128(D0[1], D2[0]);
-
 	C[0] = D0[0];
 	C[1] = middle ^ D0[0] ^ D1[0];
 	C[2] = middle ^ D1[1] ^ D2[1];
@@ -145,7 +131,7 @@
 
 
 /**
- * @brief Compute C(x) = A(x)*B(x) 
+ * @brief Compute C(x) = A(x)*B(x)
  *
  * This function computes A(x)*B(x) using Karatsuba
  * A(x) and B(x) are stored in 256-bit registers
@@ -153,28 +139,29 @@
  * @param[in] A Pointer to the polynomial A(x)
  * @param[in] B Pointer to the polynomial B(x)
  */
-inline static void karat_mult_2(__m256i *C, __m256i *A, __m256i *B) {
+static inline void karat_mult_2(__m256i *C, const __m256i *A, const __m256i *B) {
 	__m256i D0[2], D1[2], D2[2], SAA, SBB;
-	__m128i *A128 = (__m128i *)A, *B128 = (__m128i *)B;
+	const __m128i *A128 = (const __m128i *)A;
+	const __m128i *B128 = (const __m128i *)B;
+	__m256i middle;
 
 	karat_mult_1((__m128i *) D0, A128, B128);
 	karat_mult_1((__m128i *) D2, A128 + 2, B128 + 2);
+
 	SAA = A[0] ^ A[1];
 	SBB = B[0] ^ B[1];
 	karat_mult_1((__m128i *) D1,(__m128i *) &SAA,(__m128i *) &SBB);
-	
-	__m256i middle = _mm256_xor_si256(D0[1], D2[0]);
+	middle = _mm256_xor_si256(D0[1], D2[0]);
 
 	C[0] = D0[0];
 	C[1] = middle ^ D0[0] ^ D1[0];
 	C[2] = middle ^ D1[1] ^ D2[1];
 	C[3] = D2[1];
 }
-	
 
 
 /**
- * @brief Compute C(x) = A(x)*B(x) 
+ * @brief Compute C(x) = A(x)*B(x)
  *
  * This function computes A(x)*B(x) using Karatsuba
  * A(x) and B(x) are stored in 256-bit registers
@@ -182,20 +169,24 @@
  * @param[in] A Pointer to the polynomial A(x)
  * @param[in] B Pointer to the polynomial B(x)
  */
-inline static void karat_mult_4(__m256i *C, __m256i *A, __m256i *B) {
+static inline void karat_mult_4(__m256i *C, const __m256i *A, const __m256i *B) {
 	__m256i D0[4], D1[4], D2[4], SAA[2], SBB[2];
-			
-	karat_mult_2(D0, A,B);
+	__m256i middle0;
+	__m256i middle1;
+
+	karat_mult_2(D0, A, B);
 	karat_mult_2(D2, A + 2, B + 2);
+
 	SAA[0] = A[0] ^ A[2];
 	SBB[0] = B[0] ^ B[2];
 	SAA[1] = A[1] ^ A[3];
 	SBB[1] = B[1] ^ B[3];
+
 	karat_mult_2(D1, SAA, SBB);
-	
-	__m256i middle0 = _mm256_xor_si256(D0[2], D2[0]);
-	__m256i middle1 = _mm256_xor_si256(D0[3], D2[1]);
-	
+
+	middle0 = _mm256_xor_si256(D0[2], D2[0]);
+	middle1 = _mm256_xor_si256(D0[3], D2[1]);
+
 	C[0] = D0[0];
 	C[1] = D0[1];
 	C[2] = middle0 ^ D0[0] ^ D1[0];
@@ -204,12 +195,12 @@
 	C[5] = middle1 ^ D1[3] ^ D2[3];
 	C[6] = D2[2];
 	C[7] = D2[3];
-}	
+}
 
 
 
 /**
- * @brief Compute C(x) = A(x)*B(x) 
+ * @brief Compute C(x) = A(x)*B(x)
  *
  * This function computes A(x)*B(x) using Karatsuba
  * A(x) and B(x) are stored in 256-bit registers
@@ -217,26 +208,29 @@
  * @param[in] A Pointer to the polynomial A(x)
  * @param[in] B Pointer to the polynomial B(x)
  */
-inline static void karat_mult_8(__m256i *C, __m256i *A, __m256i *B) {
+static inline void karat_mult_8(__m256i *C, const __m256i *A, const __m256i *B) {
+	size_t i, is, is2, is3;
 	__m256i D0[8], D1[8], D2[8], SAA[4], SBB[4];
-			
+	__m256i middle;
+
 	karat_mult_4(D0, A, B);
 	karat_mult_4(D2, A + 4, B + 4);
-	for(int32_t i = 0 ; i < 4 ; i++) {
-		int32_t is = i + 4; 
+
+	for (i = 0 ; i < 4 ; i++) {
+		is = i + 4;
 		SAA[i] = A[i] ^ A[is];
 		SBB[i] = B[i] ^ B[is];
 	}
 
 	karat_mult_4(D1, SAA, SBB);
 
-	for(int32_t i = 0 ; i < 4 ; i++) {
-		int32_t is = i + 4;
-		int32_t is2 = is + 4;
-		int32_t is3 = is2 + 4;
-		
-		__m256i middle = _mm256_xor_si256(D0[is], D2[i]);
-		
+	for (i = 0 ; i < 4 ; i++) {
+		is = i + 4;
+		is2 = is + 4;
+		is3 = is2 + 4;
+
+		middle = _mm256_xor_si256(D0[is], D2[i]);
+
 		C[i]   = D0[i];
 		C[is]  = middle ^ D0[i] ^ D1[i];
 		C[is2] = middle ^ D1[is] ^ D2[is];
@@ -247,7 +241,7 @@
 
 
 /**
- * @brief Compute C(x) = A(x)*B(x) 
+ * @brief Compute C(x) = A(x)*B(x)
  *
  * This function computes A(x)*B(x) using Karatsuba
  * A(x) and B(x) are stored in 256-bit registers
@@ -255,27 +249,29 @@
  * @param[in] A Pointer to the polynomial A(x)
  * @param[in] B Pointer to the polynomial B(x)
  */
-inline static void karat_mult_16(__m256i *C, __m256i *A, __m256i *B) {
+inline static void karat_mult_16(__m256i *C, const __m256i *A, const __m256i *B) {
+	size_t i, is, is2, is3;
+	__m256i middle;
 	__m256i D0[16], D1[16], D2[16], SAA[8], SBB[8];
-			
+
 	karat_mult_8(D0, A, B);
 	karat_mult_8(D2, A + 8, B + 8);
 
-	for(int32_t i = 0 ; i < 8 ; i++) {
-		int32_t is = i + 8; 
+	for (i = 0 ; i < 8 ; i++) {
+		is = i + 8;
 		SAA[i] = A[i] ^ A[is];
 		SBB[i] = B[i] ^ B[is];
 	}
 
 	karat_mult_8(D1, SAA, SBB);
 
-	for(int32_t i = 0 ; i < 8 ; i++) {
-		int32_t is = i + 8;
-		int32_t is2 = is + 8;
-		int32_t is3 = is2 + 8;
-		
-		__m256i middle = _mm256_xor_si256(D0[is], D2[i]);
-		
+	for (i = 0 ; i < 8 ; i++) {
+		is = i + 8;
+		is2 = is + 8;
+		is3 = is2 + 8;
+
+		middle = D0[is] ^ D2[i];
+
 		C[i]   = D0[i];
 		C[is]  = middle ^ D0[i] ^ D1[i];
 		C[is2] = middle ^ D1[is] ^ D2[is];
@@ -284,9 +280,8 @@
 }
 
 
-
 /**
- * @brief Compute C(x) = A(x)*B(x) 
+ * @brief Compute C(x) = A(x)*B(x)
  *
  * This function computes A(x)*B(x) using Karatsuba
  * A(x) and B(x) are stored in 256-bit registers
@@ -294,15 +289,15 @@
  * @param[in] A Pointer to the polynomial A(x)
  * @param[in] B Pointer to the polynomial B(x)
  */
-inline static void karat_mult5(__m256i *Out, __m256i *A, __m256i *B) {
-	__m256i *a0, *b0, *a1, *b1, *a2, *b2, * a3, * b3, *a4, *b4;
+static inline void karat_mult5(__m256i *C, const __m256i *A, const __m256i *B) {
+	const __m256i *a0, *b0, *a1, *b1, *a2, *b2, * a3, * b3, *a4, *b4;
 	
-	static __m256i aa01[T_5W_256], bb01[T_5W_256], aa02[T_5W_256], bb02[T_5W_256], aa03[T_5W_256], bb03[T_5W_256], aa04[T_5W_256], bb04[T_5W_256], 
+	__m256i aa01[T_5W_256], bb01[T_5W_256], aa02[T_5W_256], bb02[T_5W_256], aa03[T_5W_256], bb03[T_5W_256], aa04[T_5W_256], bb04[T_5W_256], 
 			aa12[T_5W_256], bb12[T_5W_256], aa13[T_5W_256], bb13[T_5W_256], aa14[T_5W_256], bb14[T_5W_256],
 			aa23[T_5W_256], bb23[T_5W_256], aa24[T_5W_256], bb24[T_5W_256],
 			aa34[T_5W_256], bb34[T_5W_256];
 	
-	static __m256i D0[T2_5W_256], D1[T2_5W_256], D2[T2_5W_256], D3[T2_5W_256], D4[T2_5W_256], 
+	__m256i D0[T2_5W_256], D1[T2_5W_256], D2[T2_5W_256], D3[T2_5W_256], D4[T2_5W_256], 
 			D01[T2_5W_256], D02[T2_5W_256], D03[T2_5W_256], D04[T2_5W_256],
 			D12[T2_5W_256], D13[T2_5W_256], D14[T2_5W_256],
 			D23[T2_5W_256], D24[T2_5W_256],
@@ -387,7 +382,7 @@
 	}
 
 	for(int32_t i = 0 ; i < T_5W_256 * 10 ; i++) {
-		Out[i] = ro256[i];
+		C[i] = ro256[i];
 	}
 }
 
@@ -418,22 +413,22 @@
  * @param[in] A Pointer to the polynomial A(x)
  * @param[in] B Pointer to the polynomial B(x)
  */
-static void toom_3_mult(uint64_t *Out, const uint64_t *A, const uint64_t *B) {
-	static __m256i U0[T_TM3R_3W_256 + 2], V0[T_TM3R_3W_256 + 2], U1[T_TM3R_3W_256 + 2], V1[T_TM3R_3W_256 + 2], U2[T_TM3R_3W_256 + 2], V2[T_TM3R_3W_256 + 2];
-	static __m256i W0[2 * (T_TM3R_3W_256 + 2)], W1[2 * (T_TM3R_3W_256 + 2)], W2[2 * (T_TM3R_3W_256 + 2)], W3[2 * (T_TM3R_3W_256 + 2)], W4[2 * (T_TM3R_3W_256 + 2)];
-	static __m256i tmp[2 * (T_TM3R_3W_256 + 2) + 3];
-	static __m256i ro256[tTM3R / 2];
-	static const __m256i zero = {0ul, 0ul, 0ul, 0ul};
+static void toom_3_mult(uint64_t *Out, const aligned_vec_t *A, const aligned_vec_t *B) {
+	__m256i U0[T_TM3R_3W_256 + 2], V0[T_TM3R_3W_256 + 2], U1[T_TM3R_3W_256 + 2], V1[T_TM3R_3W_256 + 2], U2[T_TM3R_3W_256 + 2], V2[T_TM3R_3W_256 + 2];
+	__m256i W0[2 * (T_TM3R_3W_256 + 2)], W1[2 * (T_TM3R_3W_256 + 2)], W2[2 * (T_TM3R_3W_256 + 2)], W3[2 * (T_TM3R_3W_256 + 2)], W4[2 * (T_TM3R_3W_256 + 2)];
+	__m256i tmp[2 * (T_TM3R_3W_256 + 2) + 3];
+	__m256i ro256[tTM3R / 2];
+	const __m256i zero = {0ul, 0ul, 0ul, 0ul};
 	int32_t T2 = T_TM3R_3W_64 << 1;
 
 	for (int32_t i = 0 ; i < T_TM3R_3W_256 ; i++) {
 		int32_t i4 = i << 2;
-		U0[i]= _mm256_lddqu_si256((__m256i const *)(& A[i4]));
-		V0[i]= _mm256_lddqu_si256((__m256i const *)(& B[i4]));
-		U1[i]= _mm256_lddqu_si256((__m256i const *)(& A[i4 + T_TM3R_3W_64]));
-		V1[i]= _mm256_lddqu_si256((__m256i const *)(& B[i4 + T_TM3R_3W_64]));
-		U2[i]= _mm256_lddqu_si256((__m256i const *)(& A[i4 + T2]));
-		V2[i]= _mm256_lddqu_si256((__m256i const *)(& B[i4 + T2]));
+		U0[i]= _mm256_lddqu_si256((__m256i const *)(&A->arr64[i4]));
+		V0[i]= _mm256_lddqu_si256((__m256i const *)(&B->arr64[i4]));
+		U1[i]= _mm256_lddqu_si256((__m256i const *)(&A->arr64[i4 + T_TM3R_3W_64]));
+		V1[i]= _mm256_lddqu_si256((__m256i const *)(&B->arr64[i4 + T_TM3R_3W_64]));
+		U2[i]= _mm256_lddqu_si256((__m256i const *)(&A->arr64[i4 + T2]));
+		V2[i]= _mm256_lddqu_si256((__m256i const *)(&B->arr64[i4 + T2]));
 	}
 
 	for (int32_t i = T_TM3R_3W_256 ; i < T_TM3R_3W_256 + 2 ; i++)	{
@@ -599,14 +594,8 @@
  * @param[in] a1 Pointer to a polynomial
  * @param[in] a2 Pointer to a polynomial
  */
-void vect_mul(uint64_t *o, const uint64_t *a1, const uint64_t *a2) {	
-	toom_3_mult(a1_times_a2, a1, a2);
+void vect_mul(uint64_t *o, const aligned_vec_t *a1, const aligned_vec_t *a2) {
+	__m256i a1_times_a2[CEIL_DIVIDE(2*PARAM_N_MULT + 1,256)] = {0};
+	toom_3_mult((uint64_t *)a1_times_a2, a1, a2);
 	reduce(o, a1_times_a2);
-
-	// clear all
-	#ifdef __STDC_LIB_EXT1__
-		memset_s(a1_times_a2, 0, (VEC_N_SIZE_64 << 1) * sizeof(uint64_t));
-	#else
-		memset(a1_times_a2, 0, (VEC_N_SIZE_64 << 1) * sizeof(uint64_t));
-	#endif
 }

