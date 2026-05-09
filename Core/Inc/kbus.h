#ifndef __KBUS_H__
#define __KBUS_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include "main.h"
#include <string.h>
#include "ring_buf.h"

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

    void KBUS_Init(UART_HandleTypeDef *huart);
    void KBUS_Process(void);
    void KBUS_Parse(uint16_t Size);
    HAL_StatusTypeDef KBUS_Send(uint8_t Src, uint8_t Dst, const uint8_t *pData, uint8_t Size);

    void KBUS_RxMsgCallback(KBUS_MessageTypeDef *Msg);

#ifdef __cplusplus
}
#endif

#endif /* __KBUS_H__ */