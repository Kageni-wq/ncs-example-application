#ifndef _RGB_LED_H_
#define _RGB_LED_H_

#include <stdint.h>

/*
 *Colors
 */
#define LED_OFF 0, 0, 0
#define LED_RED 255, 0, 0
#define LED_GREEN 0, 255, 0
#define LED_BLUE 0, 0, 255
#define LED_WHITE 255, 255, 255
#define LED_PINK 255, 20, 147
#define LED_PURPLE 230, 0, 255
#define LED_DARK_PURPLE 80, 0, 200
#define LED_RED_PURPLE 135, 0, 116
#define LED_ORANGE 255, 128, 0
#define LED_DARK_BLUE 0, 0, 139
#define LED_LIGHT_BLUE 173, 216, 230
#define LED_DARK_GREEN 0, 100, 0
#define LED_YELLOW 255, 255, 0
#define LED_LIGHT_PURPLE 200, 162, 200
#define LED_TURQUOISE 64, 224, 208
#define LED_DARK_PINK 255, 20, 147
#define LED_CHARGE 220, 50, 245

int rgb_led_init(void);
int rgb_led_set_pixel(int pixel, int r, int g, int b);
int rgb_led_set_all(int r, int g, int b);
int rgb_led_state_update(void);
struct led_rgb rgb_apply_gamma(int r, int g, int b);

#endif