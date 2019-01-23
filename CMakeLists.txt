cmake_minimum_required(VERSION 3.7)
project(oled)

set(CMAKE_CXX_STANDARD 11)

set(SRC_DIR ${CMAKE_SOURCE_DIR}/ssd1306)
include_directories(. ${SRC_DIR})
set(SRC_FILES
        ${SRC_DIR}/ssd1306.c
        ${SRC_DIR}/canvas.c
        ${SRC_DIR}/qrcode/qr_encode.c
        )

add_executable(${PROJECT_NAME} ${SRC_FILES})
target_link_libraries(${PROJECT_NAME} m bcm2835)