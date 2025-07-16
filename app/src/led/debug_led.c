#include "main.h"
#include "debug_led.h"
#include <zephyr/drivers/gpio.h>
#include <zephyr/sys/util.h>

LOG_MODULE_REGISTER(debug_led, CONFIG_MODULE_LED_LOG_LEVEL);



void debug_led_toggle(int led)
{
	switch (led)
	{
	case 0:

		break;
	case 1:

		break;
	case 2:

		break;
	case 3:

		break;
	
	default:
		break;
	}
}

int debug_led_init(void)
{
    int ret;
	size_t color = 0;

    LOG_INF("Initializing Debug LEDs");

    return ret;
}