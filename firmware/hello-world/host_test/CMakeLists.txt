# Host (Linux-target) unit tests for the pure-logic components.
# Build/run with the IDF "linux" target -- no hardware required:
#   idf.py --preview set-target linux
#   idf.py build
#   ./build/hello_world_host_test.elf       (or: build/hello_world_host_test on some hosts)
#
# This is a standalone IDF project; the parent firmware build ignores it (it only
# scans components/ and main/). It reuses the firmware's components via EXTRA_COMPONENT_DIRS.
cmake_minimum_required(VERSION 3.16)
set(EXTRA_COMPONENT_DIRS "${CMAKE_CURRENT_LIST_DIR}/../components")
include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(hello_world_host_test)
