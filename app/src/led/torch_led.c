#include <stdio.h>
#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/zbus/zbus.h>
#include <zephyr/drivers/pwm.h>
#include <errno.h>
#include "main.h"
#include "torch_led.h"

LOG_MODULE_REGISTER(torch_led, CONFIG_MODULE_LED_LOG_LEVEL);

#define PWM_PERIOD_NS 5000
#define MAX_BRIGHTNESS_LEVEL 5

static const struct pwm_dt_spec torch_pwm = PWM_DT_SPEC_GET(TORCH_PWM);
static const struct gpio_dt_spec r_led_control = GPIO_DT_SPEC_GET(R_LED_ENABLE, gpios);
static const struct gpio_dt_spec l_led_control = GPIO_DT_SPEC_GET(L_LED_ENABLE, gpios);

static int left_led_brightness = 0;
static int right_led_brightness = 0;

const uint32_t pulse_width = 2000;


int torch_led_init(void)
{
    int err;

    LOG_INF("Initializing Torch LED");

    if (!pwm_is_ready_dt(&torch_pwm))
    {
        LOG_ERR("Error: PWM device %s is not ready\n", torch_pwm.dev->name);
        return -ENODEV;
    }

    err = gpio_pin_configure_dt(&r_led_control, GPIO_OUTPUT);
    if (err < 0)
    {
        LOG_ERR("Error %d: failed to configure %s pin %d\n", err, r_led_control.port->name, r_led_control.pin);
        return err;
    }

    err = gpio_pin_configure_dt(&l_led_control, GPIO_OUTPUT);
    if (err < 0)
    {
        LOG_ERR("Error %d: failed to configure %s pin %d\n", err, l_led_control.port->name, l_led_control.pin);
        return err;
    }

    return 0;
}

int torch_led_power_control(int led, int on)
{
    int err;
    uint32_t pulse_width = 0;
    uint8_t brightness = 0;
    const struct gpio_dt_spec *led_control;

    if (led == 0)
    {
        brightness = right_led_brightness;
        led_control = &r_led_control;
    }
    else if (led == 1)
    {
        brightness = left_led_brightness;
        led_control = &l_led_control;
    }
    else
    {
        LOG_ERR("Invalid LED: %d", led);
        return -EINVAL;
    }

    if (brightness < 0 || brightness > MAX_BRIGHTNESS_LEVEL)
    {
        LOG_ERR("Invalid brightness level: %d", brightness);
        return -EINVAL;
    }

    if (on)
    {
        pulse_width = brightness * 1000;

        err = pwm_set_dt(&torch_pwm, PWM_PERIOD_NS, pulse_width);
        if (err < 0)
        {
            LOG_ERR("Error %d: failed to set PWM\n", err);
            return err;
        }

        err = gpio_pin_set_dt(led_control, 1);
        if (err < 0)
        {
            LOG_ERR("Error %d: failed to set %s pin %d\n", err, led_control->port->name, led_control->pin);
            return err;
        }
    }

    return 0;
}

int torch_led_set_brightness(int led, int brightness)
{
    if (brightness < 0 || brightness > MAX_BRIGHTNESS_LEVEL)
    {
        LOG_ERR("Invalid brightness level: %d", brightness);
        return -EINVAL;
    }

    if (led == 0)
    {
        left_led_brightness = brightness;
    }
    else if (led == 1)
    {
        right_led_brightness = brightness;
    }
    else
    {
        return -EINVAL;
    }

    return 0;
}
