#ifndef __RING_BUF_H__
#define __RING_BUF_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include "main.h"

#define RING_BUF_SIZE 256

    void RBUF_Push(const uint8_t *pData, uint16_t Size);
    int RBUF_Pop(uint8_t *pOut);

#ifdef __cplusplus
}
#endif

#endif /* __RING_BUF_H__ */