#include "kbus_debug.h"

static UART_HandleTypeDef *_uart;

typedef struct
{
    uint8_t Id;
    const char *Name;
} KBUS_DeviceTypeDef;

typedef struct
{
    uint8_t Src;
    uint8_t Dst;
    uint8_t Cmd;
    const char *Desc;
} KBUS_CommandTypeDef;

static const KBUS_DeviceTypeDef kbusNames[] = {
    {KBUS_DEV_ZKE, "ZKE"},
    {KBUS_DEV_MFL, "MFL"},
    {KBUS_DEV_IKE, "IKE"},
    {KBUS_DEV_LCM, "LCM"},
    {KBUS_DEV_BROADCAST, "BCAST"},
};

static const KBUS_CommandTypeDef kbusCommands[] = {
    {KBUS_DEV_ZKE, KBUS_DEV_BROADCAST, KBUS_CMD_PING, "PING"},
    {KBUS_DEV_BROADCAST, KBUS_DEV_BROADCAST, KBUS_CMD_PONG, "PONG"},
    {KBUS_DEV_LCM, KBUS_DEV_IKE, KBUS_CMD_IGNITION_REQUEST, "Ignition Status Request"},
    {KBUS_DEV_IKE, KBUS_DEV_BROADCAST, KBUS_CMD_IGNITION, "Ignition Status"},
    {KBUS_DEV_LCM, KBUS_DEV_BROADCAST, KBUS_CMD_CLUSTER_INDICATORS, "Cluster Indicators"},
    {KBUS_DEV_LCM, KBUS_DEV_BROADCAST, KBUS_CMD_INSTRUMENT_BACKLIGHTING, "Instrument Backlighting"},
};

static const char *KBUS_Debug_GetDeviceName(uint8_t id)
{
    for (size_t i = 0; i < sizeof(kbusNames) / sizeof(kbusNames[0]); i++)
    {
        if (kbusNames[i].Id == id)
        {
            return kbusNames[i].Name;
        }
    }
    return "????";
}

static const char *KBUS_Debug_GetCommandDesc(uint8_t src, uint8_t dst, uint8_t cmd)
{
    for (size_t i = 0; i < sizeof(kbusCommands) / sizeof(kbusCommands[0]); i++)
    {
        uint8_t src_match = (kbusCommands[i].Src == KBUS_DEV_ZKE) || (kbusCommands[i].Src == src);
        uint8_t dst_match = (kbusCommands[i].Dst == KBUS_DEV_ZKE) || (kbusCommands[i].Dst == dst);
        if (src_match && dst_match && kbusCommands[i].Cmd == cmd)
        {
            return kbusCommands[i].Desc;
        }
    }
    return NULL;
}

static void KBUS_Debug_Write(const char *str)
{
    HAL_UART_Transmit(_uart, (uint8_t *)str, (uint16_t)strlen(str), 50);
}

void KBUS_Debug_Init(UART_HandleTypeDef *huart)
{
    _uart = huart;
    KBUS_Debug_Write("\r\n=== KBUS Monitor started ===\r\n");
}

void KBUS_Debug_Log(const char *fmt, ...)
{
    char buf[128];
    __va_list args;
    va_start(args, fmt);
    vsnprintf(buf, sizeof(buf), fmt, args);
    va_end(args);
    KBUS_Debug_Write(buf);
}

void KBUS_Debug_PrintRaw(const char *tag, const uint8_t *pData, uint8_t Size)
{
    char buf[16];
    KBUS_Debug_Write(tag);
    for (uint8_t i = 0; i < Size; i++)
    {
        snprintf(buf, sizeof(buf), " %02X", pData[i]);
        KBUS_Debug_Write(buf);
    }
    KBUS_Debug_Write("\r\n");
}

void KBUS_Debug_PrintMsg(const KBUS_MessageTypeDef *msg)
{
    char buf[128];
    const char *known_cmd = (msg->Size > 0) ? KBUS_Debug_GetCommandDesc(msg->Src, msg->Dst, msg->Data[0]) : NULL;

    snprintf(buf, sizeof(buf), "[%s(%02X) >> %s(%02X)] len=%d", KBUS_Debug_GetDeviceName(msg->Src), msg->Src, KBUS_Debug_GetDeviceName(msg->Dst), msg->Dst, msg->Size);
    KBUS_Debug_Write(buf);

    if (known_cmd)
    {
        snprintf(buf, sizeof(buf), "  <%s>", known_cmd);
        KBUS_Debug_Write(buf);
    }

    KBUS_Debug_Write("\r\n  DATA:");
    for (uint8_t i = 0; i < msg->Size; i++)
    {
        snprintf(buf, sizeof(buf), " %02X", msg->Data[i]);
        KBUS_Debug_Write(buf);
    }

    snprintf(buf, sizeof(buf), "  CRC=%02X [%s]\r\n", msg->Checksum, msg->Valid ? "OK" : "FAIL");
    KBUS_Debug_Write(buf);

    if (msg->Src == KBUS_DEV_LCM && msg->Data[0] == KBUS_CMD_INSTRUMENT_BACKLIGHTING && msg->Size >= 2)
    {
        uint8_t blk = (msg->Data[1] * 100) / 0xFE;
        snprintf(buf, sizeof(buf), "  >> BackLight: %d%%\r\n", blk);
        KBUS_Debug_Write(buf);
    }

    KBUS_Debug_Write("\r\n");
}
