#ifndef __DECODER_H__
#define __DECODER_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include "main.h"

    typedef struct
    {
        uint16_t Rpm;
        uint8_t Speed;
        uint8_t FuelLevel;
        int8_t CoolantTemp;
        int8_t OutsideTemp;
        uint8_t Gear;
        uint8_t GearMode;
        int16_t SteeringAngle;
        uint16_t EngineIndicators;
        uint16_t TransmissionIndicators;
        uint16_t BodyIndicators;
    } Car_StateTypeDef;

    typedef struct __attribute__((packed))
    {
        uint8_t Header[2]; // 0xAA 0x55
        Car_StateTypeDef Data;
    } Car_DataFrameTypeDef;

    extern volatile Car_StateTypeDef carState;

    void DECODER_CalculateRpm(uint8_t *pData);
    void DECODER_CalculateSpeed(uint8_t *pData);
    void DECODER_CalculateFuelLevel(uint8_t *pData);
    void DECODER_CalculateCoolantTemp(uint8_t *pData);
    void DECODER_CalculateOutsideTemp(uint8_t *pData);
    void DECODER_CalculateGear(uint8_t *pData);
    void DECODER_CalculateSteeringAngle(uint8_t *pData);
    void DECODER_CalculateEngineIndicators(uint8_t *pData);

#ifdef __cplusplus
}
#endif

#endif /* __DECODER_H__ */