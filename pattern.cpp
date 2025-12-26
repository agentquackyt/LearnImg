#include <iostream>
#include <fstream>
#include <cstdint>
#include <cmath>
#include "pgm.h"
#include "pattern.h"

#include <cstring> // for std::memset

// Implementations for Pattern declared in pattern.h
Pattern::Pattern(int w, int h) : width(w), height(h)
{
    data = new bool[width * height];
    std::memset(data, 0, width * height * sizeof(bool));
}

Pattern::Pattern(const Pattern& other) : width(other.width), height(other.height)
{
    data = new bool[width * height];
    for (int i = 0; i < width * height; ++i) data[i] = other.data[i];
}

Pattern& Pattern::operator=(const Pattern& other)
{
    if (this != &other) {
        delete[] data;
        width = other.width;
        height = other.height;
        data = new bool[width * height];
        for (int i = 0; i < width * height; ++i) data[i] = other.data[i];
    }
    return *this;
}

Pattern::~Pattern() {
    delete[] data;
}

Pattern Pattern::operator&&(const Pattern &other) const {
    Pattern result(width, height);
    for (int i = 0; i < width * height; ++i) result.data[i] = data[i] && other.data[i];
    return result;
}

Pattern Pattern::operator||(const Pattern &other) const {
    Pattern result(width, height);
    for (int i = 0; i < width * height; ++i) result.data[i] = data[i] || other.data[i];
    return result;
}

Pattern Pattern::operator!() const {
    Pattern result(width, height);
    for (int i = 0; i < width * height; ++i) result.data[i] = !data[i];
    return result;
}

Pattern Pattern::operator^(const Pattern &other) const {
    Pattern result(width, height);
    for (int i = 0; i < width * height; ++i) result.data[i] = data[i] ^ other.data[i];
    return result;
}

void Pattern::saveAsPgm(const char *filename) const {
    P5 img(width, height);
    for (int i = 0; i < width * height; ++i) img.img_data[i] = data[i] ? 255 : 0;
    std::string out_path = std::string("./patterns/") + filename;
    std::ofstream file(out_path.c_str(), std::ios::out | std::ios::binary);
    if (!file) { std::cerr << "Failed to open " << out_path << " for writing\n"; return; }
    file << img;
    if (!file) std::cerr << "Failed while writing " << filename << "\n";
}

// Free function wrapper to save a Pattern (declared in pattern.h)
void savePatternAsPgm(const Pattern &p, const char *filename)
{
    P5 img(p.width, p.height);
    for (int i = 0; i < p.width * p.height; ++i)
    {
        img.img_data[i] = p.data[i] ? 255 : 0;
    }

    std::string out_path = std::string("./patterns/") + filename;
    std::ofstream file(out_path.c_str(), std::ios::out | std::ios::binary);
    if (!file)
    {
        std::cerr << "Failed to open " << out_path << " for writing\n";
        return;
    }

    file << img;
    if (!file)
    {
        std::cerr << "Failed while writing " << filename << "\n";
    }
}

Pattern generateCirclePattern(int width, int height, int radius)
{
    Pattern pattern(width, height);
    const int centerX = width / 2;
    const int centerY = height / 2;
    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            const int dx = x - centerX;
            const int dy = y - centerY;
            pattern.data[y * width + x] = (dx * dx + dy * dy <= radius * radius);
        }
    }
    return pattern;
}

Pattern generateTrianglePattern(int width, int height)
{
    Pattern pattern(width, height);
    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            pattern.data[y * width + x] = (x >= (width / 2 - (y * width) / (2 * height)) && x <= (width / 2 + (y * width) / (2 * height)));
        }
    }
    return pattern;
}

Pattern generateCheckerboardPattern(int width, int height, int squareSize)
{
    Pattern pattern(width, height);
    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            int xSquare = x / squareSize;
            int ySquare = y / squareSize;
            pattern.data[y * width + x] = ((xSquare + ySquare) % 2 == 0);
        }
    }
    return pattern;
}

