#ifndef DSP_AUDIO_H
#define DSP_AUDIO_H

#include <zephyr/device.h>
#include <stdbool.h>
#include "audio_common.h"

// Accessor functions
Audio_t *GetDspAudio(void);
int GetDspPower(void);
int GetDspId(void);
const struct device *GetDspUartDev(void);
bool IsDspInit(void);

// Public functions
int dsp_init(void);
void dsp_audio_thread(void *a, void *b, void *c);

// Internal handlers (if needed outside)
static void dsp_audio_init(Audio_t *audio);
static void dsp_audio_stop(Audio_t *audio);
static void dsp_handle_filter_change(Audio_t *audio);
static void dsp_handle_source_change(Audio_t *audio);
static void dsp_handle_volume_change(Audio_t *audio);
static void dsp_handle_message(int param);

#endif // DSP_AUDIO_H
