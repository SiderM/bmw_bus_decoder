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

#define KBUS_DEV_ZKE 0x00
#define KBUS_DEV_MFL 0x50
#define KBUS_DEV_IKE 0x80
#define KBUS_DEV_LCM 0xD0
#define KBUS_DEV_BROADCAST 0xBF

#define KBUS_CMD_PING 0x01
#define KBUS_CMD_PONG 0x02
#define KBUS_CMD_IGNITION_REQUEST 0x10
#define KBUS_CMD_IGNITION 0x11
#define KBUS_CMD_CLUSTER_INDICATORS 0x5B
#define KBUS_CMD_INSTRUMENT_BACKLIGHTING 0x5C

    void KBUS_Debug_Init(UART_HandleTypeDef *huart);
    void KBUS_Debug_Log(const char *fmt, ...);
    void KBUS_Debug_PrintMsg(const KBUS_MessageTypeDef *Msg);

#ifdef __cplusplus
}
#endif

#endif /* __KBUS_DEBUG_H__ */