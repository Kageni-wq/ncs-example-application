#ifndef _ZBUS_COMMON_H_
#define _ZBUS_COMMON_H_

#define ZBUS_READ_TIMEOUT_MS	K_MSEC(100)
#define ZBUS_ADD_OBS_TIMEOUT_MS K_MSEC(200)

#define POWER_ON 1
#define POWER_OFF 0	

/***** Messages for zbus ******/

enum button_action {
	BUTTON_PRESS = 1,
};

struct button_msg {
	uint32_t button_pin;
	enum button_action button_action;
};

struct device_msg {
    int event;
	int device;
    int param;
};

struct power_msg {
	int event;
	int device;
	int param;
};

struct audio_msg {
	int type;
	int param;
};

enum device_evt_type
{
    DEVICE_POWER,
    DEVICE_VOLUME,
    DEVICE_EVENT_BT_STATE,
    DEVICE_BT_STATE,
    DEVICE_AUDIO_MODE,
    DEVICE_AUDIO_SOURCE,
    DEVICE_AUDIO_OUTPUT,
    DEVICE_AUDIO_FILTER,
    DEVICE_AUDIO_MUTE,
};

enum device_id
{
    BTM_DEV_ID,
    DSP_DEV_ID,
	SYS_DEV_ID,
	RGB_DEV_ID,
	L_LED_DEV_ID,
	R_LED_DEV_ID,
};	

enum audio_evt_type
{
    AUDIO_START,
    AUDIO_STOP,
    AUDIO_PAUSE,
    AUDIO_RESUME,
    AUDIO_FILTER_CHANGE,
    AUDIO_SOURCE_CHANGE,
    AUDIO_OUTPUT_CHANGE,
    AUDIO_VOLUME_CHANGE,
    AUDIO_DSP_MSG,
    AUDIO_BTM_MSG,
};

enum le_audio_evt_type {
	LE_AUDIO_EVT_CONFIG_RECEIVED = 1,
	LE_AUDIO_EVT_PRES_DELAY_SET,
	LE_AUDIO_EVT_STREAMING,
	LE_AUDIO_EVT_NOT_STREAMING,
	LE_AUDIO_EVT_STREAM_SENT,
	LE_AUDIO_EVT_SYNC_LOST,
	LE_AUDIO_EVT_NO_VALID_CFG,
	LE_AUDIO_EVT_COORD_SET_DISCOVERED,
};

#endif /* _ZBUS_COMMON_H_ */