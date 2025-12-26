#include "pattern.h"

int main() {
    int w = 128, h = 128;
    Pattern c = generateCirclePattern(w,h,30);
    savePatternAsPgm(c, "test_circle.pgm");
    Pattern t = generateTrianglePattern(w,h);
    savePatternAsPgm(t, "test_triangle.pgm");
    Pattern cb = generateCheckerboardPattern(w,h,8);
    savePatternAsPgm(cb, "test_checker.pgm");
    patternMixer(c, t, cb, 180, "test_mixer.ppm");
    return 0;
}
