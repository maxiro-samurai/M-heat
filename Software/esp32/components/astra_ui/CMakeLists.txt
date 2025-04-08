idf_component_register(SRCS "u8g2_font_myfont.c" "astra_ui_core.c"
                            "astra_ui_draw_driver.c"
                            "astra_ui_drawer.c"
                            "astra_ui_item.c"
                       INCLUDE_DIRS "."
                       REQUIRES u8g2 driver u8g2-hal-esp-idf)