// Generates a grayscale PGM image of a shaded 3D ball (sphere)
void generate3DBallPgm(int width, int height, const char* filename = "3d_ball.pgm")
{
    P5 img(width, height);
    int cx = width / 2;
    int cy = height / 2;
    int radius = std::min(width, height) / 2 - 4;
    // Light direction (normalized)
    double lx = -0.9, ly = -0.9, lz = 1.0;
    double len = std::sqrt(lx*lx + ly*ly + lz*lz);
    lx /= len; ly /= len; lz /= len;
    for (int y = 0; y < height; ++y)
    {
        for (int x = 0; x < width; ++x)
        {
            int dx = x - cx;
            int dy = y - cy;
            double dist2 = dx*dx + dy*dy;
            if (dist2 <= radius*radius)
            {
                // Sphere surface normal
                double dz = std::sqrt(radius*radius - dist2);
                double nx = dx / (double)radius;
                double ny = dy / (double)radius;
                double nz = dz / (double)radius;
                // Lambertian shading
                double dot = nx*lx + ny*ly + nz*lz;
                double intensity = std::max(0.0, dot);
                // Add a subtle specular highlight
                double rx = 2*dot*nx - lx;
                double ry = 2*dot*ny - ly;
                double rz = 2*dot*nz - lz;
                double viewz = 1.0;
                double spec = std::pow(std::max(0.0, rz), 20.0);
                int val = (int)(40 + 180 * intensity + 35 * spec);
                img.img_data[y * width + x] = std::min(255, std::max(0, val));
            }
            else
            {
                img.img_data[y * width + x] = 0;
            }
        }
    }
    std::string out_path = std::string("./patterns/") + filename;
    std::ofstream file(out_path.c_str(), std::ios::out | std::ios::binary);
    if (!file)
    {
        std::cerr << "Failed to open " << out_path << " for writing\n";
        return;
    }
    file << img;
    if (!file)
    {
        std::cerr << "Failed while writing " << filename << "\n";
    }
}

void patternMixer(Pattern r, Pattern g, Pattern b, int base_value, const char *filename = "pattern_mixer.ppm")
{
    P6 img(r.width, r.height);
    for (int y = 0; y < img.height; y++)
    {
        for (int x = 0; x < img.width; x++)
        {
            img.r[y * img.width + x] = r.data[y * img.width + x] ? base_value : 0;
            img.g[y * img.width + x] = g.data[y * img.width + x] ? base_value : 0;
            img.b[y * img.width + x] = b.data[y * img.width + x] ? base_value : 0;
        }
    }

    std::string out_path = std::string("./patterns/") + filename;
    std::ofstream file(out_path.c_str(), std::ios::out | std::ios::binary);
    if (!file) {
        std::cerr << "Failed to open " << out_path << " for writing\n";
        return;
    }
    file << img;
}

// Load a P5 PGM from the patterns folder and convert to a boolean Pattern (non-zero -> true)
Pattern loadPatternFromPgm(const char *filename)
{
    std::string in_path = std::string("./patterns/") + filename;
    std::ifstream file(in_path.c_str(), std::ios::in | std::ios::binary);
    if (!file) {
        std::cerr << "Failed to open " << in_path << " for reading\n";
        return Pattern(1,1);
    }

    std::string magic;
    file >> magic;
    if (magic != "P5") {
        std::cerr << "Unsupported magic: " << magic << " (expected P5)\n";
        return Pattern(1,1);
    }

    // Skip comments and read width/height and maxval
    int width=0, height=0, maxval=0;
    while (file.peek() == '\n' || file.peek() == ' ' || file.peek() == '\r' || file.peek() == '\t') file.get();
    while (file.peek() == '#') {
        std::string line;
        std::getline(file, line);
    }
    file >> width >> height;
    file >> maxval;
    file.get(); // skip single whitespace

    if (width <= 0 || height <= 0) {
        std::cerr << "Invalid dimensions in " << in_path << "\n";
        return Pattern(1,1);
    }

    Pattern p(width, height);
    std::vector<unsigned char> buffer(width * height);
    file.read(reinterpret_cast<char*>(buffer.data()), width * height);
    if (!file) {
        std::cerr << "Failed while reading image data from " << in_path << "\n";
        return Pattern(1,1);
    }
    for (int i = 0; i < width * height; ++i) {
        p.data[i] = buffer[i] != 0;
    }
    return p;
}

Pattern labyrinthPatternGenerator(Pattern &basePattern, Pattern &visited, int x, int y)
{
    visited.data[y * basePattern.width + x] = true;
    // Directions: up, right, down, left
    int directions[4][2] = { {0, -1}, {1, 0}, {0, 1}, {-1, 0} };
    // Shuffle directions for randomness
    for (int i = 3; i > 0; --i) {
        int j = rand() % (i + 1);
        std::swap(directions[i], directions[j]);
    }
    for (int d = 0; d < 4; ++d)
    {
        int nx = x + directions[d][0] * 2;
        int ny = y + directions[d][1] * 2;
        if (nx >= 0 && nx < basePattern.width && ny >= 0 && ny < basePattern.height && !visited.data[ny * basePattern.width + nx])
        {
            // Remove wall between current and next cell
            basePattern.data[(y + directions[d][1]) * basePattern.width + (x + directions[d][0])] = true;
            labyrinthPatternGenerator(basePattern, visited, nx, ny);
        }
    }
    return basePattern;
}

// main() removed to avoid duplicate symbol error