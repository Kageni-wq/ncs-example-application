#ifndef __BTM_AUDIO_H
#define __BTM_AUDIO_H

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/uart.h>
#include "audio_common.h"

/* Function declarations */
Audio_t *GetBtmAudio(void);
int GetBtmPower(void);
int GetBtmId(void);
const struct device *GetBtmUartDev(void);
bool IsBtmInit(void);
void btm_audio_thread(void *a, void *b, void *c);
int btm_init(void);

/* Internal functions that should be static in the .c file */
static void btm_uart_cb(const struct device *dev, struct uart_event *evt, void *user_data);
static void btm_audio_init(Audio_t *audio);
static void btm_audio_start(Audio_t *audio);
static void btm_audio_stop(Audio_t *audio);
static void btm_handle_filter_change(Audio_t *audio);
static void btm_handle_output_change(Audio_t *audio);
static void btm_handle_volume_change(Audio_t *audio);
static void btm_create_event(int event_type, int param);
static void btm_handle_message(int param);

#endif /* __BTM_AUDIO_H */