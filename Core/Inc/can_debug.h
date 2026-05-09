#ifndef __CAN_DEBUG_H__
#define __CAN_DEBUG_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include "main.h"
#include <string.h>
#include <stdarg.h>
#include <stdio.h>

#define CAN_ID_DME1 0x316

    void CAN_Debug_Init(UART_HandleTypeDef *huart);
    void CAN_Debug_Log(const char *fmt, ...);
    void CAN_Debug_PrintFrame(CAN_RxHeaderTypeDef *Header, uint8_t *pData, uint16_t Size);

#ifdef __cplusplus
}
#endif

#endif /* __CAN_DEBUG_H__ */