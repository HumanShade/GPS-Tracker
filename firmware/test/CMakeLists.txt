# Portable host-side logic unit tests (Phase 0 + Phase 1).
#
# Runs natively on the dev PC with any C compiler -- NOT the ESP-IDF "linux"
# target (which doesn't build on native Windows). Tests pure-logic component
# sources compiled for the host and linked against vendored Unity.
#
#   cmake -S firmware/test -B firmware/test/build -G "MinGW Makefiles"
#   cmake --build firmware/test/build
#   ctest --test-dir firmware/test/build --output-on-failure
cmake_minimum_required(VERSION 3.16)
project(gps_tracker_host_tests C)

enable_testing()

# --- vendored Unity (see vendor/unity/README.md) ---
set(UNITY_DIR ${CMAKE_CURRENT_LIST_DIR}/vendor/unity)
add_library(unity STATIC ${UNITY_DIR}/unity.c)
target_include_directories(unity PUBLIC ${UNITY_DIR})

set(FW ${CMAKE_CURRENT_LIST_DIR}/..)

# --- Phase 0: banner ---
add_executable(test_banner
    test_banner.c
    ${FW}/hello-world/components/banner/banner.c)
target_include_directories(test_banner PRIVATE
    ${FW}/hello-world/components/banner/include)
target_link_libraries(test_banner PRIVATE unity)
add_test(NAME banner COMMAND test_banner)

# --- Phase 1a: apple_find_my ---
add_executable(test_apple_find_my
    test_apple_find_my.c
    ${FW}/ble-beacon/components/apple_find_my/apple_find_my.c)
target_include_directories(test_apple_find_my PRIVATE
    ${FW}/ble-beacon/components/apple_find_my/include)
target_link_libraries(test_apple_find_my PRIVATE unity)
add_test(NAME apple_find_my COMMAND test_apple_find_my)
