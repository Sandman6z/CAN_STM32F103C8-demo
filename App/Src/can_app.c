#include "can_app.h"
#include "uart_log.h"

static CAN_TxHeaderTypeDef tx_header;
static CAN_RxHeaderTypeDef rx_header;
static uint8_t tx_data[8];
static uint8_t rx_data[8];
static uint32_t tx_mailbox;
static uint8_t tx_counter = 0;

/**
 * @brief 配置 CAN 过滤器，接收所有帧
 */
static void can_filter_init(void)
{
    CAN_FilterTypeDef filter = {0};
    filter.FilterBank           = 0;
    filter.FilterMode           = CAN_FILTERMODE_IDMASK;
    filter.FilterScale          = CAN_FILTERSCALE_32BIT;
    filter.FilterIdHigh         = 0x0000;
    filter.FilterIdLow          = 0x0000;
    filter.FilterMaskIdHigh     = 0x0000;   // Mask 全0 = 接收所有 ID
    filter.FilterMaskIdLow      = 0x0000;
    filter.FilterFIFOAssignment = CAN_RX_FIFO0;
    filter.FilterActivation     = ENABLE;
    HAL_CAN_ConfigFilter(&hcan, &filter);
}

/**
 * @brief 初始化 CAN 应用层
 */
void can_app_init(void)
{
    can_filter_init();
    HAL_CAN_Start(&hcan);
    
    // 启用 CAN RX 中断
    HAL_CAN_ActivateNotification(&hcan, CAN_IT_RX_FIFO0_MSG_PENDING);

    uart_log_print("CAN Initialized\r\n");
    uart_log_print("BoardID = 0x%03X\r\n", BOARD_ID);
    uart_log_print("PeerID  = 0x%03X\r\n\r\n", PEER_ID);
}

/**
 * @brief 发送一帧 CAN 数据
 */
void can_app_send(void)
{
    tx_header.StdId              = BOARD_ID;
    tx_header.ExtId              = 0x00;
    tx_header.RTR                = CAN_RTR_DATA;
    tx_header.IDE                = CAN_ID_STD;
    tx_header.DLC                = 8;
    tx_header.TransmitGlobalTime = DISABLE;

    // 第一字节为递增计数器，其余固定
    tx_data[0] = tx_counter++;
    tx_data[1] = 0x02;
    tx_data[2] = 0x03;
    tx_data[3] = 0x04;
    tx_data[4] = 0x05;
    tx_data[5] = 0x06;
    tx_data[6] = 0x07;
    tx_data[7] = 0x08;

    if (HAL_CAN_AddTxMessage(&hcan, &tx_header, tx_data, &tx_mailbox) == HAL_OK)
    {
        uart_log_print("[TX] ID=0x%03X  Data: %02X %02X %02X %02X %02X %02X %02X %02X\r\n",
                       (unsigned int)tx_header.StdId,
                       tx_data[0], tx_data[1], tx_data[2], tx_data[3],
                       tx_data[4], tx_data[5], tx_data[6], tx_data[7]);
    }
    else
    {
        uart_log_print("[TX] ERROR - Send failed, check bus connection\r\n");
    }
}

/**
 * @brief 轮询接收 FIFO 并打印
 */
void can_app_receive(void)
{
    while (HAL_CAN_GetRxFifoFillLevel(&hcan, CAN_RX_FIFO0) > 0)
    {
        if (HAL_CAN_GetRxMessage(&hcan, CAN_RX_FIFO0, &rx_header, rx_data) == HAL_OK)
        {
            uart_log_print("[RX] ID=0x%03X  Data: %02X %02X %02X %02X %02X %02X %02X %02X\r\n",
                           (unsigned int)rx_header.StdId,
                           rx_data[0], rx_data[1], rx_data[2], rx_data[3],
                           rx_data[4], rx_data[5], rx_data[6], rx_data[7]);
        }
    }
}

/**
 * @brief CAN RX FIFO 中断回调函数（由 HAL_CAN_IRQHandler 调用）
 */
void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
    if (HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &rx_header, rx_data) == HAL_OK)
    {
        uart_log_print("[RX-ISR] ID=0x%03X  Data: %02X %02X %02X %02X %02X %02X %02X %02X\r\n",
                       (unsigned int)rx_header.StdId,
                       rx_data[0], rx_data[1], rx_data[2], rx_data[3],
                       rx_data[4], rx_data[5], rx_data[6], rx_data[7]);
    }
}