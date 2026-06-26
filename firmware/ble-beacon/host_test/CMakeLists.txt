# Host (Linux-target) unit tests for the BLE-beacon pure-logic components.
#   idf.py --preview set-target linux
#   idf.py build
#   ./build/ble_beacon_host_test.elf
cmake_minimum_required(VERSION 3.16)
set(EXTRA_COMPONENT_DIRS "${CMAKE_CURRENT_LIST_DIR}/../components")
include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(ble_beacon_host_test)
