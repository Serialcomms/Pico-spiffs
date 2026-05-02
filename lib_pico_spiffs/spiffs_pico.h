
#define RAM_ONLY_FUNCTION __no_inline_not_in_flash_func

#define PICO_RAM_ONLY_DATA  __attribute__((section(".data")))
#define PICO_ALIGNED_WORD   __attribute__((aligned(4)))

#define PICO_RAM_ONLY_ALIGNED_DATA  __attribute__((section(".data"))) __attribute__((aligned(4)))
//  __attribute__((section(".data"))) __attribute__((aligned(4)))

//RAM_DATA uint32_t coefficients[128];

#define PICO_GPIO_SPIFFS_ACTIVITY_LED_READ  26
#define PICO_GPIO_SPIFFS_ACTIVITY_LED_WRITE 27
#define PICO_GPIO_SPIFFS_ACTIVITY_LED_ERASE 28

void pico_spiffs_mount_filesystem();
void pico_spiffs_gpio_init_activity_leds();

void test_posix();