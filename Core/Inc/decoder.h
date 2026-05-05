#ifndef __DECODER_H__
#define __DECODER_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include "main.h"

    /* Определение структуры состояния автомобиля */
    typedef struct
    {
        uint16_t RPM;
        uint8_t Speed;
        uint8_t Fuel_Level;
        int8_t Coolant_Temp;
        int8_t Outside_Temp;
        uint8_t Gear;
        int16_t Steering_Angle;
        uint16_t Warnings;
    } CarState;

/* Определения для предупреждений */
#define WARN_CHECK_ENGINE (1 << 0)
#define WARN_EML (1 << 1)
#define WARN_OIL_LEVEL (1 << 2)
#define WARN_BATTERY (1 << 3)
#define WARN_CRUISE (1 << 4)

    /* Структура для хранения состояния автомобиля */
    extern volatile CarState car;

#ifdef __cplusplus
}
#endif

#endif /* __DECODER_H__ */