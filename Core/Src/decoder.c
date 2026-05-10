#include "decoder.h"

volatile Car_StateTypeDef carState = {0};

void DECODER_CalculateRpm(uint8_t *pData)
{
    float rpm_raw = ((uint16_t)pData[3] << 8) | pData[2];
    carState.Rpm = (uint16_t)rpm_raw / 6.4f;
}

void DECODER_CalculateSpeed(uint8_t *pData)
{
    float speed_raw = ((uint16_t)pData[2] << 8 | pData[1]) >> 4;
    carState.Speed = (uint8_t)(speed_raw * 1.125f);
}

void DECODER_CalculateFuelLevel(uint8_t *pData)
{
    float fuel_raw;
    if (pData[2] >= 0x80)
    {
        fuel_raw = (float)(pData[2] - 0x80) / (float)(0x87 - 0x80) * 10.0f;
    }
    else
    {
        fuel_raw = 10.0f + (float)(pData[2] - 0x08) / (float)(0x39 - 0x08) * 90.0f;
    }

    carState.FuelLevel = (uint8_t)fuel_raw;
}

void DECODER_CalculateCoolantTemp(uint8_t *pData)
{
    carState.CoolantTemp = (int8_t)(0.75f * pData[1] - 48.37f);
}

void DECODER_CalculateOutsideTemp(uint8_t *pData)
{
    if (pData[3] > 0x80)
    {
        carState.OutsideTemp = -(int8_t)(pData[3] - 128);
    }
    else
    {
        carState.OutsideTemp = (int8_t)pData[3];
    }
}

void DECODER_CalculateGear(uint8_t *pData)
{
    carState.GearMode = pData[2];
    carState.Gear = pData[1];
}

void DECODER_CalculateSteeringAngle(uint8_t *pData)
{
    uint16_t word_a = (uint16_t)pData[0] | ((uint16_t)pData[1] << 8);
    uint16_t angle_raw = word_a & 0x7FFF;
    uint8_t angle_sign = (word_a >> 15) & 1;
    float angle_deg = (angle_sign ? -1.0f : 1.0f) * angle_raw * 0.045f;
    carState.SteeringAngle = (int16_t)angle_deg;
}

void DECODER_CalculateEngineIndicators(uint8_t *pData) {}
