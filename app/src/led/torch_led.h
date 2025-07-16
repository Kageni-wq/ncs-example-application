#ifndef _TORCH_LED_H_
#define _TORCH_LED_H_

#define R_LED_ENABLE DT_ALIAS(right_led)
#define L_LED_ENABLE DT_ALIAS(left_led)
#define TORCH_PWM DT_ALIAS(torch_pwm)

int torch_led_init(void);
int torch_led_power_control(int led, int on);
int torch_led_set_brightness(int led, int brightness);

#endif