cmake_minimum_required(VERSION 3.14)
project(test)

# GoogleTest requires at least C++14
set(CMAKE_CXX_STANDARD 14)

include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG release-1.12.1
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

include_directories("../src"
                    "../src/user"
                    "../src/deps/embedded-external-flash-memory"
                    "../src/app")

add_compile_definitions(UNIT_TEST_FRAMEWORK
                        SERIAL_DEBUG_EN
                        UNIT_TEST)

file(GLOB sources
  "../src/*.h"
  "../src/*.c"
  "../src/user/*.h"
  "../src/deps/embedded-external-flash-memory/*.h"
  "../src/deps/embedded-external-flash-memory/*.c"
  "../src/app/*.h"
  "../src/app/*.c"
  "*.c"
  "*.h")

add_executable(
  bootloader_tests
  bootloader_tests.cc
  ${sources}
)

target_link_libraries(
  bootloader_tests
  GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(bootloader_tests)