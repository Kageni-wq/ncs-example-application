#include "at_btm.h"
#include "at_common.h"
#include "audio_common.h"

static at_device_t device;
static Command_t cmd;

const command_pattern_t btm_Command_table[] = {
    {NULL, 0}, // Start of table marker
    {"OK", CMD_BTM_ACK},
    {"AT+START", CMD_BTM_START},
    {"AT+VLUP", CMD_BTM_INCREASE_VOLUME},
    {"AT+VLDN", CMD_BTM_DECREASE_VOLUME},
    {"AT+MODE", CMD_BTM_SET_MODE},
    {"AT+PAIR", CMD_BTM_PAIR},
    {"AT+UNPAIR", CMD_BTM_UNPAIR},
    {"AT+PWRON", CMD_BTM_POWER_ON},
    {"AT+PWROFF", CMD_BTM_POWER_OFF},
    {"AT+STAT", CMD_BTM_STATUS_REQUEST},
    {"AT+CONN", CMD_BTM_CONNECT_TO_DEVICE},
    {"AT+FRGT", CMD_BTM_FORGET_DEVICE},
    {"AT+NAME", CMD_BTM_SET_DEVICE_NAME},
    {"AT+MUTE", CMD_BTM_MUTE},
    {"AT+UNMUTE", CMD_BTM_UNMUTE},
    {"AT+FLTR", CMD_BTM_SET_FILTER},
    {"AT+VOL", CMD_BTM_SET_VOLUME},
    {"AT+BAT", CMD_BTM_BATTERY_LEVEL},
    {"AT+RST", CMD_BTM_RESET},
    {"AT+DSCVR", CMD_BTM_DISCOVERABLE},
    {NULL, 0} // End of table marker
};

const event_pattern_t btm_event_table[] = {
    {"EV+OK", EV_BTM_ACK, BTM_HandleAckEvent},
    {"EV+RDY", EV_BTM_READY, BTM_HandleReadyEvent},
    {"EV+CON", EV_BTM_CONN, BTM_HandleConnectedEvent},
    {"EV+DSC", EV_BTM_DISCONN, BTM_HandleDisconnectedEvent},
    {"EV+VOL", EV_BTM_VOLUME, BTM_HandleVolumeEvent},
    {"EV+ON", EV_BTM_POWER_ON, BTM_HandlePowerOnEvent},
    {"EV+OFF", EV_BTM_POWER_OFF, BTM_HandlePowerOffEvent},
    {"EV+PAIR", EV_BTM_PAIRED, BTM_HandlePairedEvent},
    {"EV+SCAN", EV_BTM_SCAN, BTM_HandleScanEvent},
    {"EV+FLTR", EV_BTM_FILTER, BTM_HandleFilterEvent},
    {"EV+AUD", EV_BTM_AUDIO, BTM_HandleAudioEvent},
    {"EV+BOOT", EV_BTM_BOOT, BTM_HandleBootEvent},
    {"EV+ERROR", EV_BTM_UNKNOWN, BTM_HandleUnknownEvent},
    {NULL, 0, NULL} // End of table marker
};

int ATM_BtmInit(int id)
{
    device.id = id;
    device.cmdTable = btm_Command_table;
    device.evtTable = btm_event_table;
    return AT_RegisterDevice(&device);
}

/*************************************************************************************************************
***************************************Command Functions******************************************************
**************************************************************************************************************/

void BTM_SendConnectCommand(esp_bd_addr_t *addr)
{
    
}
/*************************************************************************************************************
***************************************Event Functions******************************************************
**************************************************************************************************************/

void BTM_HandleAckEvent(char param[CONFIG_MAX_PARAMS_PER_EVENT][CONFIG_MAX_PARAM_LENGTH])
{
    (void)param;
    // Implementation
}

void BTM_HandleReadyEvent(char param[CONFIG_MAX_PARAMS_PER_EVENT][CONFIG_MAX_PARAM_LENGTH])
{
    (void)param;
    // Implementation
}

void BTM_HandleConnectedEvent(char param[CONFIG_MAX_PARAMS_PER_EVENT][CONFIG_MAX_PARAM_LENGTH])
{
    (void)param;
    // Implementation
}

void BTM_HandleDisconnectedEvent(char param[CONFIG_MAX_PARAMS_PER_EVENT][CONFIG_MAX_PARAM_LENGTH])
{
    (void)param;
    // Implementation
}

void BTM_HandleVolumeEvent(char param[CONFIG_MAX_PARAMS_PER_EVENT][CONFIG_MAX_PARAM_LENGTH])
{
    (void)param;
    // Implementation
}

void BTM_HandlePowerOnEvent(char param[CONFIG_MAX_PARAMS_PER_EVENT][CONFIG_MAX_PARAM_LENGTH])
{
    (void)param;
    // Implementation
}

void BTM_HandlePowerOffEvent(char param[CONFIG_MAX_PARAMS_PER_EVENT][CONFIG_MAX_PARAM_LENGTH])
{
    (void)param;
    // Implementation
}

void BTM_HandlePairedEvent(char param[CONFIG_MAX_PARAMS_PER_EVENT][CONFIG_MAX_PARAM_LENGTH])
{
    if (param != NULL)
    {
        // Implementation
    }
    else
    {
        // Error handling
    }
}

void BTM_HandleScanEvent(char param[CONFIG_MAX_PARAMS_PER_EVENT][CONFIG_MAX_PARAM_LENGTH])
{
    (void)param;
    // Implementation
}

void BTM_HandleFilterEvent(char param[CONFIG_MAX_PARAMS_PER_EVENT][CONFIG_MAX_PARAM_LENGTH])
{
    (void)param;
    // Implementation
}

void BTM_HandleAudioEvent(char param[CONFIG_MAX_PARAMS_PER_EVENT][CONFIG_MAX_PARAM_LENGTH])
{
    (void)param;
    // Implementation
}

void BTM_HandleUnknownEvent(char param[CONFIG_MAX_PARAMS_PER_EVENT][CONFIG_MAX_PARAM_LENGTH])
{
    (void)param;
    // Implementation
}

void BTM_HandleBootEvent(char param[CONFIG_MAX_PARAMS_PER_EVENT][CONFIG_MAX_PARAM_LENGTH])
{
    (void)param;
    // Implementation
}
