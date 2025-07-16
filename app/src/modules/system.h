#ifndef _SYSTEM_H_
#define _SYSTEM_H_

#include <stdint.h>
#include "audio_common.h"
#include "at_btm.h"

typedef enum
{
    BT_DISCONNECTED,
    BT_DISCOVERABLE,
    BT_CONNECTING,
    BT_CONNECTED,
    BT_DISCONNECTING
} BTState;

typedef struct
{
    int state;
    int percent;
    int voltage;
    int charge;
} Battery_t;

typedef struct
{
    esp_bd_addr_t lastDevice;
    BTState state;
} BT_t;

typedef struct
{
    esp_bd_addr_t lastDevice;
    BTState state;
} Ble_t;

// System states
typedef enum {
    SYSTEM_STATE_INIT,
    SYSTEM_STATE_IDLE,
    SYSTEM_STATE_ACTIVE,
    SYSTEM_STATE_ERROR,
} SystemState_t;

typedef struct
{
    Battery_t battery;
    int state;
    int LBrightness;
    int RBrightness;
    BT_t a2dp;
    Ble_t ble;
    Audio_t audio;
} System_t;

// Accessor functions
System_t *GetSystem(void);
Audio_t *GetSystemAudio(void);

// Internal functions 
static void system_handle_button_event(struct button_msg *msg);
static void system_handle_device_event(struct device_msg *msg);
static void system_handle_audio_event(struct audio_msg *msg);

// Public functions
int system_init(void);
void pwm_control(int on);


#endif // _SYSTEM_H_