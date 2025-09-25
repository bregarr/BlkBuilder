#ifndef BLKBUILDER_BLKUTILS_H
#define BLKBUILDER_BLKUTILS_H

#include <cstdint>
#include <cstring>
#include <filesystem>
#include <iostream>
#include <stdexcept>
#include <vector>
#include "../main.h"

uint16_t bufftoi16(const unsigned char buff[], const size_t & start, const size_t & end);
uint32_t bufftoi32(char unsigned buff[], const size_t & start, const size_t & end);
int bufftoi(const unsigned char buff[], const size_t & start, const size_t & end);
std::string bufftos(const unsigned char buff[], const size_t & start, const size_t & end, bool skipSpace = true);
void stob(const uint16_t &shrt, char* ret);
void ltob(const uint32_t &lng, char* ret);

void stobuff(const unsigned char[], const short& inp);
void ltobuff(const unsigned char[], const short& inp);

void fixFileFormat(std::string & filePath);
void createParentDirectories(const std::string & filePath);

template <typename T>
void printVector(const std::vector<T>& vec);

unsigned fileToBuffer(std::string fileName, std::vector<unsigned char> & output, unsigned & width, unsigned & height, short & hasAlpha);

#endif //BLKBUILDER_BLKUTILS_H