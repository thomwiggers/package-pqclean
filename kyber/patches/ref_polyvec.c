--- upstream/ref/polyvec.c
+++ upstream-patched/ref/polyvec.c
@@ -26,17 +26,17 @@
         t[k]  = ((((uint32_t)t[k] << 11) + KYBER_Q/2)/KYBER_Q) & 0x7ff;
       }
 
-      r[ 0] = (t[0] >>  0);
-      r[ 1] = (t[0] >>  8) | (t[1] << 3);
-      r[ 2] = (t[1] >>  5) | (t[2] << 6);
-      r[ 3] = (t[2] >>  2);
-      r[ 4] = (t[2] >> 10) | (t[3] << 1);
-      r[ 5] = (t[3] >>  7) | (t[4] << 4);
-      r[ 6] = (t[4] >>  4) | (t[5] << 7);
-      r[ 7] = (t[5] >>  1);
-      r[ 8] = (t[5] >>  9) | (t[6] << 2);
-      r[ 9] = (t[6] >>  6) | (t[7] << 5);
-      r[10] = (t[7] >>  3);
+      r[ 0] = (uint8_t)(t[0] >>  0);
+      r[ 1] = (uint8_t)((t[0] >>  8) | (t[1] << 3));
+      r[ 2] = (uint8_t)((t[1] >>  5) | (t[2] << 6));
+      r[ 3] = (uint8_t)(t[2] >>  2);
+      r[ 4] = (uint8_t)((t[2] >> 10) | (t[3] << 1));
+      r[ 5] = (uint8_t)((t[3] >>  7) | (t[4] << 4));
+      r[ 6] = (uint8_t)((t[4] >>  4) | (t[5] << 7));
+      r[ 7] = (uint8_t)(t[5] >>  1);
+      r[ 8] = (uint8_t)((t[5] >>  9) | (t[6] << 2));
+      r[ 9] = (uint8_t)((t[6] >>  6) | (t[7] << 5));
+      r[10] = (uint8_t)(t[7] >>  3);
       r += 11;
     }
   }
@@ -50,11 +50,11 @@
         t[k]  = ((((uint32_t)t[k] << 10) + KYBER_Q/2)/ KYBER_Q) & 0x3ff;
       }
 
-      r[0] = (t[0] >> 0);
-      r[1] = (t[0] >> 8) | (t[1] << 2);
-      r[2] = (t[1] >> 6) | (t[2] << 4);
-      r[3] = (t[2] >> 4) | (t[3] << 6);
-      r[4] = (t[3] >> 2);
+      r[0] = (uint8_t)(t[0] >> 0);
+      r[1] = (uint8_t)((t[0] >> 8) | (t[1] << 2));
+      r[2] = (uint8_t)((t[1] >> 6) | (t[2] << 4));
+      r[3] = (uint8_t)((t[2] >> 4) | (t[3] << 6));
+      r[4] = (uint8_t)(t[3] >> 2);
       r += 5;
     }
   }
@@ -91,8 +91,9 @@
       t[7] = (a[9] >> 5) | ((uint16_t)a[10] << 3);
       a += 11;
 
-      for(k=0;k<8;k++)
+      for(k=0;k<8;k++) {
         r->vec[i].coeffs[8*j+k] = ((uint32_t)(t[k] & 0x7FF)*KYBER_Q + 1024) >> 11;
+      }
     }
   }
 #elif (KYBER_POLYVECCOMPRESSEDBYTES == (KYBER_K * 320))

