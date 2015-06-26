project(events)
cmake_minimum_required(VERSION 2.8)
enable_testing()

include_directories(${events_SOURCE_DIR}/include)

# Enable warning level 4 in Visual Studio
string(TOLOWER ${CMAKE_BUILD_TOOL} build_tool)
if(build_tool MATCHES "(msdev|devenv|nmake|msbuild)")
  add_definitions(/W4)
endif()

add_subdirectory(tests)
