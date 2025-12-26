#ifndef PATTERN_H
#define PATTERN_H

struct Pattern {
    int width;
    int height;
    bool *data;
    Pattern(int w, int h);
    Pattern(const Pattern& other);
    Pattern& operator=(const Pattern& other);
    ~Pattern();
    Pattern operator&&(const Pattern &other) const;
    Pattern operator||(const Pattern &other) const;
    Pattern operator!() const;
    Pattern operator^(const Pattern &other) const;
    void saveAsPgm(const char *filename) const;
};

Pattern generateCirclePattern(int width, int height, int radius);
Pattern generateTrianglePattern(int width, int height);
Pattern generateCheckerboardPattern(int width, int height, int squareSize);
void generate3DBallPgm(int width, int height, const char* filename);
void patternMixer(Pattern r, Pattern g, Pattern b, int base_value, const char *filename);

// Load a binary P5 PGM file from ./patterns/ and convert to a Pattern (non-zero pixels -> true)
Pattern loadPatternFromPgm(const char *filename);
Pattern labyrinthPatternGenerator(Pattern &basePattern, Pattern &visited, int x = 0, int y = 0);

// Helper to save Pattern to file (implemented in pattern.cpp)
void savePatternAsPgm(const Pattern &p, const char *filename);

#endif // PATTERN_H
