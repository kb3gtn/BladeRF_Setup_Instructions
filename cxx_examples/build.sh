#!/bin/bash
# simple stupid build script.
# assumes you have pkg-config setup for bladeRF library.
# Peter Fetterer (kb3gtn@gmail.com)

# make build script exit on error
set -e

echo "Compiling threshold_detect.cpp..."
g++ threshold_detect.cpp -o threshold_detect -std=c++14 `pkg-config libbladeRF --cflags --libs` -fmax-errors=4
echo "Compiling fm_modulator.cpp..."
g++ fm_modulator.cpp -o fm_modulator -std=c++14 `pkg-config libbladeRF --cflags --libs` -fmax-errors=4
echo "Build complete.."
