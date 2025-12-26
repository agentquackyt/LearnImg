#include <iostream>
#include <fstream>
#include <cstdint>
#include "pgm.h"

void generateGrayscaleImg(const char *filename = "test.pgm")
{
    P5 img(256, 256);
    for (int y = 0; y < img.height; y++)
    {
        for (int x = 0; x < img.width; x++)
        {
            img.img_data[y * img.width + x] = static_cast<unsigned char>((x + y) / 2);
        }
    }

    std::ofstream file(filename, std::ios::out | std::ios::binary);
    if (!file)
    {
        std::cerr << "Failed to open " << filename << " for writing\n";
        return;
    }

    file << img;
    if (!file)
    {
        std::cerr << "Failed while writing test.pgm\n";
    }
}

void generateColorImg(const char *filename = "test_color.ppm")
{
    P6 img(128, 128);
    for (int y = 0; y < img.height; y++)
    {
        for (int x = 0; x < img.width; x++)
        {
            img.r[y * img.width + x] = static_cast<unsigned char>(x) * 2;
            img.g[y * img.width + x] = static_cast<unsigned char>(y) * 2;
            img.b[y * img.width + x] = static_cast<unsigned char>(x + y) * 2;
        }
    }

    std::ofstream file(filename, std::ios::out | std::ios::binary);
    file << img;
}

void generateGrayscaleCircle(int width, int height, int radius, int stroke, bool filled, const char *filename = "circle.pgm")
{
    P5 img(width, height);
    const int centerX = width / 2;
    const int centerY = height / 2;

    for (int y = 0; y < img.height; y++)
    {
        for (int x = 0; x < img.width; x++)
        {
            const int dx = x - centerX;
            const int dy = y - centerY;
            if (filled && dx * dx + dy * dy <= radius * radius)
            {
                img.img_data[y * img.width + x] = 255; // white inside the circle
            }
            else if (!filled && dx * dx + dy * dy >= (radius - stroke) * (radius - stroke) && dx * dx + dy * dy <= radius * radius)
            {
                img.img_data[y * img.width + x] = 255; // white for the circle outline
            }
            else
            {
                img.img_data[y * img.width + x] = 0; // black background
            }
        }
    }

    std::ofstream file(filename, std::ios::out | std::ios::binary);
    file << img;
}

void generateColorGradient(int width, int height, const char *filename = "gradient.ppm")
{
    P6 img(width, height);
    for (int y = 0; y < img.height; y++)
    {
        for (int x = 0; x < img.width; x++)
        {
            img.r[y * img.width + x] = static_cast<unsigned char>((x * 255) / (img.width - 1));  // Red gradient
            img.g[y * img.width + x] = static_cast<unsigned char>((y * 255) / (img.height - 1)); // Green gradient
            img.b[y * img.width + x] = 128;                                                      // Constant blue value
        }
    }

    std::ofstream file(filename, std::ios::out | std::ios::binary);
    file << img;
}

struct Point
{
    int x;
    int y;

    Point(int x_val, int y_val) : x(x_val), y(y_val) {}

    double distanceTo(const Point &other) const
    {
        int dx = x - other.x;
        int dy = y - other.y;
        return std::sqrt(dx * dx + dy * dy);
    }

    int *interpolateTo(const Point &other, double t) const
    {
        int *result = new int[2];
        result[0] = static_cast<int>(x + t * (other.x - x));
        result[1] = static_cast<int>(y + t * (other.y - y));
        return result;
    }
};

void generatePointCloudImg(int numPoints, int maxX, int maxY)
{
    P5 img(maxX, maxY);
    // Initialize image data to black
    std::fill(img.img_data, img.img_data + (img.width * img.height), 0);

    std::vector<Point> points;
    for (int i = 0; i < numPoints; ++i)
    {
        int x = rand() % maxX;
        int y = rand() % maxY;
        points.emplace_back(x, y);
        img.img_data[y * img.width + x] = 255; // Mark the point in white
    }

    // interpolate between points and mark the path in a color based on distance (min 0 to max 255)
    for (size_t i = 0; i < points.size(); ++i)
    {
        Point &start = points[i];
        Point &end = points[(i + 1) % points.size()]; // wrap
        double dist = start.distanceTo(end);
        int steps = static_cast<int>(dist);
        for (int s = 0; s <= steps; ++s)
        {
            double t = static_cast<double>(s) / steps;
            int *interp = start.interpolateTo(end, t);
            int ix = interp[0];
            int iy = interp[1];
            if (ix >= 0 && ix < img.width && iy >= 0 && iy < img.height)
            {
                img.img_data[iy * img.width + ix] = static_cast<unsigned char>(std::min(255.0, dist)); // Color based on distance
            }
            delete[] interp;
        }
    }

    std::ofstream file("point_cloud.pgm", std::ios::out | std::ios::binary);
    file << img;
}

void generateColorGradientCircle(int width, int height, int radius, const char *filename = "gradient_circle.ppm")
{
    P6 img(width, height);
    const int centerX = width / 2;
    const int centerY = height / 2;

    for (int y = 0; y < img.height; y++)
    {
        for (int x = 0; x < img.width; x++)
        {
            const int dx = x - centerX;
            const int dy = y - centerY;
            if (dx * dx + dy * dy <= radius * radius)
            {
                img.r[y * img.width + x] = static_cast<unsigned char>((x * 255) / (img.width - 1));  // Red gradient
                img.g[y * img.width + x] = static_cast<unsigned char>((y * 255) / (img.height - 1)); // Green gradient
                img.b[y * img.width + x] = 128;                                                      // Constant blue value
            }
            else
            {
                img.r[y * img.width + x] = 0;
                img.g[y * img.width + x] = 0;
                img.b[y * img.width + x] = 0;
            }
        }
    }

    std::ofstream file(filename, std::ios::out | std::ios::binary);
    file << img;
}


int main(int argc, char **argv)
{
    // generateGrayscaleImg((argc > 1) ? argv[1] : "test.pgm");
    // generateColorImg((argc > 2) ? argv[2] : "test_color.ppm");
    // generateGrayscaleCircle(256, 256, 100, 5, false, "circle_2.pgm");
    // generateColorGradient(256, 256, (argc > 1) ? argv[1] : "gradient.ppm");
    // generatePointCloudImg(200, 256, 256);
    generateColorGradientCircle(256, 256, 100, (argc > 1) ? argv[1] : "gradient_circle.ppm");
    return 0;
}