cmake_minimum_required(VERSION 3.31.12)
include($ENV{IDF_PATH}/tools/cmake/project.cmake)
idf_build_set_property(MINIMAL_BUILD ON)
project(ESP_WiFi testing)
