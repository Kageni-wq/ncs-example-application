#include <stdio.h>
#include <zephyr/kernel.h>
#include <zephyr/zbus/zbus.h>
#include <zephyr/drivers/uart.h>
#include "dsp_audio.h"
#include "at_common.h"
#include "audio_common.h"
#include "zbus_common.h"
#include "macros_common.h"
#include "at_dsp.h"

LOG_MODULE_REGISTER(dsp_audio, CONFIG_DSP_AUDIO_LOG_LEVEL);

ZBUS_SUBSCRIBER_DEFINE(audio_evt_sub_dsp, CONFIG_AUDIO_MSG_SUB_QUEUE_SIZE);

ZBUS_CHAN_DECLARE(audio_evt_chan);
ZBUS_CHAN_DECLARE(device_chan);

K_THREAD_DEFINE(dsp_audio_thread_id, CONFIG_DSP_AUDIO_STACK_SIZE, dsp_audio_thread, NULL, NULL, NULL,
                CONFIG_DSP_AUDIO_THREAD_PRIO, 0, 0);

extern struct k_msgq power_queue;

static const struct device *dsp_uart_dev;
static Audio_Peripheral_t dsp;
static Event_t event;

static char dsp_rx_buf[CONFIG_UART_RX_BUF_SIZE];
static char dsp_event_buf[CONFIG_MAX_EVENT_LENGTH];
static uint8_t dsp_event_len;

Audio_t *GetDspAudio(void)
{
    return &dsp.audio;
}

int GetDspPower(void)
{
    return dsp.power;
}

int GetDspId(void)
{
    return dsp.id;
}

const struct device *GetDspUartDev(void)
{
    return dsp_uart_dev;
}

bool IsDspInit(void)
{
    return dsp.init;
}

// UART callback
static void dsp_uart_cb(const struct device *dev, struct uart_event *evt, void *user_data)
{
    int ret;
    struct audio_msg msg;

    if (evt->type == UART_RX_RDY) {
        memcpy(dsp_event_buf, evt->data.rx.buf, evt->data.rx.len);
        msg.type = AUDIO_DSP_MSG;
        msg.param = evt->data.rx.len;  // pass length; consider carrying full buffer in struct
        ret = zbus_chan_pub(&audio_evt_chan, &msg, K_NO_WAIT);
        if (ret) {
            LOG_ERR("Failed to publish DSP message: %d", ret);
        }
    }
    if (evt->type == UART_RX_DISABLED) {
        uart_rx_enable(dsp_uart_dev, dsp_rx_buf, CONFIG_UART_RX_BUF_SIZE, CONFIG_UART_RX_TIMEOUT_MS);
    }
}

// DSP thread
void dsp_audio_thread(void *a, void *b, void *c)
{
    int ret;
	const struct zbus_channel *chan;

    dsp_init();

    while (1) {
        ret = zbus_sub_wait(&audio_evt_sub_dsp, &chan, K_FOREVER);
        ERR_CHK(ret);

        struct audio_msg msg;

        ret = zbus_chan_read(chan, &msg, ZBUS_READ_TIMEOUT_MS);
        ERR_CHK(ret);

        Audio_t *audio = GetSystemAudio();

        switch (msg.type) {
        case AUDIO_START:
            dsp_audio_init(audio);
            break;
        case AUDIO_STOP:
            dsp_audio_stop(audio);
            break;
        case AUDIO_FILTER_CHANGE:
            dsp_handle_filter_change(audio);
            break;
        case AUDIO_SOURCE_CHANGE:
            dsp_handle_source_change(audio);
            break;
        case AUDIO_VOLUME_CHANGE:
            dsp_handle_volume_change(audio);
            break;
        case AUDIO_DSP_MSG:
            dsp_handle_message(msg.param);
            break;
        default:
            LOG_WRN("DSP thread: unhandled event %d", msg.type);
        }
    }
}

static void dsp_audio_init(Audio_t *audio)
{
    struct power_msg msg = {
        .event = DEVICE_POWER,
        .device = DSP_DEV_ID,
        .param = POWER_ON,
    };

    uart_rx_enable(dsp_uart_dev, dsp_rx_buf, CONFIG_UART_RX_BUF_SIZE, CONFIG_UART_RX_TIMEOUT_MS);
    k_msgq_put(&power_queue, &msg, K_FOREVER);
    dsp.audio.State = AUDIO_STATE_INIT;
}

static void dsp_audio_stop(Audio_t *audio)
{
    AT_SendCmdWith1Param(DSP_DEV_ID, CMD_ADC_POWER, POWER_OFF);
    dsp.audio.State = AUDIO_STATE_STOP;
}

static void dsp_handle_filter_change(Audio_t *audio)
{
    AT_SendCmdWith1Param(DSP_DEV_ID, CMD_ADC_SET_FILTER, audio->Filter);
}

static void dsp_handle_source_change(Audio_t *audio)
{
    AT_SendCmdWith1Param(DSP_DEV_ID, CMD_ADC_SET_MODE, audio->Source);
}

static void dsp_handle_volume_change(Audio_t *audio)
{
    AT_SendCmdWith1Param(DSP_DEV_ID, CMD_ADC_SET_VOLUME, audio->Volume);
}

static void dsp_handle_message(int param)
{
    Event_Data_t event_data = {
        .buffer = dsp_event_buf,
        .len = param,
        .device_id = DSP_DEV_ID,
    };

    AT_ParseEvent(&event_data);
}

int dsp_init(void)
{
    int ret;

    dsp_uart_dev = DEVICE_DT_GET(DT_NODELABEL(uart0));  // change if needed
    if (!device_is_ready(dsp_uart_dev)) {
        LOG_ERR("DSP UART device not ready");
        return -ENODEV;
    }

    ret = uart_callback_set(dsp_uart_dev, dsp_uart_cb, NULL);
    if (ret) {
        LOG_ERR("Failed to set DSP UART callback");
        return ret;
    }

    dsp.id = ATM_DspInit(DSP_DEV_ID);
    dsp.init = true;

    LOG_INF("DSP initialized");
    return 0;
}
