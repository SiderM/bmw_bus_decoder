#include "ring_buf.h"

static uint8_t ringBuf[RING_BUF_SIZE];
static volatile uint32_t ringHead = 0;
static volatile uint32_t ringTail = 0;

void RBUF_Push(const uint8_t *pData, uint16_t Size)
{
    for (uint16_t i = 0; i < Size; i++)
    {
        uint32_t next = (ringHead + 1) % RING_BUF_SIZE;
        if (next != ringTail)
        {
            ringBuf[ringHead] = pData[i];
            ringHead = next;
        }
    }
}

int RBUF_Pop(uint8_t *pOut)
{
    if (ringTail == ringHead)
        return 0;
    *pOut = ringBuf[ringTail];
    ringTail = (ringTail + 1) % RING_BUF_SIZE;
    return 1;
}
