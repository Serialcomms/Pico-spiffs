
#include "pico/bootrom.h"
#include "pico/stdlib.h"
#include "pico/printf.h"
#include "pico/status_led.h"

#include "pico/flash.h"
#include "hardware/flash.h"

#include "spiffs_pico.h"

bool ERASE_FIRST = false;

void reboot_pico();

int main() {

    stdio_uart_init();

    if (ERASE_FIRST) {

        printf("Erasing flash range 0x0100000 ...\n");

        flash_range_erase(0x0100000, (1024 * 1024));

        printf("Flash range 0x0100000 Erased\n");

        reboot_pico();

    }
    
    pico_spiffs_mount_filesystem();

    test_posix();

    reboot_pico();

}


void reboot_pico() {

	printf("\n\n");

	printf("Rebooting Pico to boot select mode ...\n");
	
    status_led_init();

    absolute_time_t reboot_time = make_timeout_time_ms(1000);

    do {

        busy_wait_ms(50); 
 
        bool status_led_state = status_led_get_state();
        
        status_led_set_state(!status_led_state);
        
    } while (time_reached(reboot_time) == false);

    status_led_set_state(false);

    rom_reset_usb_boot(0, 0);

}	

