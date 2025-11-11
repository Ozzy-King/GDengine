cmake_minimum_required(VERSION 3.23)

project(testing C)

# Stop static attachment of DLLs (use dynamic linking)
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-Bdynamic")

# Strip symbols in release mode
set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} -s")

# Ensure release optimization if not already set
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

add_executable(MyProgram
    main.c
    GDIEngineSource.c
    GDISpriteSource.c
)

# MinGW-specific settings
if(MINGW)
    target_compile_options(MyProgram PRIVATE -Os -ffunction-sections -fdata-sections)
    target_link_options(MyProgram PRIVATE -Wl,--gc-sections -s)
endif()

# Link Windows system libraries
target_link_libraries(MyProgram 
	PRIVATE 
	user32 
	gdi32 
	kernel32
	windowscodecs
    ole32
    uuid
)
