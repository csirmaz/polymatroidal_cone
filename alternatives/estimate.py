
# Try to estimate the number of rays
import math

SET_N = 5

if SET_N == 5:
    AXIOMS=80
    VARS=26
    CASES = 3.634e20
    tries = 15066639538
    finds = 211899616
    distrib = [2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
 2, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 5, 5, 5,
5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 8, 8, 8, 8, 8,
 8, 8, 8, 8, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 10, 10, 10, 10, 10, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 12, 12, 12, 12, 12, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 14, 14, 14, 14, 14, 15, 15, 15, 15, 15, 16, 16, 16, 16, 17, 17, 19, 19, 19, 19, 20, 21, 22, 22, 22, 22, 23, 23, 24, 24, 25, 25, 25, 25, 29, 29, 29, 30, 30, 31, 31, 32, 32, 33, 33, 33, 34, 34, 35, 37, 38, 39, 42, 42, 43, 45, 46, 46, 48, 54, 72, 75, 80, 81, 87, 89, 93, 97, 99, 104, 113, 126, 129, 129,
134, 135, 138, 153, 155, 160, 161, 163, 166, 167, 172, 178, 179, 185, 186, 188, 192, 193, 197, 198, 198, 198, 199, 200, 205, 207, 208, 209, 210, 211, 212, 214, 217,
 217, 217, 217, 217, 218, 218, 219, 219, 220, 220, 220, 221, 222, 224, 225, 225, 226, 226, 226, 226, 227, 228, 230, 230, 231, 231, 232, 232, 232, 232, 233, 233, 233, 234, 234, 234, 237, 238, 238, 239, 240, 243, 247, 247, 248, 250, 250, 250, 251, 253, 253, 253, 256, 257, 258, 258, 268, 271, 272, 274, 275, 278, 280, 282, 472, 489, 510, 513, 513, 514, 518, 521, 531, 533, 538, 539, 547, 548, 553, 555, 556, 557, 559, 559, 563, 567, 567, 568, 569, 570, 570, 570, 577, 580, 581, 581, 582, 583, 584, 590, 590, 591, 592, 592, 599, 599, 600, 606, 607, 608, 609, 609, 610, 611, 612, 614, 616, 618, 626, 628, 629, 631, 632, 637, 643, 644, 655, 657, 662, 672, 675,
679, 697, 705, 2349, 2366, 2390, 2425, 2459, 4085, 4106, 4133, 4151, 4207, 4218, 4241, 4260, 4261, 4263, 4264, 4265, 4281, 4288, 4339, 4368, 4392, 4398, 4398, 4482,
 15651, 15713, 15750, 15781, 15803, 15824, 15847, 15859, 15929, 16059, 16091, 16092, 16149, 16158, 16174, 16176, 16179, 16185, 16198, 16235, 16243, 16255, 16256, 16283, 16297, 16303, 16322, 16330, 16365, 16427, 1696212, 1705284, 1709423, 1709531, 1710253, 1711724, 1712403, 1712423, 1712546, 1713630, 1715766, 1716763, 1717584,
1717864, 1720092, 1720462, 1721939, 1722939, 1723378, 1725268, 1803744, 1808534, 1810370, 1812013, 1813011, 1813212, 1813935, 1814768, 1817892, 1819323, 5379644, 5388881, 14739262, 14749968, 14756071, 14782733, 14802990, 14817098, 14820781, 14836779, 14861627, 14894313]

if SET_N == 6:
    AXIOMS=240
    VARS=57
    CASES = 2.567e55
    tries = 5450878175
    finds = 3349372
    distrib = [1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
               1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
               1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
               1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
               1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2,
               2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 3, 3,
               3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
               4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
               5, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 7, 7, 7, 7, 7, 7, 7, 7, 7, 8, 9, 9, 11, 11, 12, 12, 13, 13, 13, 14, 14, 14, 14, 14, 14, 14, 14, 14,
               14, 15, 15, 15, 16, 16, 17, 17, 17, 17, 17, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 19, 19, 19, 19, 19, 19, 20, 20, 20, 20, 20, 20, 20,
               20, 21, 21, 21, 21, 21, 21, 21, 21, 21, 22, 22, 22, 22, 22, 22, 22, 22, 23, 23, 23, 23, 23, 23, 23, 23, 23, 24, 24, 24, 25, 26, 26, 26,
               26, 26, 27, 27, 28, 29, 29, 33, 37, 7871, 7915, 7943, 7966, 7970, 7982, 7983, 8012, 8013, 8027, 8031, 8035, 8037, 8044, 8052, 8067, 8072, 8079, 8080, 8080, 8084, 8087, 8087, 8096, 8098, 8103, 8105, 8106, 8107, 8111, 8114, 8131, 8133, 8133, 8139, 8147, 8161, 8163, 8166, 8170, 8174, 8176, 8177, 8179, 8188, 8188, 8198, 8198, 8200, 8201, 8203, 8210, 8214, 8215, 8224, 8226, 8229, 8231, 8241, 8242, 8267, 8282, 8288, 8304, 8305, 8311, 8332, 8345, 8356, 8440, 30806, 31078, 31087, 31164, 31169, 31241, 31268, 31273, 31370, 31389, 31440, 31616, 133630, 134015, 134449, 134685, 134781, 134809, 134962, 135198, 135381, 135730, 135841, 136103, 136140, 136402, 136796, 186321, 186490]

LIMIT = 100

uniques = len(distrib)
finds_below = 0
rays_below = 0
finds_above = 0
rays_above = 0
for d in distrib:
    if d < LIMIT:
        finds_below += d
        rays_below += 1
    else:
        finds_above += d
        rays_above += 1

assert finds_below+finds_above == finds
all_cases = CASES/tries*finds
rare_cases = all_cases - finds_above*all_cases/finds
print(f"Found {rays_below} rare unique rays trying {finds_below} times. Cases: {rare_cases} Tries cover: {finds_below/rare_cases*100} %")

# We have thrown N balls into K boxes
# This can be done in (n+k-1  n) ways
def ways(n, k):
    """How many ways to throw N balls into K boxes"""
    return math.comb(n+k-1, n)

def expected_number_of_boxes_filled(n, k):
    """...when throwing N balls into K boxes"""
    if k == 0:
        return 0
    prev = 0
    prev_filled = 0
    step = math.ceil(k / 100)
    for filled in range(1, k, step):
        # Need n balls into filled boxes but such that each box has at least one ball
        if n-filled<0: continue
        cases = math.comb(k, filled) * ways(n-filled, filled)
        # print(f"k={k} n={n} filled={filled} comb={math.comb(k, filled)} ways={ways(n-filled, filled)} log_cases={math.log10(cases)}")
        if prev > cases:
            return filled
        prev = cases
        prev_filled = filled
    return prev_filled

# Now figure out how many boxes (rare rays) we had if after throwing N=finds_below balls we found rays_below ones
# finds_below = 1000
for try_k in range(0, 10000, 100):
    rays = expected_number_of_boxes_filled(n=finds_below, k=try_k)
    print(f"Trying {finds_below} times, if we had {try_k} rare rays ({try_k+rays_above} total), we would have found {rays}")