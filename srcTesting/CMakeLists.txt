cmake_minimum_required(VERSION 3.23)

project(testing)

#stop static attachment of DLL
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-Bdynamic")
#strip symbols in release mode
set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} -s")

# Optional: ensure release optimization if not already set
set(CMAKE_BUILD_TYPE Release)


add_executable(MyProgram)


target_sources(MyProgram
  PRIVATE
    main.c
	GDIEngineSource.c
	GDISpriteSource.c
)

target_link_libraries(MyProgram PRIVATE user32 gdi32 kernel32)