#include "main.h"

using json = nlohmann::json_abi_v3_12_0::json;

void ManageDecomp(std::ifstream & file, std::string filenamelong){
   // Header Manager

   int GameIDMaj;
   int GameIDMin;
   int Size;
   int Padding;
   int PathLength;
   int Unused;
   int FileTableOffset;
   int DataOffset;
   int Unused2;

   {
   char HeaderBuffer[24];
   file.read(HeaderBuffer, 24);
   GameIDMaj = bufftoi16(HeaderBuffer, 0, 2);
   GameIDMin = bufftoi16(HeaderBuffer, 2, 4);
   Size = bufftoi16(HeaderBuffer, 4, 6);
   Padding = bufftoi16(HeaderBuffer, 6, 8);
   PathLength = bufftoi16(HeaderBuffer, 8, 10);
   Unused = bufftoi16(HeaderBuffer, 10, 12);
   FileTableOffset = bufftoi32(HeaderBuffer, 12, 16);
   DataOffset = bufftoi32(HeaderBuffer, 16, 20);
   Unused2 = bufftoi32(HeaderBuffer, 20, 24);
   }

   std::string filename = filenamelong.substr(0,filenamelong.size()-4);
   std::filesystem::create_directory(filename);
   // std::ofstream jsonFile(filename+std::filesystem::path::preferred_separator+filename+".json");

   file.seekg(FileTableOffset, std::ios::beg);

   for(int i=0; i<Size; ++i){
      // Debug
      file.seekg(FileTableOffset + 156*i, std::ios::beg);

      // Fill Json File
      char FileNameBuffer[128];
      file.read(FileNameBuffer, 128);
      std::string filePath = bufftos(FileNameBuffer, 0, 128);
      fixFileFormat(filePath);
      // std::cout << filePath << std::endl;

      char SegmentBuffer[28];
      file.read(SegmentBuffer, 28);
      int Offset = bufftoi32(SegmentBuffer, 0, 4);
      int Length = bufftoi32(SegmentBuffer, 4, 8);
      int PadLength = bufftoi(SegmentBuffer, 8, 9);
      int Unused = bufftoi(SegmentBuffer, 9, 10);
      int Unused2 = bufftoi16(SegmentBuffer, 10, 12);
      std::string MD5 = bufftos(SegmentBuffer, 12, 28, false);

      // Create & Populate Asset Files
      // std::cout << Offset << std::endl;
      if (!file.is_open()) {
         std::cerr << "ERROR: File not open\n";
         std::exit(1);
      }
      if (!file.good()) {
         std::cerr << "ERROR: File stream not good before seek\n";
         std::exit(1);
      }
      file.clear();
      file.seekg(Offset, std::ios::beg);
      // std::cout << filename+std::filesystem::path::preferred_separator+filePath << std::endl;
      // std::cout << file.tellg() << std::endl;
      createParentDirectories(filename+static_cast<char>(std::filesystem::path::preferred_separator)+filePath);
      std::ofstream assetFile(filename+static_cast<char>(std::filesystem::path::preferred_separator)+filePath, std::ios::binary);
      std::vector<unsigned char> assetBuffer(Length); // VLA is bad dont write this
      file.read(reinterpret_cast<char*>(assetBuffer.data()), Length);
      assetFile.write(reinterpret_cast<const char*>(assetBuffer.data()), assetBuffer.size());

      assetFile.close();
   }
   // jsonFile << std::setw(4) << j.dump(-1, ' ', false, json::error_handler_t::replace) << std::endl;

   // jsonFile.close();
}

