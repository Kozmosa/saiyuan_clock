idf_build_get_property(target IDF_TARGET)

if(${target} STREQUAL "linux")
    set(requires esp_event esp_stubs protocol_examples_common nvs_flash driver esp_driver_gpio)
endif()

if("${CONFIG_EXAMPLE_IPV4}" STREQUAL y)
    set(tcp_client_ip tcp_client_v4.c)
else()
    set(tcp_client_ip tcp_client_v6.c)
endif()

idf_component_register(SRCS "bsp_matrixkey.c" "activity.c" "tcp_client_main.c" "${tcp_client_ip}" "OLEDDisplay.c" "OLEDDisplayFonts.c" "utils.c" "activity.c"
                                INCLUDE_DIRS "."
                                REQUIRES ${requires})