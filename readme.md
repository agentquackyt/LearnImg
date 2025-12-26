## LearnImg: C++ Image Pattern Generator

LearnImg is a C++ project for generating and saving grayscale and color images in PGM/PPM formats. It provides utilities for creating geometric patterns, gradients, 3D effects, and logical pattern operations, with a focus on educational and experimental image processing.

### Features
- Generate grayscale and color images (PGM/PPM)
- Create geometric patterns: circles, triangles, checkerboards
- Combine patterns using logical operations (AND, OR, NOT, XOR)
- Generate 3D shaded ball images
- Create color gradients and gradient circles
- Generate random point clouds and labyrinth patterns
- Save images to the `patterns/` directory

### File Structure

```
├── app.cpp           # Main app for image/gradient generation
├── pattern.cpp       # Pattern generation and logical operations
├── pgm.h             # PGM/PPM image structures and I/O
├── readme.md         # Project documentation
└── patterns/         # Output images (PGM/PPM)
```

### Building

You need a C++ compiler (e.g., g++ or clang++). To build:

```
g++ app.cpp -o app
g++ pattern.cpp -o pattern
```

### Usage

#### 1. Generate Color Gradient Circle (default in app.cpp):
```
./app [output_filename.ppm]
```
This creates a color gradient circle and saves it as `gradient_circle.ppm` (or the filename you provide).

#### 2. Generate Patterns and 3D Ball (pattern.cpp):
```
./pattern
```
You will be prompted for width and height. The program generates various patterns and saves them in the `patterns/` directory.

#### 3. Terminal GUI (ncurses) for layered generation
A prompt-based terminal GUI (`./gui`) lets you build stacked layers of patterns, apply logical operators between them, preview, and export.

- Launch the interactive GUI:
```
./gui
```
- Key commands (in the GUI):
  - A: Add a layer (circle, triangle, checkerboard, load from `patterns/`, maze)
  - E: Edit layer name
  - D: Delete layer
  - O: Set operator for a layer (AND, OR, XOR) — applied to this layer relative to the accumulated result
  - N: Toggle NOT for a layer
  - U/J: Move layer up/down in the stack
  - P: Preview combined result (saves `./patterns/gui_preview.pgm`)
  - X: Export: P5 (PGM) or P6 (PPM) (choose layers for R/G/B channels when exporting P6)
  - W: Change default width/height for future layers

#### 4. Example Output Files
- `patterns/3d_ball.pgm`: 3D shaded ball
- `patterns/gui_preview.pgm`: Preview from the GUI
- `patterns/pattern_mixer.ppm`: Color pattern mixer
- `patterns/<your_export>.pgm/.ppm`: Files you export from the GUI

### Customization
- Modify `app.cpp` or `pattern.cpp` to experiment with different pattern parameters or add new image generation functions.

### Requirements
- Standard C++17 or later
- ncurses for GUI (`-lncurses`)

### License
MIT License

### Customization
- Modify `app.cpp` or `pattern.cpp` to experiment with different pattern parameters or add new image generation functions.

### Requirements
- Standard C++17 or later
- No external dependencies

### License
MIT License
