#-------------------------------------------------------------------------------
# Zephyr Example Application
#
# Copyright (c) 2021 Nordic Semiconductor ASA
# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.13.1)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})

project(app LANGUAGES C)

target_sources(app PRIVATE src/main.c)

add_subdirectory(src/modules)
add_subdirectory(src/drivers/at_master)
add_subdirectory(src/audio)
add_subdirectory(src/power)
add_subdirectory(src/led)

target_include_directories(app PRIVATE inc)
target_include_directories(app PRIVATE src/modules)
target_include_directories(app PRIVATE src/drivers/at_master)
target_include_directories(app PRIVATE src/audio)
target_include_directories(app PRIVATE src/power)
target_include_directories(app PRIVATE src/led)
