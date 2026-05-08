#ifndef __KBUS_H__
#define __KBUS_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include "main.h"
#include <string.h>
#include "ring_buf.h"

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

#define KBUS_RX_BUFFER_SIZE 64
#define KBUS_MAX_DATA_LEN 32

    typedef enum
    {
        KBUS_PARSET_STATE_SRC,
        KBUS_PARSET_STATE_LEN,
        KBUS_PARSET_STATE_DST,
        KBUS_PARSET_STATE_DATA,
        KBUS_PARSET_STATE_CHK,
    } KBUS_ParserStateTypeDef;

    typedef struct
    {
        uint8_t Src;
        uint8_t Len;
        uint8_t Dst;
        uint8_t Data[KBUS_RX_BUFFER_SIZE];
        uint8_t Size;
        uint8_t Checksum;
        uint8_t Valid;
    } KBUS_MessageTypeDef;

    HAL_StatusTypeDef KBUS_Init(UART_HandleTypeDef *huart);
    void KBUS_Process(void);
    void KBUS_ParseRxMsg(uint16_t Size);
    HAL_StatusTypeDef KBUS_SendMsg(uint8_t Src, uint8_t Dst, const uint8_t *pData, uint8_t Size);

    void KBUS_RxMsgCallback(KBUS_MessageTypeDef *msg);

#ifdef __cplusplus
}
#endif

#endif /* __KBUS_H__ */