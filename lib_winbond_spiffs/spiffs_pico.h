
#define RAM_ONLY_FUNCTION __no_inline_not_in_flash_func

#define PICO_GPIO_SPIFFS_ACTIVITY_LED_READ  26
#define PICO_GPIO_SPIFFS_ACTIVITY_LED_WRITE 27
#define PICO_GPIO_SPIFFS_ACTIVITY_LED_ERASE 28

void pico_spiffs_mount_filesystem();
void pico_spiffs_gpio_init_activity_leds();

void test_posix();