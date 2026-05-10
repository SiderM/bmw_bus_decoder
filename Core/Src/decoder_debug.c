#include "decoder_debug.h"

static UART_HandleTypeDef *_uart;

static void DECODER_Debug_Write(const char *str)
{
    HAL_UART_Transmit(_uart, (uint8_t *)str, (uint16_t)strlen(str), 50);
}

void DECODER_Debug_Init(UART_HandleTypeDef *huart)
{
    _uart = huart;
    DECODER_Debug_Write("\r\n=== Decoder Monitor started ===\r\n");
}

void DECODER_Debug_PrintMsg(volatile Car_StateTypeDef *State)
{
    char buf[128];
    DECODER_Debug_Write("\r\n=== Decoder Monitor State ===\r\n");

    DECODER_Debug_Write(">>>> Engine <<<<\r\n");

    snprintf(buf, sizeof(buf), " > RPM: %d r/min\r\n", State->Rpm);
    DECODER_Debug_Write(buf);

    snprintf(buf, sizeof(buf), " > Speed: %d km/h\r\n", State->Speed);
    DECODER_Debug_Write(buf);

    snprintf(buf, sizeof(buf), " > Fuel Level: %d %%\r\n", State->FuelLevel);
    DECODER_Debug_Write(buf);

    snprintf(buf, sizeof(buf), " > Coolant Temp: %d °C\r\n", State->CoolantTemp);
    DECODER_Debug_Write(buf);

    DECODER_Debug_Write(">>>> Transmission <<<<\r\n");

    snprintf(buf, sizeof(buf), " > Gear: %d\r\n", State->Gear);
    DECODER_Debug_Write(buf);

    snprintf(buf, sizeof(buf), " > Selector: %d\r\n", State->Selector);
    DECODER_Debug_Write(buf);

    snprintf(buf, sizeof(buf), " > Gear Mode: %d\r\n", State->GearMode);
    DECODER_Debug_Write(buf);

    DECODER_Debug_Write(">>>> Body <<<<\r\n");

    snprintf(buf, sizeof(buf), " > Steering Angle: %d °\r\n", State->SteeringAngle);
    DECODER_Debug_Write(buf);

    snprintf(buf, sizeof(buf), " > Outside Temp: %d °C\r\n", State->OutsideTemp);
    DECODER_Debug_Write(buf);
}