void STP(std::ifstream & file, std::string filenamelong){
   // Header Manager
   char code[2];
   short version;
   short numFrames;
   short unused;
   unsigned long flags;
   float framesPerSecond;
   long dataStartOffset;
   
   // Current theory: Header holds frames, frames hold segments
   // Nest the FUCK out of for loops :)
   // Pixel data found in segment header's offset?
   {
      char HeaderBuffer[20];
      file.read(HeaderBuffer, 20);
      code[0] = HeaderBuffer[0]; code[1] = HeaderBuffer[1];
      version = bufftoi16(HeaderBuffer, 2, 4);
      numFrames = bufftoi16(HeaderBuffer, 4, 6);
      unused = bufftoi16(HeaderBuffer, 6, 8);
      flags = bufftoi32(HeaderBuffer, 8, 12);
      framesPerSecond = bufftoi32(HeaderBuffer, 12, 16);
      dataStartOffset = bufftoi32(HeaderBuffer, 16, 20);
   }

   std::string filename = filenamelong.substr(0,filenamelong.size()-4);

   int totalSegs = 0;
   // Frame Iterator
   size_t frameSize = 16;
   size_t segSize = 20;

   std::vector<std::size_t> segGroupVec;
   const std::size_t segDataLen = 5;
   std::vector<long> segData; // Pairs of 5, [Length, Offset, Width, Height, Group]
   unsigned totalWidth;
   unsigned totalHeight;

   for(int i=0; i<numFrames; ++i){
      char FrameHeaderBuffer[16];
      file.read(FrameHeaderBuffer, 16);

      short mode = bufftoi16(FrameHeaderBuffer, 0, 2);
      short hasAlpha = bufftoi16(FrameHeaderBuffer, 2, 4);
      short width = bufftoi16(FrameHeaderBuffer, 4, 6); totalWidth = 2*width;
      short height = bufftoi16(FrameHeaderBuffer, 6, 8); totalHeight = 2*height;
      short hotSpotX = bufftoi16(FrameHeaderBuffer, 8, 10);
      short hotSpotY = bufftoi16(FrameHeaderBuffer, 10, 12);
      short unused2 = bufftoi16(FrameHeaderBuffer, 12, 14); // Noted to be padding?
      short numSegments = bufftoi16(FrameHeaderBuffer, 14, 16);
      segGroupVec.push_back(numSegments);
      totalSegs += numSegments;

      std::cout << "BLKBLDR:: Previous Frame with mode: " << mode << std::endl;


      std::size_t segGroup = segGroupVec.size()-1;
      int groupCount = 0;

      for(int j=0; j<numSegments; ++j){
         char SegHeaderBuffer[20];
         file.read(SegHeaderBuffer, 20);

         short segFlags = bufftoi16(SegHeaderBuffer, 0, 2);
         short numMipMaps = bufftoi16(SegHeaderBuffer, 2, 4);
         short segWidth = bufftoi16(SegHeaderBuffer, 4, 6);
         short segHeight = bufftoi16(SegHeaderBuffer, 6, 8);
         short xOffset = bufftoi16(SegHeaderBuffer, 8, 10);
         short yOffset = bufftoi16(SegHeaderBuffer, 10, 12);
         long segLength = bufftoi32(SegHeaderBuffer, 12, 16);
         long segOffset = bufftoi32(SegHeaderBuffer, 16, 20);
         segData.push_back(segLength);
         segData.push_back(segOffset);
         segData.push_back(segWidth);
         segData.push_back(segHeight);
         if(groupCount >= segGroupVec.at(segGroup)) {
            groupCount = 0;
            segGroup++;
         }
         segData.push_back(segGroup);
         groupCount++;
      }

   }

   std::vector<std::vector<uint8_t>> rawImageBuffer;
   std::size_t prevIndex = -1;
   std::vector<uint8_t> temp;
   for (int i=0; i<totalSegs; ++i){
      std::size_t bufferIndex = segData[i*segDataLen+4];

      file.seekg(segData[1+segDataLen*i] + dataStartOffset, std::ios::beg);
      std::size_t oldSize = temp.size();
      temp.resize(oldSize + segData[i*segDataLen]);
      file.read(reinterpret_cast<char*>(temp.data() + oldSize), segData[i*segDataLen]);

      if(bufferIndex != prevIndex) {
         rawImageBuffer.push_back(temp);
         temp.clear();
      }
      prevIndex = bufferIndex;
   }
   if(!temp.empty()){
      rawImageBuffer.push_back(temp);
   }

   for (int i=0; i<numFrames; ++i){
      std::string combinedFileName = filename+std::to_string(i)+".png";
      // createParentDirectories(filename+std::filesystem::path::preferred_separator+filePath);

      LodePNGColorType cType = LCT_RGBA;
      unsigned bitdepth = 8;
      const std::size_t expected_rgba = segData[2 + segDataLen*i] * segData[3 + segDataLen*i] * 4;
      const std::size_t expected_rgb  = segData[2 + segDataLen*i] * segData[3 + segDataLen*i] * 3;
      const std::size_t expected_gray = segData[2 + segDataLen*i] * segData[3 + segDataLen*i];
      std::vector<uint8_t> frameBuffer;
      if (expected_rgba == rawImageBuffer.at(i).size()){ // rgba
         frameBuffer = rawImageBuffer.at(i);
      }
      else if (expected_rgb == rawImageBuffer.at(i).size()){ // rgb
         std::cout << "BLKBLDR:: Not enough bytes found. Removing alpha channel." << std::endl;
         std::size_t rgbSize = rawImageBuffer.at(i).size() / 3 * 4;
         frameBuffer.reserve(rgbSize);
         for(int j=0; j<rawImageBuffer.at(i).size(); j+=3){
            frameBuffer.push_back(rawImageBuffer.at(i).at(j));
            frameBuffer.push_back(rawImageBuffer.at(i).at(j+1));
            frameBuffer.push_back(rawImageBuffer.at(i).at(j+2));
            frameBuffer.push_back(255);
         }
      }
      else if (expected_gray == rawImageBuffer.at(i).size()){ //grey-scale
         std::cout << "BLKBLDR:: Not enough bytes found. Outputting in greyscale." << std::endl;
         frameBuffer = rawImageBuffer.at(i);
         cType = LCT_GREY;
         bitdepth = 8;
      }

      unsigned error = lodepng::encode(combinedFileName, frameBuffer, segData[2 + segDataLen*i], segData[3 + segDataLen*i], cType); // , LCT_GREY, 8
      if(error){
         std::cerr << "PNG Encode Error " << error << ": " << lodepng_error_text(error) << "\n";
      }
   }

   std::cout << "BLKBLDR:: Decompiled file with FPS: " << framesPerSecond << ", flags: " << flags << std::endl;

}

