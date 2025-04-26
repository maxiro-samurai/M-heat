
idf_component_register(SRCS  "heater.c"                        
                       INCLUDE_DIRS "."
                       REQUIRES  driver astra_ui timer adc firetool_PID_adaptor)