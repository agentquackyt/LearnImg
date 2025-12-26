// Header guard to prevent multiple inclusion
#ifndef PGM_H
#define PGM_H
#include <fstream>
#include <cstdint>
struct P5
{
    int width;
    int height;
    unsigned char *img_data;

    P5(int w, int h) : width(w), height(h)
    {
        img_data = new unsigned char[width * height];
    }

    ~P5()
    {
        delete[] img_data;
    }
};

struct P6
{
    int width;
    int height;
    unsigned char *r;
    unsigned char *g;
    unsigned char *b;

    P6(int w, int h) : width(w), height(h)
    {
        r = new unsigned char[width * height];
        g = new unsigned char[width * height];
        b = new unsigned char[width * height];
    }

    ~P6()
    {
        delete[] r;
        delete[] g;
        delete[] b;
    }
};

std::ostream &operator<<(std::ostream &s, const P5 &img)
{
    s << "P5\n";
    s << img.width << ' ' << img.height << "\n";
    s << "255\n";

    const auto byteCount = static_cast<std::streamsize>(img.width) * static_cast<std::streamsize>(img.height);
    s.write(reinterpret_cast<const char *>(img.img_data), byteCount);
    return s;
}

std::ostream &operator<<(std::ostream &s, const P6 &img)
{
    s << "P6\n";
    s << img.width << ' ' << img.height << "\n";
    s << "255\n";

    const auto byteCount = static_cast<std::streamsize>(img.width) * static_cast<std::streamsize>(img.height);
    for (std::streamsize i = 0; i < byteCount; ++i)
    {
        s.put(static_cast<char>(img.r[i]));
        s.put(static_cast<char>(img.g[i]));
        s.put(static_cast<char>(img.b[i]));
    }
    return s;
}
#endif // PGM_H