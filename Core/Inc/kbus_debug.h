#ifndef __KBUS_DEBUG_H__
#define __KBUS_DEBUG_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include "main.h"
#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include "kbus.h"

    void KBUS_Debug_Init(UART_HandleTypeDef *huart);
    void KBUS_Debug_PrintMsg(const KBUS_MessageTypeDef *msg);
    void KBUS_Debug_PrintRaw(const char *tag, const uint8_t *pData, uint8_t Size);
    void KBUS_Debug_Log(const char *fmt, ...);

#ifdef __cplusplus
}
#endif

#endif /* __KBUS_DEBUG_H__ */