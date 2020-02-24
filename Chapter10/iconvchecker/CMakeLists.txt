cmake_minimum_required(VERSION 3.20.0)
project(iconvchecker)

find_package(LLVM REQUIRED CONFIG)

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

list(APPEND CMAKE_MODULE_PATH ${LLVM_DIR})
include(AddLLVM)
include(HandleLLVMOptions)

find_package(Clang REQUIRED)
list(APPEND CMAKE_MODULE_PATH ${Clang_DIR})
include(AddClang)

include_directories("${LLVM_INCLUDE_DIR}" "${CLANG_INCLUDE_DIRS}")
add_definitions("${LLVM_DEFINITIONS}")

link_directories("${LLVM_LIBRARY_DIR}")

add_llvm_library(IconvChecker MODULE IconvChecker.cpp PLUGIN_TOOL clang)

if(WIN32 OR CYGWIN)
  set(LLVM_LINK_COMPONENTS
    Support
    )
  clang_target_link_libraries(IconvChecker PRIVATE
    clangAnalysis
    clangAST
    clangStaticAnalyzerCore
    clangStaticAnalyzerFrontend
  )
endif()
