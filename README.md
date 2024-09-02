# Pong

This project is a basic SDL2 application that displays an image on the screen. You can move the image around using the arrow keys or `W`, `A`, `S`, `D`.

## Requirements

Before you can compile and run the program, make sure you have the following dependencies installed on your system:

- **g++**: A C++ compiler.
- **SDL2**: The Simple DirectMedia Layer 2 library.
- **SDL2_image**: An SDL2 extension for loading images.

### Installing Dependencies on Ubuntu/Debian

You can install the required libraries using `apt`:

```bash
sudo apt-get update
sudo apt-get install g++ libsdl2-dev libsdl2-image-dev
```

## Compilation and execution
```bash
g++ main.cpp -o main -lSDL2 -lSDL2_image && ./main
```

## License
This `README.md` includes all the necessary steps for setting up, compiling, and running your SDL2 project, along with some troubleshooting tips and customization instructions.
