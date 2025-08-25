#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <stdio.h>
#include <filesystem>

using byte = std::uint8_t;

   //  Debug shit
void printVector(std::vector<std::byte> vec){
   for(std::byte byt : vec){
      std::cout << static_cast<int>(byt);
   }
}

std::byte Byt(int inp) { return static_cast<std::byte>(inp); }

int ReadByte(std::byte bytearr[], int size){ 
   std::ostringstream oss;
   for(int i=0; i<size; ++i) {
      oss << static_cast<int>(bytearr[i]);
   }
   return std::stoi(oss.str());
}

// Code
class Blockfile{

   std::byte GameID[4];// 4 Byte Game ID (?)
   std::byte Length[2];// 2 Byte Length
   // std::byte ??[2];// 2 Byte Unkown
   std::byte BlockSize[2];// 2 Byte String Block Size (?)
   // std::byte ??[2];// 2 Byte Unkown 
   std::byte Magic[4] = {Byt(02), Byt(00), Byt(00), Byt(00)};// 4 Byte Magic [02 00 00 00]
   Blockfile(std::ifstream &BlkFile){

   }

};

std::vector<std::byte> getBytes(const std::string &path){
   std::ifstream file { path, std::ios::binary };
   auto length { std::filesystem::file_size(path) };
   std::vector<std::byte> result (length);
   file.read(reinterpret_cast<char*>(result.data()), static_cast<long>(length));
   return result;
}

// Argument -d (Deconstruct)
//    input.blk, target directory, -d
int main(int argc, char *argv[]){

   if(argc == 4 && static_cast<std::string>(argv[3]) == "-d"){
      std::vector<std::byte> fileBytes = getBytes(argv[1]);
      printVector(fileBytes);
   }

   return 0;
}