void PTS(const std::string &fileName, const float &fps, const unsigned long &flagsPass, std::vector<std::string> &fileVec){
   // Open the file to read w*h values
   std::ifstream file(fileName, std::ios::binary);
   if(!file.is_open()) { std::cerr << "Could not open file.\n"; return; }

   file.seekg(16);
   unsigned w; unsigned h;
   file.read((char *)&w, 4);
   file.read((char *)&h, 4);
   file.close(); // Close the file to avoid overlapping memory

   // Read png data into the buffer (from file name???)
   std::vector<unsigned char> fileBuffer;
   lodepng::load_file(fileBuffer, fileName);

   // Decode PNG data into the dataBuffer
            // !!! MASSIVE RE-WRITE. YOU ARE READING IN MULTIPLE PNGS (1 PNG = 1 FRAME)
   std::vector<unsigned char> dataBuffer;
   unsigned error = lodepng::decode(dataBuffer, w, h, fileBuffer);

   // Header Manager -- 20 bytes
   char code[2]; code[0] = 'S'; code[1] = 'p';
   char verbuff[2]; verbuff[0] = '3'; verbuff[1] = '0';
   short version = bufftoi16(verbuff, 0, 2); // Seems like [3], [0]
   short numFrames = fileVec.size();
   short unused; // Blank?
   unsigned long flags = flagsPass;
   float framesPerSecond = fps;
   // Header+(numFrames*FrameManager+SegManager)
   long dataStartOffset = 20 + numFrames * (16 + 20);

   std::vector<std::size_t> segGroupVec;
   size_t totalSegs = 0;
   // Frame Header Manager -- 16 bytes * numFrames
   for (int i=0;i<numFrames;++i) {
      short mode; // Set a default one for now
      short hasAlpha; // Try not to take user input, that feels like a lot of work
      short width; // Read these in from the png shit
      short height;
      short hotSpotX;
      short hotSpotY; // Does stay even know these?
      short unused2; // Noted to be padding?
      short numSegments = 1; // I can't be assed to write a segment thing
      segGroupVec.push_back(numSegments);
      totalSegs += numSegments;

      std::size_t segGroup = segGroupVec.size()-1;
      int groupCount = 0;

      // Seg Header Manager -- 20 * numFrames ( * numSegments but Im not doing multiple segments)
      // Probably just put 1 segment per frame
      for (int c=0;c<numSegments;++c) {

      }
   }



   // file.close();
}


