# set minimum cmake version
cmake_minimum_required(VERSION 3.5)

# project name and language
project(ebook_demo)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -O2")

if(ENABLE_ASAN)
  message(STATUS "BUILD WITH ADDRESS SANTITIZE")
  set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -fno-omit-frame-pointer -fsanitize=address")
  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fno-omit-frame-pointer -fsanitize=address")
  set(CMAKE_LINKER_FLAGS_DEBUG "${CMAKE_LINKER_FLAGS_DEBUG} -fno-omit-frame-pointer -fsanitize=address")
endif()

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

if(CMAKE_SYSTEM_NAME STREQUAL "Android")
else()
  if(CMAKE_C_COMPILER MATCHES "aarch64")
    set(LIB_ARCH aarch64)
  else()
    set(LIB_ARCH armhf)
  endif()
endif()

message(STATUS "Build test for soc: ${TARGET_SOC} PATH=${CMAKE_CURRENT_SOURCE_DIR}")

if(TARGET_SOC STREQUAL "rk356x")
  add_definitions(-DENABLE_RK3566)
elseif(TARGET_SOC STREQUAL "rk3588")
  add_definitions(-DENABLE_RK3588)
elseif(TARGET_SOC STREQUAL "rk3576")
    add_definitions(-DENABLE_RK3576)
else()
  message(FATAL_ERROR "TARGET_SOC is not set, ref value: rk356x or rk3588")
endif()

# demo
set(ebook_demo_SRCS
    src/main.cpp
)
add_executable(ebook_demo ${ebook_demo_SRCS})

include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  ${CMAKE_SOURCE_DIR}/allocator/include
)

target_link_libraries(ebook_demo
  ebook
  z
  dl
  allocator
)

if(ANDROID)
  target_link_libraries(ebook_demo log c)
endif()

INSTALL(TARGETS ebook_demo DESTINATION bin/${CMAKE_SYSTEM_NAME}/${TARGET_SOC}/${CMAKE_ANDROID_ARCH_ABI})
