#include "at_common.h"
#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/logging/log.h>
#include <zephyr/drivers/uart.h>
#include "zbus_common.h"

#define MAX_DEVICES 3
#define CMD_BUFFER_SIZE 64

LOG_MODULE_REGISTER(at_master, CONFIG_ATM_AUDIO_LOG_LEVEL);

uint8_t num_devices = 0;
at_device_t devices[MAX_DEVICES];

int AT_RegisterDevice(at_device_t *device)
{
    if (device->id >= MAX_DEVICES)
    {
        return -1;
    }

    devices[device->id] = *device;
    return 0;
}

/*************************************************************************************************************
***************************************Command Functions******************************************************
**************************************************************************************************************/

int AT_CreateCommand(Command_t *command)
{
    char cmd_buffer[CMD_BUFFER_SIZE] = {0};
    char param_str[CONFIG_MAX_PARAM_LENGTH] = {0};
    uint16_t param_len = 0;
    uint16_t cmd_len = 0;
    uint8_t total_len = 0;
    command_pattern_t cmd_pattern = devices[command->device].cmdTable[command->index];

    cmd_len = strlen(cmd_pattern.command);

    // If parameters are present, append them
    if (command->num_params != 0)
    {
        param_str[0] = MSG_PARAM_SEPERATOR;
        param_len = 1;

        for (int i = 0; i < command->num_params; i++)
        {
            // Convert the parameter to a string
            param_len += snprintf(param_str + param_len, sizeof(param_str), "%d", command->param[i]);

            // Add a comma only if there are multiple parameters and this is not the last one
            if (command->num_params > 1 && i < command->num_params - 1)
            {
                param_str[param_len] = PARAM_SEPERATOR;
                param_len++;
            }
        }
        total_len += param_len;
    }

    // If character parameters are present, append them
    if (command->c_param != NULL)
    {
        param_str[0] = MSG_PARAM_SEPERATOR;
        param_len = 1;

        param_len += snprintf(param_str + param_len, sizeof(param_str), "%s", command->c_param);
        total_len += param_len;
    }

    memcpy(cmd_buffer, cmd_pattern.command, cmd_len);

    for (uint_fast8_t i = 0; i < param_len; i++)
    {
        cmd_buffer[cmd_len] = param_str[i];
        cmd_len++;
    }

    cmd_buffer[cmd_len] = '\r';
    cmd_len++;
    cmd_buffer[cmd_len] = '\n';
    cmd_len++;

    return AT_SendCommand(command->device, cmd_buffer, cmd_len);
}

int AT_SendCommand(uint8_t dev_id, char *command, uint16_t len)
{
    const struct device *dev;
    int err;

    if (dev_id == BTM_DEV_ID)
    {
        dev = GetBtmUartDev();
    }
    else if (dev_id == DSP_DEV_ID)
    {
        dev = GetDspUartDev();
    }
    else
    {
        return -1;
    }

    err = uart_tx(dev, (uint8_t *)command, len, SYS_FOREVER_MS);
    if (err) {
        LOG_ERR("Failed to send command: %d", err);
        return err;
    }

    return 0;
}

// Command Helpers
int AT_SendAction(uint8_t dev_id, uint8_t cmd_idx)
{
    Command_t cmd = {
        .device = dev_id,
        .index = cmd_idx,
        .param = {0},
        .c_param = NULL,
        .num_params = 0
    };

    return AT_CreateCommand(&cmd);
}

int AT_SendCmdWith1Param(uint8_t dev_id, uint8_t cmd_idx, uint16_t param)
{
    Command_t cmd = {
        .device = dev_id,
        .index = cmd_idx,
        .param = {param},
        .c_param = NULL,
        .num_params = 1
    };

    return AT_CreateCommand(&cmd);
}

int AT_SendCmdWith2Params(uint8_t dev_id, uint8_t cmd_idx, uint16_t param1, uint16_t param2)
{
    Command_t cmd = {
        .device = dev_id,
        .index = cmd_idx,
        .param = {param1, param2},
        .c_param = NULL,
        .num_params = 2
    };

    return AT_CreateCommand(&cmd);
}

int AT_SendCmdWith3Params(uint8_t dev_id, uint8_t cmd_idx, uint16_t param1, uint16_t param2, uint16_t param3)
{
    Command_t cmd = {
        .device = dev_id,
        .index = cmd_idx,
        .param = {param1, param2, param3},
        .c_param = NULL,
        .num_params = 3
    };

    return AT_CreateCommand(&cmd);
}

int AT_SendCmdWithStrParam(uint8_t dev_id, uint8_t cmd_idx, char *param)
{
    Command_t cmd = {
        .device = dev_id,
        .index = cmd_idx,
        .c_param = param,
        .num_params = 0
    };

    return AT_CreateCommand(&cmd);
}


/*************************************************************************************************************
***************************************Event Functions******************************************************
**************************************************************************************************************/

int AT_ParseEvent(Event_Data_t *event_data)
{
    Event_t currentEvent = {0};
    uint8_t evt_idx = 0;
    uint8_t char_idx = 0;
    uint8_t param_idx = 0;
    int state = WAITING_FOR_START_CHAR;
    currentEvent.device_id = event_data->device_id;
    // Parse event
    for (int i = 0; i < event_data->len; i++)
    {
        char c = &event_data->buffer[evt_idx++];
        if (state == WAITING_FOR_START_CHAR)
        {
            if (c == MSG_START_CHAR)
            {
                state = START_CHAR_RECEIVED;
                currentEvent.event[char_idx] = c;
                char_idx++;
            }
        }
        else if (state == START_CHAR_RECEIVED)
        {
            if (c == MSG_PARAM_SEPERATOR)
            {
                state = EVENT_CHAR_RECEIVED;
                char_idx = 0;
                currentEvent.num_params = 1;
            }
            else if ((c == PARAM_END_CHAR) | (c == '\n'))
            {
                state = 0;
                return AT_DispatchEvent(&currentEvent);
            }
            else
            {
                currentEvent.event[char_idx] = c;
                char_idx++;
            }
        }
        else if (state == EVENT_CHAR_RECEIVED)
        {
            if (c == PARAM_SEPERATOR)
            {
                param_idx++;
                currentEvent.num_params++;
                char_idx = 0;
            }
            else if ((c == PARAM_END_CHAR) | (c == '\n'))
            {
                state = WAITING_FOR_START_CHAR;
                return AT_DispatchEvent(&currentEvent);
            }
            else
            {
                currentEvent.param[param_idx][char_idx] = c;
                char_idx++;
            }
        }
    }
    return -1; // No event found
}

int AT_DispatchEvent(Event_t *event)
{
    if (!event || !event->event[0])
    {
        return -1; // Invalid input
    }

    for (int i = 0; devices[event->device_id].evtTable[i].pattern != NULL; i++)
    {
        if (strcmp(event->event, devices[event->device_id].evtTable[i].pattern) == 0)
        {
            if (devices[event->device_id].evtTable[i].handler != NULL)
            {
                devices[event->device_id].evtTable[i].handler((char (*)[CONFIG_MAX_PARAM_LENGTH])event->param);
                return 0; // Success
            }
        }
    }

    return -1; // Event not found
}