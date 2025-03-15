#!/bin/bash

# Set compiler variables
export CC=gcc-13
export CXX=g++-13

# Prompt the user
echo "Choose an option:"
echo "1) Full reset (delete build folder, re-run CMake, and use Ninja)"
echo "2) Just re-run CMake with Ninja"
echo "3) Run Ninja to build the project"
read -p "Enter choice (1, 2, or 3): " choice

# Convert input to lowercase for flexibility
choice=$(echo "$choice" | tr '[:upper:]' '[:lower:]')

if [[ "$choice" == "1" ]]; then
    echo "Performing full reset..."
    rm -rf build   # Delete build folder
    mkdir build    # Recreate build folder
    cmake -B build -G Ninja -DCMAKE_C_COMPILER=gcc-13 -DCMAKE_CXX_COMPILER=g++-13
elif [[ "$choice" == "2" ]]; then
    echo "Re-running CMake..."
    cmake -B build -G Ninja -DCMAKE_C_COMPILER=gcc-13 -DCMAKE_CXX_COMPILER=g++-13
elif [[ "$choice" == "3" ]]; then
    echo "Running Ninja build..."
    ninja -C build
else
    echo "Invalid choice. Exiting."
    exit 1
fi

echo "Done!"
