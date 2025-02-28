# CMakeLists.txt
cmake_minimum_required(VERSION 3.10)

# 项目名称
project(wamr)

# 嵌入整个wamr项目进来编译

set (WAMR_BUILD_PLATFORM "windows")

# Reset default linker flags
set (CMAKE_SHARED_LIBRARY_LINK_C_FLAGS "")
set (CMAKE_SHARED_LIBRARY_LINK_CXX_FLAGS "")

add_definitions(-DCOMPILING_WASM_RUNTIME_API=1)

# Set WAMR_BUILD_TARGET, currently values supported:
# "X86_64", "AMD_64", "X86_32", "AARCH64[sub]", "ARM[sub]", "THUMB[sub]", "MIPS", "XTENSA"
if (NOT DEFINED WAMR_BUILD_TARGET)
  if (CMAKE_SIZEOF_VOID_P EQUAL 8)
    # Build as X86_64 by default in 64-bit platform
    set (WAMR_BUILD_TARGET "X86_64")
  elseif (CMAKE_SIZEOF_VOID_P EQUAL 4)
    # Build as X86_32 by default in 32-bit platform
    set (WAMR_BUILD_TARGET "X86_32")
  else ()
    message(FATAL_ERROR "Unsupported build target platform!")
  endif ()
endif ()

if (NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif ()

if (NOT DEFINED WAMR_BUILD_INTERP)
  # Enable Interpreter by default
  set (WAMR_BUILD_INTERP 1)
endif ()

if (NOT DEFINED WAMR_BUILD_AOT)
  # Enable AOT by default.
  set (WAMR_BUILD_AOT 1)
endif ()

if (NOT DEFINED WAMR_BUILD_JIT)
  # Disable JIT by default.
  set (WAMR_BUILD_JIT 0)
endif ()

if (NOT DEFINED WAMR_BUILD_LIBC_BUILTIN)
  # Enable libc builtin support by default
  set (WAMR_BUILD_LIBC_BUILTIN 1)
endif ()

if (NOT DEFINED WAMR_BUILD_LIBC_WASI)
  # Enable libc wasi support by default
  set (WAMR_BUILD_LIBC_WASI 1)
endif ()

if (NOT DEFINED WAMR_BUILD_FAST_INTERP)
  # Enable fast interpreter
  set (WAMR_BUILD_FAST_INTERP 1)
endif ()

if (NOT DEFINED WAMR_BUILD_MULTI_MODULE)
  # Enable multiple modules
  set (WAMR_BUILD_MULTI_MODULE 0)
endif ()

if (NOT DEFINED WAMR_BUILD_LIB_PTHREAD)
  # Disable pthread library by default
  set (WAMR_BUILD_LIB_PTHREAD 0)
endif ()

if (NOT DEFINED WAMR_BUILD_MINI_LOADER)
  # Disable wasm mini loader by default
  set (WAMR_BUILD_MINI_LOADER 0)
endif ()

if (NOT DEFINED WAMR_BUILD_SIMD)
  # Enable SIMD by default
  set (WAMR_BUILD_SIMD 1)
endif ()

if (NOT DEFINED WAMR_BUILD_DEBUG_INTERP)
  # Disable Debug feature by default
  set (WAMR_BUILD_DEBUG_INTERP 0)
endif ()

if (WAMR_BUILD_DEBUG_INTERP EQUAL 1)
  set (WAMR_BUILD_FAST_INTERP 0)
  set (WAMR_BUILD_MINI_LOADER 0)
  set (WAMR_BUILD_SIMD 0)
endif ()

# 支持reference type特性，否则运行有externref的wasm文件会报错
set (WAMR_BUILD_REF_TYPES 1)

if (WAMR_BUILD_LIBC_WASI EQUAL 1)
  set (CMAKE_C_STANDARD 11)
  if (MSVC)
    add_compile_options(/experimental:c11atomics)
  endif()
else()
  set (CMAKE_C_STANDARD 99)
endif()
set (WAMR_ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../wasm-micro-runtime)
include (${WAMR_ROOT_DIR}/build-scripts/runtime_lib.cmake)
add_library(vmlib SHARED ${WAMR_RUNTIME_LIB_SOURCE})


target_include_directories(vmlib INTERFACE
  $<INSTALL_INTERFACE:include/iwasm>
)

set (WAMR_PUBLIC_HEADERS
  ${WAMR_ROOT_DIR}/core/iwasm/include/wasm_c_api.h
  ${WAMR_ROOT_DIR}/core/iwasm/include/wasm_export.h
  ${WAMR_ROOT_DIR}/core/iwasm/include/lib_export.h
)

set_target_properties (vmlib PROPERTIES
  OUTPUT_NAME libiwasm
  PUBLIC_HEADER "${WAMR_PUBLIC_HEADERS}"
  POSITION_INDEPENDENT_CODE ON
)

target_link_libraries (vmlib ${LLVM_AVAILABLE_LIBS} ${UV_A_LIBS})
target_link_libraries(vmlib ntdll)
set_version_info(vmlib)

#target_link_libraries (wamr vmlib)

# 设置输出为动态库
#add_library(wamr SHARED)
#add_executable(wamr src/wamr.c)

#target_link_libraries (wamr vmlib ${LLVM_AVAILABLE_LIBS} ${UV_A_LIBS})
#target_link_libraries(wamr ntdll)

# 设置导出宏
#target_compile_definitions(wamr PRIVATE MY_LIBRARY_EXPORTS)