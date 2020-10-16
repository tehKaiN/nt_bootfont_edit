cmake_minimum_required(VERSION 3.5)

project(bootfont LANGUAGES C)
set(CMAKE_C_STANDARD 11)

add_executable(bootfont_edit src/main.c src/main.rc src/defines.h src/res_id.h)
target_compile_options(bootfont_edit PRIVATE -Wall -Wextra)
target_link_libraries(bootfont_edit PRIVATE comctl32 gdi32 comdlg32)
target_link_options(bootfont_edit PRIVATE -mwindows)
