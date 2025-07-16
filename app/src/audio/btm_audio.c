#include <stdio.h>
#include <zephyr/kernel.h>
#include <zephyr/zbus/zbus.h>
#include <zephyr/drivers/uart.h>
#include "btm_audio.h"
#include "at_common.h"
#include "audio_common.h"
#include "zbus_common.h"
#include "macros_common.h"
#include "at_btm.h"

LOG_MODULE_REGISTER(btm_audio, CONFIG_BTM_AUDIO_LOG_LEVEL);

ZBUS_SUBSCRIBER_DEFINE(audio_evt_sub_btm, CONFIG_AUDIO_MSG_SUB_QUEUE_SIZE);

ZBUS_CHAN_DECLARE(audio_evt_chan);
ZBUS_CHAN_DECLARE(device_chan);

K_THREAD_DEFINE(btm_audio_thread_id, CONFIG_BTM_AUDIO_STACK_SIZE, btm_audio_thread, NULL, NULL, NULL,
                CONFIG_BTM_AUDIO_THREAD_PRIO, 0, 0);

extern struct k_msgq power_queue; 

static const struct device *btm_uart_dev;
static Audio_Peripheral_t btm;
static Event_t event;

static char btm_rx_buf[CONFIG_UART_RX_BUF_SIZE];
static char btm_event_buf[CONFIG_MAX_EVENT_LENGTH];
static uint8_t btm_event_len;    

Audio_t *GetBtmAudio(void)
{
    return &btm.audio;
}

int GetBtmPower(void)
{
    return btm.power;
}

int GetBtmId(void)
{
    return btm.id;
}

const struct device *GetBtmUartDev(void)
{
    return btm_uart_dev;
}

bool IsBtmInit(void)
{
    return btm.init;
}

// UART callback
static void btm_uart_cb(const struct device *dev, struct uart_event *evt, void *user_data) {
    int ret;
    struct audio_msg msg;
    if (evt->type == UART_RX_RDY) {
        memcpy(btm_event_buf, evt->data.rx.buf, evt->data.rx.len);
        msg.type = AUDIO_BTM_MSG;
        msg.param = evt->data.rx.len;
        ret = zbus_chan_pub(&audio_evt_chan, &msg, K_NO_WAIT);
        if (ret) {
            LOG_ERR("Failed to publish message: %d", ret);
        }
    }
    if (evt->type == UART_RX_DISABLED) {
        uart_rx_enable(btm_uart_dev, btm_rx_buf, CONFIG_UART_RX_BUF_SIZE, CONFIG_UART_RX_TIMEOUT_MS);
    }
}

// BTM thread (listens for audio events)
void btm_audio_thread(void *a, void *b, void *c) {
    int ret;
	const struct zbus_channel *chan;

    btm_init();

    while (1) {
        ret = zbus_sub_wait(&audio_evt_sub_btm, &chan, K_FOREVER);
        ERR_CHK(ret);

        struct audio_msg msg;

        ret = zbus_chan_read(chan, &msg, ZBUS_READ_TIMEOUT_MS);
        ERR_CHK(ret);

        Audio_t *audio = GetSystemAudio();

        switch (msg.type) {
        case AUDIO_START:
            if (audio->Out == AUDIO_OUT_A2DP){
                btm_audio_init(audio);
            }
            break;
        case AUDIO_STOP:
            if (audio->Out == AUDIO_OUT_A2DP){
                btm_audio_stop(audio);
            }
            break;
        case AUDIO_FILTER_CHANGE:
            if (audio->Out == AUDIO_OUT_A2DP){
                btm_handle_filter_change(audio);
            }
            break;
        case AUDIO_OUTPUT_CHANGE:
            btm_handle_output_change(audio);
            break;
        case AUDIO_VOLUME_CHANGE:
            if (audio->Out == AUDIO_OUT_A2DP){
                btm_handle_volume_change(audio);
            }
            break;
        case AUDIO_BTM_MSG:
            btm_handle_message(msg.param);
            break;
        default:
            LOG_WRN("BTM thread: unhandled event %d", msg.type);
        }
    }
}

static void btm_audio_init(Audio_t *audio)
{
    struct power_msg msg = {
        .event = DEVICE_POWER,
        .device = BTM_DEV_ID,
        .param = POWER_ON,
    };

    uart_rx_enable(btm_uart_dev, btm_rx_buf, CONFIG_UART_RX_BUF_SIZE, CONFIG_UART_RX_TIMEOUT_MS);
    k_msgq_put(&power_queue, &msg, K_FOREVER);
    btm.audio.State = AUDIO_STATE_INIT;
}

static void btm_audio_start(Audio_t *audio)
{
    btm.audio.State = AUDIO_STATE_POWERED;
    AT_SendCmdWith3Params(BTM_DEV_ID, CMD_BTM_START, audio->Out, audio->Filter, audio->Volume);

}

static void btm_audio_stop(Audio_t *audio)
{
    AT_SendCmdWith1Param(BTM_DEV_ID, CMD_BTM_POWER_OFF, 0);
}

static void btm_handle_filter_change(Audio_t *audio)
{
    // TODO: Implement filter change handling
}

static void btm_handle_output_change(Audio_t *audio)
{
    if (audio->Out == AUDIO_OUT_A2DP){
        btm_audio_init(audio);
    } else {
        if (btm.power == 1){
            btm_audio_stop(audio);
        }
    }
}

static void btm_handle_volume_change(Audio_t *audio)
{
    AT_SendCmdWith1Param(BTM_DEV_ID, CMD_BTM_SET_VOLUME, audio->Volume);
}

static void btm_create_event(int event_type, int param)
{
    struct device_msg msg = {
        .event = event_type,
        .param = param,
    };

    int ret = zbus_chan_pub(&device_chan, &msg, K_NO_WAIT);
    if (ret) {
        LOG_ERR("Failed to publish message: %d", ret);
    }
}

static void btm_handle_message(int param)
{
    Event_Data_t event_data = {
        .buffer = btm_event_buf,
        .len = param,
        .device_id = BTM_DEV_ID,
    };

    AT_ParseEvent(&event_data);
}

int btm_init(void)
{
    int ret;

    btm_uart_dev = DEVICE_DT_GET(DT_NODELABEL(uart2));
	if (!device_is_ready(btm_uart_dev)) {
		LOG_ERR("UART device not ready");
		return -ENODEV;
	}

    ret = uart_callback_set(btm_uart_dev, btm_uart_cb, NULL);
    if (ret) {
        LOG_ERR("Failed to set BTM UART callback");
        return ret;
    }

    btm.id = ATM_BtmInit(BTM_DEV_ID);

    ret = zbus_chan_add_obs(&audio_evt_chan, &audio_evt_sub_btm, ZBUS_ADD_OBS_TIMEOUT_MS);
	if (ret) {
		LOG_ERR("Failed to add audio sub");
		return ret;
	}

    btm.init = true;

    return 0;
}