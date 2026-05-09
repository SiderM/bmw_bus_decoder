#include "can_debug.h"

static UART_HandleTypeDef *_uart;

typedef struct
{
    uint32_t Id;
    const char *Name;
} CAN_NameTypeDef;

static const CAN_NameTypeDef canNames[] = {
    {CAN_ID_DME1, "DME1"},
};

static const char *CAN_Debug_GetName(uint32_t Id)
{
    for (size_t i = 0; i < sizeof(canNames) / sizeof(canNames[0]); i++)
    {
        if (canNames[i].Id == Id)
        {
            return canNames[i].Name;
        }
    }
    return "????";
}

static void CAN_Debug_Write(const char *str)
{
    HAL_UART_Transmit(_uart, (uint8_t *)str, (uint16_t)strlen(str), 50);
}

void CAN_Debug_Init(UART_HandleTypeDef *huart)
{
    _uart = huart;
    CAN_Debug_Write("\r\n=== CAN Monitor started ===\r\n");
}

void CAN_Debug_Log(const char *fmt, ...)
{
    char buf[128];
    __va_list args;
    va_start(args, fmt);
    vsnprintf(buf, sizeof(buf), fmt, args);
    va_end(args);
    CAN_Debug_Write(buf);
}

void CAN_Debug_PrintFrame(CAN_RxHeaderTypeDef *Header, uint8_t *pData, uint16_t Size)
{
    char buf[128];

    snprintf(buf, sizeof(buf), "[%s(%03lX)]", CAN_Debug_GetName(Header->StdId), Header->StdId);
    CAN_Debug_Write(buf);

    CAN_Debug_Write("\r\n  DATA:");
    for (uint8_t i = 0; i < Size; i++)
    {
        snprintf(buf, sizeof(buf), " %02X", pData[i]);
        CAN_Debug_Write(buf);
    }
    CAN_Debug_Write("\r\n");

    if (Header->StdId == CAN_ID_DME1 && Size >= 2)
    {
        float rpm_raw = ((uint16_t)pData[3] << 8) | pData[2];
        uint16_t rpm = (uint16_t)rpm_raw / 6.4f;
        snprintf(buf, sizeof(buf), "  >> RPM: %d\r\n", rpm);
        CAN_Debug_Write(buf);
    }

    CAN_Debug_Write("\r\n");
}