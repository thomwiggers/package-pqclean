--- upstream/ref/indcpa.c
+++ upstream-patched/ref/indcpa.c
@@ -187,6 +187,7 @@
         buflen = off + XOF_BLOCKBYTES;
         ctr += rej_uniform(a[i].vec[j].coeffs + ctr, KYBER_N - ctr, buf, buflen);
       }
+      xof_ctx_release(&state);
     }
   }
 }

