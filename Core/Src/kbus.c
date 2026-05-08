#include "kbus.h"

static UART_HandleTypeDef *_huart;

uint8_t kbusRxBuffer[KBUS_RX_BUFFER_SIZE];

static KBUS_ParserStateTypeDef parserState = KBUS_PARSET_STATE_SRC;
static KBUS_MessageTypeDef kbusMessage;
static uint8_t parserCount = 0;

static volatile uint8_t uartTxBusy = 0;

static uint8_t KBUS_CalculateChecksum(uint8_t src, uint8_t len, uint8_t dst, const uint8_t *data, uint8_t data_len)
{
    uint8_t crc = src ^ len ^ dst;
    for (uint8_t i = 0; i < data_len; i++)
        crc ^= data[i];
    return crc;
}

static void KBUS_ResetParser(void)
{
    parserState = KBUS_PARSET_STATE_SRC;
    parserCount = 0;
    memset(&kbusMessage, 0, sizeof(kbusMessage));
}

static void KBUS_ParseByte(uint8_t b)
{
    switch (parserState)
    {

    case KBUS_PARSET_STATE_SRC:
        kbusMessage.Src = b;
        parserState = KBUS_PARSET_STATE_LEN;
        break;

    case KBUS_PARSET_STATE_LEN:
        if (b < 2 || b > (KBUS_MAX_DATA_LEN + 2))
        {
            // Невалидная длина — пробуем текущий байт как SRC
            KBUS_ResetParser();
            kbusMessage.Src = b;
            parserState = KBUS_PARSET_STATE_LEN;
            break;
        }
        kbusMessage.Len = b;
        kbusMessage.Size = b - 2; // len включает dst + checksum
        parserState = KBUS_PARSET_STATE_DST;
        break;

    case KBUS_PARSET_STATE_DST:
        kbusMessage.Dst = b;
        parserCount = 0;
        parserState = (kbusMessage.Size > 0) ? KBUS_PARSET_STATE_DATA : KBUS_PARSET_STATE_CHK;
        break;

    case KBUS_PARSET_STATE_DATA:
        if (parserCount < kbusMessage.Size)
        {
            kbusMessage.Data[parserCount++] = b;
        }
        if (parserCount >= kbusMessage.Size)
        {
            parserState = KBUS_PARSET_STATE_CHK;
        }
        break;

    case KBUS_PARSET_STATE_CHK:
    {
        kbusMessage.Checksum = b;
        uint8_t expected = KBUS_CalculateChecksum(kbusMessage.Src, kbusMessage.Len, kbusMessage.Dst, kbusMessage.Data, kbusMessage.Size);
        kbusMessage.Valid = (b == expected);

        if (kbusMessage.Valid)
        {
            KBUS_RxMsgCallback(&kbusMessage);
        }
        KBUS_ResetParser();
        break;
    }
    }
}

HAL_StatusTypeDef KBUS_Init(UART_HandleTypeDef *huart)
{
    _huart = huart;
    KBUS_ResetParser();
    HAL_UARTEx_ReceiveToIdle_IT(_huart, kbusRxBuffer, KBUS_RX_BUFFER_SIZE);
    return HAL_OK;
}

void KBUS_Process(void)
{
    uint8_t b;
    while (RBUF_Pop(&b))
    {
        KBUS_ParseByte(b);
    }
}

void KBUS_ParseRxMsg(uint16_t Size)
{
    RBUF_Push(kbusRxBuffer, Size);
    HAL_UARTEx_ReceiveToIdle_IT(_huart, kbusRxBuffer, KBUS_RX_BUFFER_SIZE);
}

HAL_StatusTypeDef KBUS_SendMsg(uint8_t Src, uint8_t Dst, const uint8_t *pData, uint8_t Size)
{
    if (Size > KBUS_MAX_DATA_LEN)
        return HAL_ERROR;
    if (uartTxBusy)
        return HAL_BUSY;

    uint8_t buf[KBUS_MAX_DATA_LEN + 4];

    buf[0] = Src;
    buf[1] = Size + 2; // dst + (data) + checksum
    buf[2] = Dst;
    if (Size > 0)
    {
        memcpy(&buf[3], pData, Size);
    }
    buf[3 + Size] = KBUS_CalculateChecksum(Src, Size + 2, Dst, pData, Size);

    // KBUS требует паузу перед отправкой (~10 мс)
    // чтобы не создавать коллизию с другими устройствами
    HAL_Delay(10);

    uartTxBusy = 1;
    HAL_StatusTypeDef status = HAL_UART_Transmit(_huart, buf, 4 + Size, 50);
    uartTxBusy = 0;

    return status;
}

__weak void KBUS_RxMsgCallback(KBUS_MessageTypeDef *msg)
{
    // Пользовательская реализация должна переопределить эту функцию
    UNUSED(msg);
}
