--- hqc-2020-05-29/Reference_Implementation/hqc-256/src/gf.c
+++ hqc-2020-05-29-patched/Reference_Implementation/hqc-256/src/gf.c
@@ -7,125 +7,57 @@
 #include "parameters.h"
 #include <stdint.h>
 
-
-/**
- * Powers of the root alpha of x^10 + x^3 + 1.
- * The last two elements are needed by the gf_mul function from gf_mul.c
- * (for example if both elements to multiply are zero).
- */
-static const uint16_t exp[1026] = {
-	1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 9, 18, 36, 72, 144, 288, 576, 137, 274, 548, 65, 130, 260, 520, 25, 50, 100, 200, 400, 800, 585, 155, 310, 620, 209, 418, 836, 641, 267, 534, 37, 74, 148, 296, 592, 169, 338, 676, 321, 642, 269, 538, 61, 122, 244, 488, 976, 937, 859, 703, 375, 750, 469, 938, 861, 691, 367, 734, 437, 874, 733, 435, 870, 709, 387, 774, 517, 3, 6, 12, 24, 48, 96, 192, 384, 768, 521, 27, 54, 108, 216, 432, 864, 713, 411, 822, 613, 195, 390, 780, 529, 43, 86, 172, 344, 688, 361, 722, 429, 858, 701, 371, 742, 453, 906, 797, 563, 111, 222, 444, 888, 761, 507, 1014, 997, 963, 911, 791, 551, 71, 142, 284, 568, 121, 242, 484, 968, 921, 827, 639, 247, 494, 988, 945, 875, 735, 439, 878, 725, 419, 838, 645, 259, 518, 5, 10, 20, 40, 80, 160, 320, 640, 265, 530, 45, 90, 180, 360, 720, 425, 850, 685, 339, 678, 325, 650, 285, 570, 125, 250, 500, 1000, 985, 955, 895, 759, 487, 974, 917, 803, 591, 151, 302, 604, 177, 354, 708, 385, 770, 525, 19, 38, 76, 152, 304, 608, 201, 402, 804, 577, 139, 278, 556, 81, 162, 324, 648, 281, 562, 109, 218, 436, 872, 729, 443, 886, 741, 451, 902, 773, 515, 15, 30, 60, 120, 240, 480, 960, 905, 795, 575, 119, 238, 476, 952, 889, 763, 511, 1022, 1013, 995, 975, 919, 807, 583, 135, 270, 540, 49, 98, 196, 392, 784, 553, 91, 182, 364, 728, 441, 882, 749, 467, 934, 837, 643, 271, 542, 53, 106, 212, 424, 848, 681, 347, 694, 357, 714, 413, 826, 637, 243, 486, 972, 913, 811, 607, 183, 366, 732, 433, 866, 717, 403, 806, 581, 131, 262, 524, 17, 34, 68, 136, 272, 544, 73, 146, 292, 584, 153, 306, 612, 193, 386, 772, 513, 11, 22, 44, 88, 176, 352, 704, 393, 786, 557, 83, 166, 332, 664, 313, 626, 237, 474, 948, 865, 715, 415, 830, 629, 227, 454, 908, 785, 555, 95, 190, 380, 760, 505, 1010, 1005, 979, 943, 855, 679, 327, 654, 277, 554, 93, 186, 372, 744, 473, 946, 877, 723, 431, 862, 693, 355, 710, 389, 778, 541, 51, 102, 204, 408, 816, 617, 219, 438, 876, 721, 427, 854, 677, 323, 646, 261, 522, 29, 58, 116, 232, 464, 928, 841, 667, 319, 638, 245, 490, 980, 929, 843, 671, 311, 622, 213, 426, 852, 673, 331, 662, 293, 586, 157, 314, 628, 225, 450, 900, 769, 523, 31, 62, 124, 248, 496, 992, 969, 923, 831, 631, 231, 462, 924, 817, 619, 223, 446, 892, 753, 491, 982, 933, 835, 655, 279, 558, 85, 170, 340, 680, 345, 690, 365, 730, 445, 890, 765, 499, 998, 965, 899, 783, 535, 39, 78, 156, 312, 624, 233, 466, 932, 833, 651, 287, 574, 117, 234, 468, 936, 857, 699, 383, 766, 501, 1002, 989, 947, 879, 727, 423, 846, 661, 291, 582, 133, 266, 532, 33, 66, 132, 264, 528, 41, 82, 164, 328, 656, 297, 594, 173, 346, 692, 353, 706, 397, 794, 573, 115, 230, 460, 920, 825, 635, 255, 510, 1020, 1009, 1003, 991, 951, 871, 711, 391, 782, 533, 35, 70, 140, 280, 560, 105, 210, 420, 840, 665, 315, 630, 229, 458, 916, 801, 587, 159, 318, 636, 241, 482, 964, 897, 779, 543, 55, 110, 220, 440, 880, 745, 475, 950, 869, 707, 399, 798, 565, 99, 198, 396, 792, 569, 123, 246, 492, 984, 953, 891, 767, 503, 1006, 981, 931, 847, 663, 295, 590, 149, 298, 596, 161, 322, 644, 257, 514, 13, 26, 52, 104, 208, 416, 832, 649, 283, 566, 101, 202, 404, 808, 601, 187, 374, 748, 465, 930, 845, 659, 303, 606, 181, 362, 724, 417, 834, 653, 275, 550, 69, 138, 276, 552, 89, 178, 356, 712, 409, 818, 621, 211, 422, 844, 657, 299, 598, 165, 330, 660, 289, 578, 141, 282, 564, 97, 194, 388, 776, 537, 59, 118, 236, 472, 944, 873, 731, 447, 894, 757, 483, 966, 901, 771, 527, 23, 46, 92, 184, 368, 736, 457, 914, 813, 595, 175, 350, 700, 369, 738, 461, 922, 829, 627, 239, 478, 956, 881, 747, 479, 958, 885, 739, 463, 926, 821, 611, 207, 414, 828, 625, 235, 470, 940, 849, 683, 351, 702, 373, 746, 477, 954, 893, 755, 495, 990, 949, 867, 719, 407, 814, 597, 163, 326, 652, 273, 546, 77, 154, 308, 616, 217, 434, 868, 705, 395, 790, 549, 67, 134, 268, 536, 57, 114, 228, 456, 912, 809, 603, 191, 382, 764, 497, 994, 973, 915, 815, 599, 167, 334, 668, 305, 610, 205, 410, 820, 609, 203, 406, 812, 593, 171, 342, 684, 337, 674, 333, 666, 317, 634, 253, 506, 1012, 993, 971, 927, 823, 615, 199, 398, 796, 561, 107, 214, 428, 856, 697, 379, 758, 485, 970, 925, 819, 623, 215, 430, 860, 689, 363, 726, 421, 842, 669, 307, 614, 197, 394, 788, 545, 75, 150, 300, 600, 185, 370, 740, 449, 898, 781, 531, 47, 94, 188, 376, 752, 489, 978, 941, 851, 687, 343, 686, 341, 682, 349, 698, 381, 762, 509, 1018, 1021, 1011, 1007, 983, 935, 839, 647, 263, 526, 21, 42, 84, 168, 336, 672, 329, 658, 301, 602, 189, 378, 756, 481, 962, 909, 787, 559, 87, 174, 348, 696, 377, 754, 493, 986, 957, 883, 751, 471, 942, 853, 675, 335, 670, 309, 618, 221, 442, 884, 737, 459, 918, 805, 579, 143, 286, 572, 113, 226, 452, 904, 793, 571, 127, 254, 508, 1016, 1017, 1019, 1023, 1015, 999, 967, 903, 775, 519, 7, 14, 28, 56, 112, 224, 448, 896, 777, 539, 63, 126, 252, 504, 1008, 1001, 987, 959, 887, 743, 455, 910, 789, 547, 79, 158, 316, 632, 249, 498, 996, 961, 907, 799, 567, 103, 206, 412, 824, 633, 251, 502, 1004, 977, 939, 863, 695, 359, 718, 405, 810, 605, 179, 358, 716, 401, 802, 589, 147, 294, 588, 145, 290, 580, 129, 258, 516, 1, 2, 4
-};
-
-
-
-/**
- * Logarithm of elements of GF(2^10) to the base alpha (root of x^10 + x^3 + 1).
- * The logarithm of 0 is set to 1024 by convention.
- */
-static const uint16_t log[1024] = {
-	1024, 0, 1, 77, 2, 154, 78, 956, 3, 10, 155, 325, 79, 618, 957, 231, 4, 308, 11, 200, 156, 889, 326, 695, 80, 24, 619, 87, 958, 402, 232, 436, 5, 513, 309, 551, 12, 40, 201, 479, 157, 518, 890, 101, 327, 164, 696, 860, 81, 258, 25, 385, 620, 277, 88, 577, 959, 772, 403, 680, 233, 52, 437, 966, 6, 20, 514, 768, 310, 650, 552, 129, 13, 314, 41, 849, 202, 757, 480, 980, 158, 213, 519, 335, 891, 462, 102, 907, 328, 654, 165, 264, 697, 369, 861, 354, 82, 675, 259, 590, 26, 628, 386, 991, 621, 556, 278, 822, 89, 219, 578, 117, 960, 937, 773, 533, 404, 491, 681, 241, 234, 133, 53, 595, 438, 178, 967, 943, 7, 1020, 21, 305, 515, 510, 769, 255, 311, 17, 651, 210, 553, 672, 130, 934, 14, 1017, 315, 1014, 42, 610, 850, 191, 203, 318, 758, 31, 481, 428, 981, 568, 159, 613, 214, 752, 520, 667, 336, 788, 892, 45, 463, 801, 103, 525, 908, 705, 329, 194, 655, 1008, 166, 642, 265, 296, 698, 853, 370, 633, 862, 899, 355, 779, 83, 321, 676, 97, 260, 845, 591, 818, 27, 206, 629, 797, 387, 793, 992, 727, 622, 34, 557, 661, 279, 420, 823, 834, 90, 761, 220, 391, 579, 926, 118, 451, 961, 431, 938, 349, 774, 563, 534, 446, 405, 484, 492, 731, 682, 341, 242, 714, 235, 571, 134, 290, 54, 412, 596, 140, 439, 984, 179, 996, 968, 810, 944, 539, 8, 616, 1021, 152, 22, 400, 306, 887, 516, 162, 511, 38, 770, 50, 256, 275, 312, 755, 18, 648, 652, 367, 211, 460, 554, 217, 673, 626, 131, 176, 935, 489, 15, 670, 1018, 508, 316, 426, 1015, 608, 43, 523, 611, 665, 851, 897, 192, 640, 204, 791, 319, 843, 759, 924, 32, 418, 482, 339, 429, 561, 982, 808, 569, 410, 160, 48, 614, 398, 215, 174, 753, 365, 521, 895, 668, 424, 337, 806, 789, 922, 893, 804, 46, 172, 464, 872, 802, 870, 104, 466, 526, 283, 909, 874, 706, 736, 330, 528, 195, 380, 656, 285, 1009, 1003, 167, 106, 643, 838, 266, 468, 297, 66, 699, 708, 854, 111, 371, 738, 634, 60, 863, 911, 900, 827, 356, 876, 780, 497, 84, 197, 322, 74, 677, 382, 98, 548, 261, 332, 846, 765, 592, 530, 819, 587, 28, 1011, 207, 302, 630, 1005, 798, 749, 388, 658, 794, 94, 993, 287, 728, 346, 623, 645, 35, 149, 558, 840, 662, 505, 280, 169, 421, 395, 824, 108, 835, 377, 91, 299, 762, 71, 221, 68, 392, 146, 580, 268, 927, 224, 119, 470, 452, 687, 962, 856, 432, 227, 939, 113, 350, 976, 775, 701, 564, 930, 535, 710, 447, 723, 406, 636, 485, 271, 493, 62, 732, 918, 683, 373, 342, 583, 243, 740, 715, 719, 236, 902, 572, 690, 135, 829, 291, 186, 55, 865, 413, 455, 597, 913, 141, 744, 440, 782, 985, 473, 180, 499, 997, 602, 969, 358, 811, 122, 945, 878, 540, 247, 9, 324, 617, 230, 1022, 76, 153, 955, 23, 86, 401, 435, 307, 199, 888, 694, 517, 100, 163, 859, 512, 550, 39, 478, 771, 679, 51, 965, 257, 384, 276, 576, 313, 848, 756, 979, 19, 767, 649, 128, 653, 263, 368, 353, 212, 334, 461, 906, 555, 821, 218, 116, 674, 589, 627, 990, 132, 594, 177, 942, 936, 532, 490, 240, 16, 209, 671, 933, 1019, 304, 509, 254, 317, 30, 427, 567, 1016, 1013, 609, 190, 44, 800, 524, 704, 612, 751, 666, 787, 852, 632, 898, 778, 193, 1007, 641, 295, 205, 796, 792, 726, 320, 96, 844, 817, 760, 390, 925, 450, 33, 660, 419, 833, 483, 730, 340, 713, 430, 348, 562, 445, 983, 995, 809, 538, 570, 289, 411, 139, 161, 37, 49, 274, 615, 151, 399, 886, 216, 625, 175, 488, 754, 647, 366, 459, 522, 664, 896, 639, 669, 507, 425, 607, 338, 560, 807, 409, 790, 842, 923, 417, 894, 423, 805, 921, 47, 397, 173, 364, 465, 282, 873, 735, 803, 171, 871, 869, 105, 837, 467, 65, 527, 379, 284, 1002, 910, 826, 875, 496, 707, 110, 737, 59, 331, 764, 529, 586, 196, 73, 381, 547, 657, 93, 286, 345, 1010, 301, 1004, 748, 168, 394, 107, 376, 644, 148, 839, 504, 267, 223, 469, 686, 298, 70, 67, 145, 700, 929, 709, 722, 855, 226, 112, 975, 372, 582, 739, 718, 635, 270, 61, 917, 864, 454, 912, 743, 901, 689, 828, 185, 357, 121, 877, 246, 781, 472, 498, 601, 85, 434, 198, 693, 323, 229, 75, 954, 678, 964, 383, 575, 99, 858, 549, 477, 262, 352, 333, 905, 847, 978, 766, 127, 593, 941, 531, 239, 820, 115, 588, 989, 29, 566, 1012, 189, 208, 932, 303, 253, 631, 777, 1006, 294, 799, 703, 750, 786, 389, 449, 659, 832, 795, 725, 95, 816, 994, 537, 288, 138, 729, 712, 347, 444, 624, 487, 646, 458, 36, 273, 150, 885, 559, 408, 841, 416, 663, 638, 506, 606, 281, 734, 170, 868, 422, 920, 396, 363, 825, 495, 109, 58, 836, 64, 378, 1001, 92, 344, 300, 747, 763, 585, 72, 546, 222, 685, 69, 144, 393, 375, 147, 503, 581, 717, 269, 916, 928, 721, 225, 974, 120, 245, 471, 600, 453, 742, 688, 184, 963, 574, 857, 476, 433, 692, 228, 953, 940, 238, 114, 988, 351, 904, 977, 126, 776, 293, 702, 785, 565, 188, 931, 252, 536, 137, 711, 443, 448, 831, 724, 815, 407, 415, 637, 605, 486, 457, 272, 884, 494, 57, 63, 1000, 733, 867, 919, 362, 684, 143, 374, 502, 343, 746, 584, 545, 244, 599, 741, 183, 716, 915, 720, 973, 237, 987, 903, 125, 573, 475, 691, 952, 136, 442, 830, 814, 292, 784, 187, 251, 56, 999, 866, 361, 414, 604, 456, 883, 598, 182, 914, 972, 142, 501, 745, 544, 441, 813, 783, 250, 986, 124, 474, 951, 181, 971, 500, 543, 998, 360, 603, 882, 970, 542, 359, 881, 812, 249, 123, 950, 946, 947, 879, 948, 541, 880, 248, 949
-};
-
-
-
-/**
- * @brief Generates exp and log lookup tables of GF(2^m).
- *
- * The logarithm of 0 is defined as 2^PARAM_M by convention. <br>
- * The last two elements of the exp table are needed by the gf_mul function from gf_lutmul.c
- * (for example if both elements to multiply are zero).
- * @param[out] exp Array of size 2^PARAM_M + 2 receiving the powers of the primitive element
- * @param[out] log Array of size 2^PARAM_M receiving the logarithms of the elements of GF(2^m)
- * @param[in] m Parameter of Galois field GF(2^m)
- */
-void gf_generate(uint16_t *exp, uint16_t *log, const int16_t m) {
-	uint16_t elt = 1;
-	uint16_t alpha = 2; // primitive element of GF(2^PARAM_M)
-	uint16_t gf_poly = PARAM_GF_POLY;
-
-	for (size_t i = 0 ; i < (1U << m) - 1 ; ++i){
-		exp[i] = elt;
-		log[elt] = i;
-
-		elt *= alpha;
-		if(elt >= 1 << m)
-			elt ^= gf_poly;
-	}
-
-	exp[(1 << m) - 1] = 1;
-	exp[1 << m] = 2;
-	exp[(1 << m) + 1] = 4;
-	log[0] = 1 << m; // by convention
-}
-
-
-
-/**
- * @brief Returns the integer i such that elt = a^i where a is the primitive element of GF(2^PARAM_M).
- *
- * @returns the logarithm of the given element
- */
-uint16_t gf_log(uint16_t elt) {
-	return log[elt];
-}
-
-
-
 /**
  * @brief Multiplies nonzero element a by element b
- *
  * @returns the product a*b
  * @param[in] a First element of GF(2^PARAM_M) to multiply (cannot be zero)
  * @param[in] b Second element of GF(2^PARAM_M) to multiply (cannot be zero)
  */
 uint16_t gf_mul(uint16_t a, uint16_t b) {
-	// mask = 0xffff if neither a nor b is zero. Otherwise mask is 0.
-	int16_t mask = ((log[a] | log[b]) >> PARAM_M) - 1;
-	return mask & exp[gf_mod(log[a] + log[b])];
+  uint16_t mask;
+  mask = (uint16_t) (-((int32_t) a) >> 31); // a != 0
+  mask &= (uint16_t) (-((int32_t) b) >> 31); // b != 0
+	return mask & gf_exp[gf_mod(gf_log[a] + gf_log[b])];
 }
 
 
 
 /**
  * @brief Squares an element of GF(2^PARAM_M)
- *
  * @returns a^2
  * @param[in] a Element of GF(2^PARAM_M)
  */
 uint16_t gf_square(uint16_t a) {
-	int16_t mask = (log[a] >> PARAM_M) - 1;
-	return mask & exp[gf_mod(2 * log[a])];
+	int16_t mask = (uint16_t) (-((int32_t) a) >> 31); // a != 0
+	return mask & gf_exp[gf_mod(2 * gf_log[a])];
 }
 
 
 
 /**
  * @brief Computes the inverse of an element of GF(2^PARAM_M)
- *
  * @returns the inverse of a
  * @param[in] a Element of GF(2^PARAM_M)
  */
 uint16_t gf_inverse(uint16_t a) {
-	return exp[PARAM_GF_MUL_ORDER - log[a]];
+	int16_t mask = (uint16_t) (-((int32_t) a) >> 31); // a != 0
+	return mask & gf_exp[PARAM_GF_MUL_ORDER - gf_log[a]];
 }
 
 
 
 /**
  * @brief Returns i modulo 2^PARAM_M-1
- *
  * i must be less than 2*(2^PARAM_M-1).
  * Therefore, the return value is either i or i-2^PARAM_M+1.
- *
  * @returns i mod (2^PARAM_M-1)
  * @param[in] i The integer whose modulo is taken
  */
 uint16_t gf_mod(uint16_t i) {
-	uint16_t tmp = i - PARAM_GF_MUL_ORDER;
+	uint16_t tmp = (uint16_t) (i - PARAM_GF_MUL_ORDER);
 
 	// mask = 0xffff if(i < PARAM_GF_MUL_ORDER)
-	int16_t mask = -(tmp >> 15);
+	uint16_t mask = -(tmp >> 15);
 
 	return tmp + (mask & PARAM_GF_MUL_ORDER);
 }

