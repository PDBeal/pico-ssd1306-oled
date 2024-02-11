
cmake_minimum_required(VERSION 3.12)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

project(pico-ssd1306-oled
    VERSION 1.0.0
    DESCRIPTION "RP2040 library for controlling SSD1306 OLED display using pico-sdk"
)

add_library(${PROJECT_NAME} INTERFACE
    ssd1306_oled.cpp
    ssd1306_oled_font.cpp
    ssd1306_oled_graphics.cpp
    ssd1306_oled_print.cpp
)

target_sources(${PROJECT_NAME} INTERFACE
        ${CMAKE_CURRENT_LIST_DIR}/ssd1306_oled.cpp
        ${CMAKE_CURRENT_LIST_DIR}/ssd1306_oled_font.cpp
        ${CMAKE_CURRENT_LIST_DIR}/ssd1306_oled_graphics.cpp
        ${CMAKE_CURRENT_LIST_DIR}/ssd1306_oled_print.cpp
)

target_include_directories(${PROJECT_NAME} INTERFACE ${CMAKE_CURRENT_LIST_DIR})

#if(PICO_SDK)
#    pico_generate_pio_header(${PROJECT_NAME} ${CMAKE_CURRENT_LIST_DIR}/ws2812.pio)
#endif(PICO_SDK)

# Link to pico_stdlib (gpio, time, etc. functions)
target_link_libraries(${PROJECT_NAME} INTERFACE
    pico_stdlib
    hardware_i2c
)