// Argument -d (Deconstruct)
//    input.blk, -d
// Argument -stp (.spa to .png)
//    input.spa, -stp
// Argument -pts (.png to .spa)
//    output.spa, fps, flags, [input.png, input1.png, input2.png, ...], -pts
int main(int argc, char *argv[]){

   // Test GUI Code

   // End

   if(argc == 3 && static_cast<std::string>(argv[2]) == "-d"){
      std::ifstream file(static_cast<std::string>(argv[1]), std::ios::binary);
      if(!file.is_open()) { std::cerr << "Could not open file.\n"; return 1; }
      
      ManageDecomp(file, static_cast<std::string>(argv[1]));

      file.close();
   }
   else if(argc == 3 && static_cast<std::string>(argv[2]) == "-stp"){
      std::ifstream file(static_cast<std::string>(argv[1]), std::ios::binary);
      if(!file.is_open()) { std::cerr << "Could not open file.\n"; return 1; }
      
      STP(file, static_cast<std::string>(argv[1]));

      file.close();
   }
   else if(argc >= 6 && static_cast<std::string>(argv[argc-1]) == "-pts"){
      std::vector<std::string> fileVec;
      for(std::size_t i = 4; i < argc-1; ++i){
         fileVec.push_back(static_cast<std::string>(argv[i]));
      }
      char* endPtr;
      PTS(static_cast<std::string>(argv[1]), std::stof(std::string(argv[2])), std::strtoul(argv[3], &endPtr, 10), fileVec);
   }

   return 0;
}

extern "C" BLKBUILDERDLL_API void BB_Decomp(const char* filePath) {
   std::string cppFilePath (filePath);
   std::ifstream file(cppFilePath, std::ios::out | std::ios::binary);
   if(!file.is_open()) { std::cerr << "Could not open file.\n"; return; }

   ManageDecomp(file, cppFilePath);

   file.close();
}

extern "C" BLKBUILDERDLL_API void BB_STP(const char* filePath) {
   std::string cppFilePath(filePath);
   std::ifstream file(cppFilePath, std::ios::out | std::ios::binary);
   if(!file.is_open()) { std::cerr << "Could not open file.\n"; return; }

   STP(file, cppFilePath);

   file.close();
}


// Json shit
   // for(int i=0; i<Size; ++i){
   //    // Debug
   //    file.seekg(FileTableOffset + 156*i, std::ios::beg);

   //    // Fill Json File
   //    char FileNameBuffer[128];
   //    file.read(FileNameBuffer, 128);
   //    std::string filePath = bufftos(FileNameBuffer, 0, 128);
   //    j[i]["FilePath"] = filePath;

   //    char SegmentBuffer[28];
   //    file.read(SegmentBuffer, 28);
   //    j[i]["Offset"] = bufftoi32(SegmentBuffer, 0, 4);
   //    j[i]["Length"] = bufftoi32(SegmentBuffer, 4, 8);
   //    j[i]["PadLength"] = bufftoi(SegmentBuffer, 8, 9);
   //    j[i]["Unused"] = bufftoi(SegmentBuffer, 9, 10);
   //    j[i]["Unused2"] = bufftoi16(SegmentBuffer, 10, 12);
   //    j[i]["MD5"] = bufftos(SegmentBuffer, 12, 28, false);

   //    // Create & Populate Asset Files
   //    file.seekg(bufftoi32(SegmentBuffer, 0, 4), std::ios::beg);
   //    std::ofstream assetFile(filename+std::filesystem::path::preferred_separator+filePath, std::ios::binary);
   //    char assetBuffer[bufftoi32(SegmentBuffer, 4, 8)];
   //    file.read(assetBuffer, bufftoi32(SegmentBuffer, 4, 8));
   //    assetFile << static_cast<std::string>(assetBuffer);

   //    assetFile.close();
   // }