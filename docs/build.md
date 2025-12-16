# Build Instructions

## Prerequisites

- CMake ¡İ 3.16
- C11 compatible compiler (MSVC, GCC, or Clang)
- (Optional) For code coverage:
  - **Windows**: [OpenCppCoverage](https://github.com/OpenCppCoverage/OpenCppCoverage) in your `PATH`
  - **Unix**: `lcov` package (provides `lcov` and `genhtml`)

## Configure

> ### Default (uses platform-native generator)
> cmake -B out

> ### Or explicitly specify generator:
> #### Visual Studio (Windows)
> cmake -B out -G "Visual Studio 17 2022"
> #### Ninja (cross-platform, faster)
> cmake -B out -G Ninja

## Build
> cmake --build out --config Debug -j 8

## Test
> ctest --test-dir out -C Debug --output-on-failure

## Coverage
> cmake --build out --target coverage

## Install
> cmake --install out

