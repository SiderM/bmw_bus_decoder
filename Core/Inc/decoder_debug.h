#ifndef __DECODER_DEBUG_H__
#define __DECODER_DEBUG_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include "main.h"
#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include "decoder.h"

    void DECODER_Debug_Init(UART_HandleTypeDef *huart);
    void DECODER_Debug_PrintMsg(volatile Car_StateTypeDef *State);

#ifdef __cplusplus
}
#endif

#endif /* __DECODER_DEBUG_H__ */