cmake_minimum_required(VERSION 4.0)
project(BlkBuilder)

set(CMAKE_CXX_STANDARD 14)

include_directories(lib/lodepng)
include_directories(lib/nlohmann)
include_directories(lib/vc_x64_lib)
include_directories(lib/vc_x64_lib/mswu)
include_directories(lib/vc_x64_lib/mswu/wx)
include_directories(lib/vc_x64_lib/mswu/wx/msw)
include_directories(lib/vc_x64_lib/mswud)
include_directories(lib/vc_x64_lib/mswud/wx)
include_directories(lib/vc_x64_lib/mswud/wx/msw)

#add_executable(BlkBuilder
#        lib/lodepng/lodepng.cpp
#        lib/lodepng/lodepng.h
#        lib/nlohmann/json.hpp
#        lib/nlohmann/json_fwd.hpp
#        lib/vc_x64_lib/mswu/wx/msw/rcdefs.h
#        lib/vc_x64_lib/mswu/wx/setup.h
#        lib/vc_x64_lib/mswud/wx/msw/rcdefs.h
#        lib/vc_x64_lib/mswud/wx/setup.h
#        main.cpp
#        lib/blkutils.h
#        lib/blkutils.cpp
#        main.h)



add_library(BlkBuilderDll SHARED main.cpp lib/blkutils.cpp lib/lodepng/lodepng.cpp)
target_link_libraries(BlkBuilderDll PRIVATE
        ${CMAKE_SOURCE_DIR}/lib/libwinpthread-1.dll
)
target_link_libraries(BlkBuilderDll PRIVATE
        -static-libgcc
        -static-libstdc++
        -Wl,-Bstatic
        winpthread
        -Wl,-Bdynamic
)
target_include_directories(BlkBuilderDll PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

include(GenerateExportHeader)
generate_export_header(BlkBuilderDll
                        BASE_NAME BlkBuilderDll
                        EXPORT_FILE_NAME BlkBuilderDll_export.h)

set_target_properties(BlkBuilderDll PROPERTIES RUNTIME_OUTPUT_DIRECTORY "..\\BlkBuilder\\BlkBuilder\\bin\\Debug\\net9.0")
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

target_compile_features(BlkBuilderDll PRIVATE cxx_std_20)