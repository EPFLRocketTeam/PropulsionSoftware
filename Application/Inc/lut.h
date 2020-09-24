#ifndef LUT_H
#define LUT_H

#define TEMP_MIN  1200
#define TEMP_MAX  2800

static const uint16_t temp_LUT[] = {
-976, -975, -973, -972, -971, -970, -969, -968, -967, -965, -964, -963, -962, -961, -960, -959,
-957, -956, -955, -954, -953, -952, -951, -949, -948, -947, -946, -945, -944, -943, -941, -940,
-939, -938, -937, -936, -934, -933, -932, -931, -930, -929, -928, -926, -925, -924, -923, -922,
-921, -919, -918, -917, -916, -915, -914, -912, -911, -910, -909, -908, -907, -905, -904, -903,
-902, -901, -900, -898, -897, -896, -895, -894, -893, -891, -890, -889, -888, -887, -885, -884,
-883, -882, -881, -880, -878, -877, -876, -875, -874, -873, -871, -870, -869, -868, -867, -865,
-864, -863, -862, -861, -859, -858, -857, -856, -855, -854, -852, -851, -850, -849, -848, -846,
-845, -844, -843, -842, -840, -839, -838, -837, -836, -834, -833, -832, -831, -830, -828, -827,
-826, -825, -824, -822, -821, -820, -819, -818, -816, -815, -814, -813, -812, -810, -809, -808,
-807, -805, -804, -803, -802, -801, -799, -798, -797, -796, -795, -793, -792, -791, -790, -788,
-787, -786, -785, -784, -782, -781, -780, -779, -777, -776, -775, -774, -773, -771, -770, -769,
-768, -766, -765, -764, -763, -761, -760, -759, -758, -756, -755, -754, -753, -752, -750, -749,
-748, -747, -745, -744, -743, -742, -740, -739, -738, -737, -735, -734, -733, -732, -730, -729,
-728, -727, -725, -724, -723, -722, -720, -719, -718, -717, -715, -714, -713, -712, -710, -709,
-708, -707, -705, -704, -703, -702, -700, -699, -698, -697, -695, -694, -693, -691, -690, -689,
-688, -686, -685, -684, -683, -681, -680, -679, -678, -676, -675, -674, -672, -671, -670, -669,
-667, -666, -665, -663, -662, -661, -660, -658, -657, -656, -655, -653, -652, -651, -649, -648,
-647, -646, -644, -643, -642, -640, -639, -638, -636, -635, -634, -633, -631, -630, -629, -627,
-626, -625, -624, -622, -621, -620, -618, -617, -616, -614, -613, -612, -611, -609, -608, -607,
-605, -604, -603, -601, -600, -599, -597, -596, -595, -594, -592, -591, -590, -588, -587, -586,
-584, -583, -582, -580, -579, -578, -576, -575, -574, -572, -571, -570, -569, -567, -566, -565,
-563, -562, -561, -559, -558, -557, -555, -554, -553, -551, -550, -549, -547, -546, -545, -543,
-542, -541, -539, -538, -537, -535, -534, -533, -531, -530, -529, -527, -526, -524, -523, -522,
-520, -519, -518, -516, -515, -514, -512, -511, -510, -508, -507, -506, -504, -503, -502, -500,
-499, -497, -496, -495, -493, -492, -491, -489, -488, -487, -485, -484, -482, -481, -480, -478,
-477, -476, -474, -473, -472, -470, -469, -467, -466, -465, -463, -462, -461, -459, -458, -456,
-455, -454, -452, -451, -450, -448, -447, -445, -444, -443, -441, -440, -438, -437, -436, -434,
-433, -431, -430, -429, -427, -426, -425, -423, -422, -420, -419, -418, -416, -415, -413, -412,
-411, -409, -408, -406, -405, -404, -402, -401, -399, -398, -397, -395, -394, -392, -391, -389,
-388, -387, -385, -384, -382, -381, -380, -378, -377, -375, -374, -372, -371, -370, -368, -367,
-365, -364, -363, -361, -360, -358, -357, -355, -354, -353, -351, -350, -348, -347, -345, -344,
-343, -341, -340, -338, -337, -335, -334, -332, -331, -330, -328, -327, -325, -324, -322, -321,
-319, -318, -317, -315, -314, -312, -311, -309, -308, -306, -305, -304, -302, -301, -299, -298,
-296, -295, -293, -292, -290, -289, -288, -286, -285, -283, -282, -280, -279, -277, -276, -274,
-273, -271, -270, -268, -267, -265, -264, -263, -261, -260, -258, -257, -255, -254, -252, -251,
-249, -248, -246, -245, -243, -242, -240, -239, -237, -236, -234, -233, -231, -230, -228, -227,
-225, -224, -222, -221, -219, -218, -216, -215, -213, -212, -210, -209, -207, -206, -204, -203,
-201, -200, -198, -197, -195, -194, -192, -191, -189, -188, -186, -185, -183, -182, -180, -179,
-177, -176, -174, -173, -171, -170, -168, -167, -165, -164, -162, -160, -159, -157, -156, -154,
-153, -151, -150, -148, -147, -145, -144, -142, -141, -139, -137, -136, -134, -133, -131, -130,
-128, -127, -125, -124, -122, -121, -119, -117, -116, -114, -113, -111, -110, -108, -107, -105,
-103, -102, -100, -99, -97, -96, -94, -93, -91, -89, -88, -86, -85, -83, -82, -80,
-78, -77, -75, -74, -72, -71, -69, -67, -66, -64, -63, -61, -60, -58, -56, -55,
-53, -52, -50, -48, -47, -45, -44, -42, -40, -39, -37, -36, -34, -33, -31, -29,
-28, -26, -25, -23, -21, -20, -18, -17, -15, -13, -12, -10, -9, -7, -5, -4,
-2, 0, 0, 2, 3, 5, 7, 8, 10, 12, 13, 15, 16, 18, 20, 21,
23, 25, 26, 28, 29, 31, 33, 34, 36, 38, 39, 41, 42, 44, 46, 47,
49, 51, 52, 54, 56, 57, 59, 61, 62, 64, 65, 67, 69, 70, 72, 74,
75, 77, 79, 80, 82, 84, 85, 87, 89, 90, 92, 94, 95, 97, 99, 100,
102, 104, 105, 107, 109, 110, 112, 114, 115, 117, 119, 120, 122, 124, 125, 127,
129, 130, 132, 134, 136, 137, 139, 141, 142, 144, 146, 147, 149, 151, 152, 154,
156, 158, 159, 161, 163, 164, 166, 168, 169, 171, 173, 175, 176, 178, 180, 181,
183, 185, 187, 188, 190, 192, 193, 195, 197, 199, 200, 202, 204, 205, 207, 209,
211, 212, 214, 216, 218, 219, 221, 223, 225, 226, 228, 230, 231, 233, 235, 237,
238, 240, 242, 244, 245, 247, 249, 251, 252, 254, 256, 258, 259, 261, 263, 265,
267, 268, 270, 272, 274, 275, 277, 279, 281, 282, 284, 286, 288, 289, 291, 293,
295, 297, 298, 300, 302, 304, 305, 307, 309, 311, 313, 314, 316, 318, 320, 322,
323, 325, 327, 329, 331, 332, 334, 336, 338, 340, 341, 343, 345, 347, 349, 350,
352, 354, 356, 358, 359, 361, 363, 365, 367, 369, 370, 372, 374, 376, 378, 380,
381, 383, 385, 387, 389, 390, 392, 394, 396, 398, 400, 402, 403, 405, 407, 409,
411, 413, 414, 416, 418, 420, 422, 424, 426, 427, 429, 431, 433, 435, 437, 439,
440, 442, 444, 446, 448, 450, 452, 453, 455, 457, 459, 461, 463, 465, 467, 468,
470, 472, 474, 476, 478, 480, 482, 484, 485, 487, 489, 491, 493, 495, 497, 499,
501, 502, 504, 506, 508, 510, 512, 514, 516, 518, 520, 522, 523, 525, 527, 529,
531, 533, 535, 537, 539, 541, 543, 545, 546, 548, 550, 552, 554, 556, 558, 560,
562, 564, 566, 568, 570, 572, 574, 576, 577, 579, 581, 583, 585, 587, 589, 591,
593, 595, 597, 599, 601, 603, 605, 607, 609, 611, 613, 615, 617, 619, 621, 623,
625, 627, 629, 630, 632, 634, 636, 638, 640, 642, 644, 646, 648, 650, 652, 654,
656, 658, 660, 662, 664, 666, 668, 670, 672, 674, 676, 678, 680, 682, 684, 686,
688, 690, 692, 694, 696, 699, 701, 703, 705, 707, 709, 711, 713, 715, 717, 719,
721, 723, 725, 727, 729, 731, 733, 735, 737, 739, 741, 743, 745, 747, 749, 752,
754, 756, 758, 760, 762, 764, 766, 768, 770, 772, 774, 776, 778, 780, 783, 785,
787, 789, 791, 793, 795, 797, 799, 801, 803, 805, 808, 810, 812, 814, 816, 818,
820, 822, 824, 826, 829, 831, 833, 835, 837, 839, 841, 843, 845, 848, 850, 852,
854, 856, 858, 860, 862, 865, 867, 869, 871, 873, 875, 877, 880, 882, 884, 886,
888, 890, 892, 895, 897, 899, 901, 903, 905, 908, 910, 912, 914, 916, 918, 921,
923, 925, 927, 929, 931, 934, 936, 938, 940, 942, 944, 947, 949, 951, 953, 955,
958, 960, 962, 964, 966, 969, 971, 973, 975, 977, 980, 982, 984, 986, 988, 991,
993, 995, 997, 1000, 1002, 1004, 1006, 1008, 1011, 1013, 1015, 1017, 1020, 1022, 1024, 1026,
1029, 1031, 1033, 1035, 1038, 1040, 1042, 1044, 1047, 1049, 1051, 1053, 1056, 1058, 1060, 1062,
1065, 1067, 1069, 1071, 1074, 1076, 1078, 1081, 1083, 1085, 1087, 1090, 1092, 1094, 1097, 1099,
1101, 1103, 1106, 1108, 1110, 1113, 1115, 1117, 1120, 1122, 1124, 1126, 1129, 1131, 1133, 1136,
1138, 1140, 1143, 1145, 1147, 1150, 1152, 1154, 1157, 1159, 1161, 1164, 1166, 1168, 1171, 1173,
1175, 1178, 1180, 1182, 1185, 1187, 1190, 1192, 1194, 1197, 1199, 1201, 1204, 1206, 1208, 1211,
1213, 1216, 1218, 1220, 1223, 1225, 1227, 1230, 1232, 1235, 1237, 1239, 1242, 1244, 1247, 1249,
1251, 1254, 1256, 1259, 1261, 1263, 1266, 1268, 1271, 1273, 1276, 1278, 1280, 1283, 1285, 1288,
1290, 1292, 1295, 1297, 1300, 1302, 1305, 1307, 1310, 1312, 1314, 1317, 1319, 1322, 1324, 1327,
1329, 1332, 1334, 1337, 1339, 1342, 1344, 1346, 1349, 1351, 1354, 1356, 1359, 1361, 1364, 1366,
1369, 1371, 1374, 1376, 1379, 1381, 1384, 1386, 1389, 1391, 1394, 1396, 1399, 1401, 1404, 1406,
1409, 1411, 1414, 1416, 1419, 1422, 1424, 1427, 1429, 1432, 1434, 1437, 1439, 1442, 1444, 1447,
1450, 1452, 1455, 1457, 1460, 1462, 1465, 1467, 1470, 1473, 1475, 1478, 1480, 1483, 1485, 1488,
1491, 1493, 1496, 1498, 1501, 1504, 1506, 1509, 1511, 1514, 1517, 1519, 1522, 1524, 1527, 1530,
1532, 1535, 1537, 1540, 1543, 1545, 1548, 1551, 1553, 1556, 1559, 1561, 1564, 1566, 1569, 1572,
1574, 1577, 1580, 1582, 1585, 1588, 1590, 1593, 1596, 1598, 1601, 1604, 1606, 1609, 1612, 1614,
1617, 1620, 1622, 1625, 1628, 1631, 1633, 1636, 1639, 1641, 1644, 1647, 1649, 1652, 1655, 1658,
1660, 1663, 1666, 1669, 1671, 1674, 1677, 1679, 1682, 1685, 1688, 1690, 1693, 1696, 1699, 1701,
1704, 1707, 1710, 1712, 1715, 1718, 1721, 1724, 1726, 1729, 1732, 1735, 1737, 1740, 1743, 1746,
1749, 1751, 1754, 1757, 1760, 1763, 1765, 1768, 1771, 1774, 1777, 1780, 1782, 1785, 1788, 1791,
1794, 1797, 1799, 1802, 1805, 1808, 1811, 1814, 1816, 1819, 1822, 1825, 1828, 1831, 1834, 1836,
1839, 1842, 1845, 1848, 1851, 1854, 1857, 1860, 1862, 1865, 1868, 1871, 1874, 1877, 1880, 1883
};

#endif
