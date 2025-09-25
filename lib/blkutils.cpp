
#include "blkutils.h"

uint16_t bufftoi16(const unsigned char buff[], const size_t & start, const size_t & end){
    size_t length = end - start;
    uint16_t val;
    unsigned char bytes[length];
    std::copy(buff+start, buff+end, bytes);
    std::memcpy(&val, bytes, length);
    return val;
}
uint32_t bufftoi32(char unsigned buff[], const size_t & start, const size_t & end){
    size_t length = end - start;
    if (length > sizeof(uint32_t)) {
        throw std::runtime_error("Length too large for uint32_t");
    }
    uint32_t val;
    std::memcpy(&val, buff + start, length);
    return val;
}
int bufftoi(const unsigned char buff[], const size_t & start, const size_t & end){
    size_t length = end - start;
    char val;
    unsigned char bytes[length];
    std::copy(buff+start, buff+end, bytes);
    std::memcpy(&val, bytes, length);
    return val;
}

std::string bufftos(const unsigned char buff[], const size_t & start, const size_t & end, bool skipSpace){
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

// into buffers

// void stobuff(char* buff, const short& inp) {
//     size_t length = 2;
//     uint16_t val;
//     unsigned char bytes[length];
//     std::copy(buff+start, buff+end, bytes);
//     std::memcpy(&val, bytes, length);
//     return val;
// }
// void ltobuff(const char[], const short& inp);

// Utils
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

unsigned fileToBuffer(std::string fileName, std::vector<unsigned char> &output, unsigned & width, unsigned & height, short & hasAlpha) {
    // Read png data into the buffer (from file name???)
    std::vector<unsigned char> fileBuffer;
    lodepng::load_file(fileBuffer, fileName);
    lodepng::State state;

    // Decode PNG data into the dataBuffer
    // !!! MASSIVE RE-WRITE. YOU ARE READING IN MULTIPLE PNGS (1 PNG = 1 FRAME)
    unsigned error =  lodepng::decode(output, width, height, state, fileBuffer);

    const LodePNGColorMode& color = state.info_png.color;
    switch (color.colortype) {
        case LCT_RGBA:
        case LCT_GREY_ALPHA:
            hasAlpha = true;
            break;
        case LCT_RGB:
        case LCT_GREY:
            hasAlpha = color.key_defined;
            break;
        default:
            hasAlpha = false;
            break;
    }

    return error;
}
