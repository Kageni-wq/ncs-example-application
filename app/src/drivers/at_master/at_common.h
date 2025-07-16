#ifndef _AT_COMMON_H_
#define _AT_COMMON_H_

#include <stdint.h>
#include <stddef.h>

#define MSG_START_CHAR '+'
#define MSG_PARAM_SEPERATOR '='
#define PARAM_SEPERATOR ','
#define PARAM_END_CHAR '\r'

typedef struct
{
    const char *command; // Command string (e.g., "AT+NAME")  
    uint8_t index;       // Unique identifier for the command
} command_pattern_t;

typedef struct {
    const char *pattern;  // Event string pattern
    uint8_t index;        // Unique event identifier
    void (*handler)(char param[CONFIG_MAX_PARAMS_PER_EVENT][CONFIG_MAX_PARAM_LENGTH]);
} event_pattern_t;

typedef struct {
    uint8_t id;
    command_pattern_t *cmdTable;
    event_pattern_t *evtTable;
} at_device_t;

typedef struct {
    char *buffer;
    uint8_t len;
    uint8_t device_id;
} Event_Data_t;

typedef struct {
    char event[CONFIG_MAX_EVENT_LENGTH];          // Event name
    char param[CONFIG_MAX_PARAMS_PER_EVENT][CONFIG_MAX_PARAM_LENGTH]; // Parameters as strings
    int num_params;                        // Number of parameters
    uint8_t device_id;                    // Device ID associated with the event
} Event_t;

typedef struct 
{
    uint8_t device;
    uint8_t index;
    uint16_t param[CONFIG_MAX_PARAMS_PER_EVENT];
    char *c_param;
    uint8_t num_params;
} Command_t;

typedef enum {
    WAITING_FOR_START_CHAR,
    START_CHAR_RECEIVED,
    EVENT_CHAR_RECEIVED,
    EVENT_SENT
} EVENT_CODE;

int AT_RegisterDevice(at_device_t *device);
int AT_CreateCommand(Command_t *command);
int AT_SendCommand(uint8_t dev_id, char *command, uint16_t len);
int AT_SendAction(uint8_t dev_id, uint8_t cmd_idx);
int AT_SendCmdWith1Param(uint8_t dev_id, uint8_t cmd_idx, uint16_t param);
int AT_SendCmdWith2Params(uint8_t dev_id, uint8_t cmd_idx, uint16_t param1, uint16_t param2);
int AT_SendCmdWith3Params(uint8_t dev_id, uint8_t cmd_idx, uint16_t param1, uint16_t param2, uint16_t param3);
int AT_SendCmdWithStrParam(uint8_t dev_id, uint8_t cmd_idx, char *param);
int AT_ParseEvent(Event_Data_t *event_data);
int AT_DispatchEvent(Event_t *event);

#endif