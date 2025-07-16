#include <stdio.h>
#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/zbus/zbus.h>
#include <zephyr/drivers/pwm.h>
#include <errno.h>
#include "button_handler.h"
#include "main.h"
#include "button_assignments.h"
#include "zbus_common.h"
#include "macros_common.h"
#include "system.h"
#include "debug_led.h"
#include "rgb_led.h"
#include "torch_led.h"

LOG_MODULE_REGISTER(system, LOG_LEVEL_DBG);

ZBUS_SUBSCRIBER_DEFINE(button_evt_sub_sys, CONFIG_BUTTON_MSG_SUB_QUEUE_SIZE);
ZBUS_SUBSCRIBER_DEFINE(device_evt_sub_sys, CONFIG_DEVICE_MSG_SUB_QUEUE_SIZE);
ZBUS_SUBSCRIBER_DEFINE(audio_evt_sub_sys, CONFIG_AUDIO_MSG_SUB_QUEUE_SIZE);

ZBUS_CHAN_DECLARE(button_chan);
ZBUS_CHAN_DECLARE(device_chan);
ZBUS_CHAN_DECLARE(audio_evt_chan);

static struct k_thread button_msg_sys_sub_thread_data;
static k_tid_t button_msg_sys_sub_thread_id;

K_THREAD_STACK_DEFINE(button_msg_sys_sub_thread_stack, CONFIG_SYSTEM_THREAD_STACK_SIZE);



System_t system;

System_t *GetSystem(void)
{
    return &system;
}

Audio_t *GetSystemAudio(void)
{
    return &system.audio;
}

static void button_msg_sys_sub_thread(void)
{
    int ret;
    const struct zbus_channel *chan;

    while (1)
    {
        ret = zbus_sub_wait(&button_evt_sub_sys, &chan, K_FOREVER);
        ERR_CHK(ret);

        struct button_msg msg;

        ret = zbus_chan_read(chan, &msg, ZBUS_READ_TIMEOUT_MS);
        ERR_CHK(ret);

        LOG_DBG("Got btn evt from queue - id = %d, action = %d", msg.button_pin,
                msg.button_action);

        if (msg.button_action != BUTTON_PRESS)
        {
            LOG_WRN("Unhandled button action");
            return;
        }

        switch (msg.button_pin)
        {
        case BUTTON_0:
            LOG_DBG("Button 0 pressed");
            torch_led_power_control(0, 1);
            break;

        case BUTTON_1:
            LOG_DBG("Button 1 pressed");
            torch_led_power_control(0, 0);
            break;

        case BUTTON_2:
            LOG_DBG("Button 2 pressed");
            rgb_led_state_update();
            break;

        case BUTTON_3:
            LOG_DBG("Button 3 pressed");
            debug_led_toggle(3);
            break;
        default:
            LOG_WRN("Unexpected/unhandled button id: %d", msg.button_pin);
        }

        STACK_USAGE_PRINT("button_msg_thread", &button_msg_sub_thread_data);
    }
}

static void system_handle_device_event(struct device_msg *msg)
{
    // Handle device events
}

static void system_handle_audio_event(struct audio_msg *msg)
{
    // Handle audio events
}

int system_init(void)
{
    int ret;

    LOG_INF("Initializing system");
    memset(&system, 0, sizeof(system));

    // Initialize led
    ret = torch_led_init();
    if (ret < 0) {
        return 0;
    }

    ret = rgb_led_init();
    if (ret < 0) {
        return 0;
    }

    ret = debug_led_init();
    if (ret < 0) {
        return 0;
    }

    button_msg_sys_sub_thread_id = k_thread_create(
        &button_msg_sys_sub_thread_data, button_msg_sys_sub_thread_stack,
        CONFIG_SYSTEM_THREAD_STACK_SIZE, (k_thread_entry_t)button_msg_sys_sub_thread, NULL,
        NULL, NULL, K_PRIO_PREEMPT(CONFIG_SYSTEM_THREAD_PRIO), 0, K_NO_WAIT);
    ret = k_thread_name_set(button_msg_sys_sub_thread_id, "BUTTON_MSG_SYS_SUB");
    if (ret)
    {
        LOG_ERR("Failed to create system button thread");
        return ret;
    }

    ret = zbus_chan_add_obs(&button_chan, &button_evt_sub_sys, ZBUS_ADD_OBS_TIMEOUT_MS);
    if (ret)
    {
        LOG_ERR("Failed to add button sub");
        return ret;
    }

    ret = zbus_chan_add_obs(&device_chan, &device_evt_sub_sys, ZBUS_ADD_OBS_TIMEOUT_MS);
    if (ret)
    {
        LOG_ERR("Failed to add device sub");
        return ret;
    }

    ret = zbus_chan_add_obs(&audio_evt_chan, &audio_evt_sub_sys, ZBUS_ADD_OBS_TIMEOUT_MS);
    if (ret)
    {
        LOG_ERR("Failed to add audio sub");
        return ret;
    }

    system.state = SYSTEM_STATE_IDLE;

    return ret;
}


