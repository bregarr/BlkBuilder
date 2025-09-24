
#include "blkutils.h"

uint16_t bufftoi16(const char buff[], const size_t & start, const size_t & end){
    size_t length = end - start;
    uint16_t val;
    unsigned char bytes[length];
    std::copy(buff+start, buff+end, bytes);
    std::memcpy(&val, bytes, length);
    return val;
}
uint32_t bufftoi32(char buff[], const size_t & start, const size_t & end){
    size_t length = end - start;
    if (length > sizeof(uint32_t)) {
        throw std::runtime_error("Length too large for uint32_t");
    }
    uint32_t val;
    std::memcpy(&val, buff + start, length);
    return val;
}
int bufftoi(const char buff[], const size_t & start, const size_t & end){
    size_t length = end - start;
    char val;
    unsigned char bytes[length];
    std::copy(buff+start, buff+end, bytes);
    std::memcpy(&val, bytes, length);
    return val;
}

std::string bufftos(const char buff[], const size_t & start, const size_t & end, bool skipSpace){
    std::string returnString;
    for(int i=start; i<end; ++i){
        if(isspace(buff[i]) || buff[i] == '\u0000') { return returnString; }
        returnString += buff[i];
    }
    return returnString;
}

void stob(const uint16_t &shrt, char* ret){
    for(int i=0; i < 2; ++i){
        ret[1 - i] = (shrt >> (i * 8));
    }
}
void ltob(const uint32_t &lng, char* ret){
    for(int i=0; i < 4; ++i){
        ret[3 - i] = (lng >> (i * 8));
    }
}

void fixFileFormat(std::string & filePath){
    for(char & c : filePath){
        if(c == '/') { c = std::filesystem::path::preferred_separator; }
    }
}

void createParentDirectories(const std::string & filePath){
    std::string workingPath;
    for(const char & c : filePath){
        workingPath += c;
        if(c == std::filesystem::path::preferred_separator){

            if(!std::filesystem::exists(workingPath)){
                std::filesystem::create_directories(workingPath);
            }

        }
    }
}

template <typename T>
void printVector(const std::vector<T>& vec){
    for(T item : vec){
        std::cout << item << ", ";
    }
    std::cout << std::endl;
}

unsigned fileToBuffer(std::string fileName, std::vector<unsigned char> &output, short & width, short & height) {
    std::ifstream file(fileName, std::ios::binary);
    if(!file.is_open()) { std::cerr << "Could not open file.\n"; return -1; }

    file.seekg(16);
    file.read((char *)&width, 4);
    file.read((char *)&height, 4);
    file.close(); // Close the file to avoid overlapping memory

    // Read png data into the buffer (from file name???)
    std::vector<unsigned char> fileBuffer;
    lodepng::load_file(fileBuffer, fileName);

    // Decode PNG data into the dataBuffer
    // !!! MASSIVE RE-WRITE. YOU ARE READING IN MULTIPLE PNGS (1 PNG = 1 FRAME)
    return lodepng::decode(output, w, h, fileBuffer);
}
