#ifndef _AUDIO_COMMON_H_
#define _AUDIO_COMMON_H_

#include "main.h"

typedef enum
{
    AUDIO_STATE_RESET,
    AUDIO_STATE_INIT,
    AUDIO_STATE_POWERED,
    AUDIO_STATE_READY,
    AUDIO_STATE_STREAMING,
    AUDIO_STATE_STOP,
    AUDIO_STATE_PAUSE,
    AUDIO_STATE_ERROR,
} AudioState;

typedef enum
{
    SOURCE_MIC,
    SOURCE_STETH,
    SOURCE_USB,
} AudioSource;

typedef enum
{
    AUDIO_OUT_A2DP,
    AUDIO_OUT_BLE,
    AUDIO_OUT_USB,
} AudioOutput;

typedef enum
{
    FILTER_BALANCED,
    FILTER_HEART,
    FILTER_LUNG,
    FILTER_NONE,
} AudioFilter;

typedef struct
{
    uint8_t Volume;
    AudioFilter Filter;
    AudioOutput Out;
    AudioSource Source;
    bool mute;
    AudioState State;
} Audio_t;

typedef struct {
    int power;
    int id;
    bool init;
    Audio_t audio;
} Audio_Peripheral_t;

typedef enum 
{
    VOLUME_UP,
    VOLUME_DOWN,
    VOLUME_SET,
    VOLUME_MUTE,
    VOLUME_UNMUTE,
} VolumeEvent;

typedef struct {
    int type;
    int param;
} AudioEvent;

Audio_t *GetDspAudio(void);
Audio_t *GetBtmAudio(void);
Audio_t *GetSystemAudio(void);

#endif