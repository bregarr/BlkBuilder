#ifndef BLKBUILDER_BLKUTILS_H
#define BLKBUILDER_BLKUTILS_H

#include <cstdint>
#include <cstring>
#include <filesystem>
#include <iostream>
#include <stdexcept>
#include <vector>
#include "../main.h"

uint16_t bufftoi16(const char buff[], const size_t & start, const size_t & end);
uint32_t bufftoi32(char buff[], const size_t & start, const size_t & end);
int bufftoi(const char buff[], const size_t & start, const size_t & end);
std::string bufftos(const char buff[], const size_t & start, const size_t & end, bool skipSpace = true);
void stob(const uint16_t &shrt, char* ret);
void ltob(const uint32_t &lng, char* ret);

void fixFileFormat(std::string & filePath);
void createParentDirectories(const std::string & filePath);

template <typename T>
void printVector(const std::vector<T>& vec);

unsigned fileToBuffer(std::string fileName, std::vector<unsigned char> & output, short & width, short & height);

#endif //BLKBUILDER_BLKUTILS_H