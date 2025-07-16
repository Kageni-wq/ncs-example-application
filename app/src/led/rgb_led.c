#include "main.h"
#include "rgb_led.h"
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/led_strip.h>
#include <zephyr/drivers/spi.h>
#include <zephyr/sys/util.h>
#include "system.h"

LOG_MODULE_REGISTER(rgb_led, CONFIG_MODULE_LED_LOG_LEVEL);

#define STRIP_NODE		DT_ALIAS(led_strip)

#if DT_NODE_HAS_PROP(DT_ALIAS(led_strip), chain_length)
#define STRIP_NUM_PIXELS	DT_PROP(DT_ALIAS(led_strip), chain_length)
#else
#error Unable to determine length of LED strip
#endif

#define SPI_FREQ    4000000
#define ZERO_FRAME  0x40
#define ONE_FRAME   0x70

#define DELAY_TIME K_MSEC(CONFIG_SAMPLE_LED_UPDATE_DELAY)

#define RGB(_r, _g, _b) { .r = (_r), .g = (_g), .b = (_b) }

// Gamma correction table
const uint8_t gammaTable[] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2,
    2, 3, 3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 4, 5, 5, 5,
    5, 6, 6, 6, 6, 7, 7, 7, 7, 8, 8, 8, 9, 9, 9, 10,
    10, 10, 11, 11, 11, 12, 12, 13, 13, 13, 14, 14, 15, 15, 16, 16,
    17, 17, 18, 18, 19, 19, 20, 20, 21, 21, 22, 22, 23, 24, 24, 25,
    25, 26, 27, 27, 28, 29, 29, 30, 31, 32, 32, 33, 34, 35, 35, 36,
    37, 38, 39, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 50,
    51, 52, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 66, 67, 68,
    69, 70, 72, 73, 74, 75, 77, 78, 79, 81, 82, 83, 85, 86, 87, 89,
    90, 92, 93, 95, 96, 98, 99, 101, 102, 104, 105, 107, 109, 110, 112, 114,
    115, 117, 119, 120, 122, 124, 126, 127, 129, 131, 133, 135, 137, 138, 140, 142,
    144, 146, 148, 150, 152, 154, 156, 158, 160, 162, 164, 167, 169, 171, 173, 175,
    177, 180, 182, 184, 186, 189, 191, 193, 196, 198, 200, 203, 205, 208, 210, 213,
    215, 218, 220, 223, 225, 228, 231, 233, 236, 239, 241, 244, 247, 249, 252, 255
};

static const struct led_rgb colors[] = {
	RGB(CONFIG_SAMPLE_LED_BRIGHTNESS, 0x00, 0x00), /* red */
	RGB(0x00, CONFIG_SAMPLE_LED_BRIGHTNESS, 0x00), /* green */
	RGB(0x00, 0x00, CONFIG_SAMPLE_LED_BRIGHTNESS), /* blue */
};

static struct led_rgb pixels[STRIP_NUM_PIXELS];

static const struct device *const strip = DEVICE_DT_GET(STRIP_NODE);

int rgb_led_init(void)
{
    int ret;

    LOG_INF("Initializing RGB LEDs");

	if (device_is_ready(strip)) {
		LOG_INF("Found LED strip device %s", strip->name);
	} else {
		LOG_ERR("LED strip device %s is not ready", strip->name);
		return 0;
	}

    ret = rgb_led_set_all(LED_PINK); // Pink
    
    return ret;
}

int rgb_led_set_pixel(int pixel, int r, int g, int b)
{
    int ret;

    pixels[pixel].r = gammaTable[r];
    pixels[pixel].g = gammaTable[g];
    pixels[pixel].b = gammaTable[b];

    ret = led_strip_update_rgb(strip, pixels, STRIP_NUM_PIXELS);
    if (ret) {
        LOG_ERR("couldn't update strip: %d", ret);
    }

    return ret;
}

int rgb_led_set_all(int r, int g, int b)
{
    int ret;
    struct led_rgb color;

    LOG_INF("Setting entire strip");

    color = rgb_apply_gamma(r, g, b);

    for (size_t cursor = 0; cursor < ARRAY_SIZE(pixels); cursor++) {
        pixels[cursor] = color;
    }

    ret = led_strip_update_rgb(strip, pixels, STRIP_NUM_PIXELS);
    if (ret) {
        LOG_ERR("couldn't update strip: %d", ret);
    }

    return ret;
}

int rgb_led_state_update(void)
{
    LOG_INF("RGB LED State Update");
    int ret;
    int led_index = 0;
    int left_half = STRIP_NUM_PIXELS / 2;
    struct led_rgb color;
    const System_t *sys = GetSystem();
    BTState bt = 0;

    if (sys->audio.State == AUDIO_STATE_RESET)
    {
        return rgb_led_set_all(LED_OFF);
    }

    switch (sys->audio.Filter)
    {
    case FILTER_BALANCED:
        color = rgb_apply_gamma(LED_DARK_PURPLE);
        break;
    case FILTER_HEART:
        color = rgb_apply_gamma(LED_DARK_PINK);
        break;
    case FILTER_LUNG:
        color = rgb_apply_gamma(LED_TURQUOISE);
        break;
    case FILTER_NONE:
        color = rgb_apply_gamma(LED_ORANGE);
        break;
    
    default:
        break;
    }

    for (led_index = 0; led_index < left_half; led_index++)
    {
        pixels[led_index] = color;
    }

    if (sys->audio.Out == AUDIO_OUT_A2DP)
    {
        bt = sys->a2dp.state;
    }
    else if (sys->audio.Out == AUDIO_OUT_BLE)
    {
        bt = sys->ble.state;
    }
    else if (sys->audio.Out == AUDIO_OUT_USB)
    {
        bt = 10;
        color = rgb_apply_gamma(LED_LIGHT_BLUE);
    }

    if (bt <= BT_CONNECTED)
    {
        switch (bt)
        {
        case BT_DISCONNECTED:
            color = rgb_apply_gamma(LED_DARK_BLUE);
            break;
        case BT_DISCOVERABLE:
            color = rgb_apply_gamma(LED_PINK);
            break;
        case BT_CONNECTING:
            color = rgb_apply_gamma(LED_ORANGE);
            break;
        case BT_CONNECTED:
            color = rgb_apply_gamma(LED_PURPLE);
            break;
        case BT_DISCONNECTING:
            color = rgb_apply_gamma(LED_ORANGE);
            break;
        
        default:
            break;
        }
    }

    for (led_index = left_half; led_index < STRIP_NUM_PIXELS; led_index++)
    {
        pixels[led_index] = color;
    }

    ret = led_strip_update_rgb(strip, pixels, STRIP_NUM_PIXELS);
    if (ret) {
        LOG_ERR("couldn't update strip: %d", ret);
    }

    return ret;
}

struct led_rgb rgb_apply_gamma(int r, int g, int b)
{
    struct led_rgb color;

    color.r = gammaTable[r];
    color.g = gammaTable[g];
    color.b = gammaTable[b];

    return color;
}