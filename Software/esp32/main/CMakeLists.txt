idf_component_register(SRCS "M-heart.c"
                       INCLUDE_DIRS "."
                       
                       REQUIRES astra_ui rotary_encoder beep adc externdraw firetool_PID_adaptor heater nvs timer wifi)
