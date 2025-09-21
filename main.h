#pragma once
#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <filesystem>
#include <cstring>
#include <cstdint>
#include "lib/nlohmann/json.hpp"
#include "lib/lodepng/lodepng.h"
#include "lib/blkutils.h"

// #ifdef BLKBUILDER_EXPORTS
// #define  BLKBUILDER_API __declspec(dllexport)
// #else
// #define BLKBUILDER_API __declspec(dllimoport)
// #endif
//
// extern "C" __declspec(dllexport) int main(int argc, char *argv[]);

#include "cmake-build-debug/BlkBuilderDll_export.h"

// class BLKBUILDERDLL_EXPORT BlkBuilderLib {
// public:
//     void Decomp(std::string filePath);
// };
//
// BLKBUILDERDLL_EXPORT void myFunction();

// #ifdef BLKBUILDERDLL_EXPORTS
#define BLKBUILDERDLL_API __declspec(dllexport)
// #else
// #define BLKBUILDERDLL_API __declspec(dllimport)
// #endif

extern "C" BLKBUILDERDLL_API void BB_Decomp(const char* filePath);
extern "C" BLKBUILDERDLL_API void BB_STP(const char* filePath);