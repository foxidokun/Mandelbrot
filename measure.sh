#!/usr/bin/env bash

sudo cpupower frequency-set -g performance

g++ -Ofast -march=native -flto -mtune=native dumb_implementation.cpp graphics.cpp main.cpp -lm -lsfml-window -lsfml-graphics -lsfml-system -o mandelbrot -lstdc++ -D DUMB_VERSION

sudo nice -n -20 ./mandelbrot

g++ -Ofast -march=native -flto -mtune=native avx_implementation.cpp graphics.cpp main.cpp -lm -lsfml-window -lsfml-graphics -lsfml-system -o mandelbrot -lstdc++ -D AVX_VERSION

sudo nice -n -20 ./mandelbrot

clang -Ofast -march=native -flto -mtune=native dumb_implementation.cpp graphics.cpp main.cpp -lm -lsfml-window -lsfml-graphics -lsfml-system -o mandelbrot -lstdc++ -D DUMB_VERSION

sudo nice -n -20 ./mandelbrot

clang -Ofast -march=native -flto -mtune=native avx_implementation.cpp graphics.cpp main.cpp -lm -lsfml-window -lsfml-graphics -lsfml-system -o mandelbrot -lstdc++ -D AVX_VERSION

sudo nice -n -20 ./mandelbrot

