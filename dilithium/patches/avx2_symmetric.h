--- upstream/avx2/symmetric.h
+++ upstream-patched/avx2/symmetric.h
@@ -6,21 +6,32 @@
 
 #include "fips202.h"
 
-typedef keccak_state stream128_state;
-typedef keccak_state stream256_state;
+typedef shake128incctx stream128_state;
+typedef shake256incctx stream256_state;
 
 #define dilithium_shake128_stream_init DILITHIUM_NAMESPACE(dilithium_shake128_stream_init)
-void dilithium_shake128_stream_init(keccak_state *state, const uint8_t seed[SEEDBYTES], uint16_t nonce);
+void dilithium_shake128_stream_init(shake128incctx *state,
+                                    const uint8_t seed[SEEDBYTES],
+                                    uint16_t nonce);
 
 #define dilithium_shake256_stream_init DILITHIUM_NAMESPACE(dilithium_shake256_stream_init)
-void dilithium_shake256_stream_init(keccak_state *state, const uint8_t seed[CRHBYTES], uint16_t nonce);
+void dilithium_shake256_stream_init(shake256incctx *state,
+                                    const uint8_t seed[CRHBYTES],
+                                    uint16_t nonce);
 
 #define STREAM128_BLOCKBYTES SHAKE128_RATE
 #define STREAM256_BLOCKBYTES SHAKE256_RATE
 
-#define stream128_init(STATE, SEED, NONCE) dilithium_shake128_stream_init(STATE, SEED, NONCE)
-#define stream128_squeezeblocks(OUT, OUTBLOCKS, STATE) shake128_squeezeblocks(OUT, OUTBLOCKS, STATE)
-#define stream256_init(STATE, SEED, NONCE) dilithium_shake256_stream_init(STATE, SEED, NONCE)
-#define stream256_squeezeblocks(OUT, OUTBLOCKS, STATE) shake256_squeezeblocks(OUT, OUTBLOCKS, STATE)
+#define stream128_init(STATE, SEED, NONCE) \
+        dilithium_shake128_stream_init(STATE, SEED, NONCE)
+#define stream128_squeezeblocks(OUT, OUTBLOCKS, STATE) \
+        shake128_inc_squeeze(OUT, (OUTBLOCKS)*(SHAKE128_RATE), STATE)
+#define stream128_release(STATE) shake128_inc_ctx_release(STATE)
+
+#define stream256_init(STATE, SEED, NONCE) \
+        dilithium_shake256_stream_init(STATE, SEED, NONCE)
+#define stream256_squeezeblocks(OUT, OUTBLOCKS, STATE) \
+        shake256_inc_squeeze(OUT, (OUTBLOCKS)*(SHAKE256_RATE), STATE)
+#define stream256_release(STATE) shake256_inc_ctx_release(STATE)
 
 #endif

