cmake_minimum_required(VERSION 3.16)
project(hwaccel_check CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
# ABSEIL_DIR must point at a checkout whose absl/base/options.h has
# ABSL_OPTION_INLINE_HW_ACCEL_STRATEGY set to 1 or 2.
if(NOT DEFINED ABSEIL_DIR)
  message(FATAL_ERROR "pass -DABSEIL_DIR=/path/to/abseil-cpp")
endif()
set(ABSL_BUILD_TESTING OFF CACHE BOOL "" FORCE)
set(ABSL_PROPAGATE_CXX_STD ON CACHE BOOL "" FORCE)
add_subdirectory(${ABSEIL_DIR} abseil-build EXCLUDE_FROM_ALL)
add_executable(hwaccel_check hwaccel_check.cc)
# -msse4.2 gives the header CRC32 path (strategy 2); strategy 1 requires it too.
# -maes additionally selects the x86 AES backend for len>32 in hash.cc.
target_compile_options(hwaccel_check PRIVATE -O2 -msse4.2 -maes)
target_link_libraries(hwaccel_check absl::hash absl::flat_hash_set absl::flat_hash_map absl::raw_hash_set)
