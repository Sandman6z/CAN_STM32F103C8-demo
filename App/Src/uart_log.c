#include "uart_log.h"
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#define LOG_BUF_SIZE 128

/**
 * @brief 初始化串口日志
 */
void uart_log_init(void)
{
    uart_log_print("============================\r\n");
    uart_log_print("  UART Log Initialized\r\n");
    uart_log_print("============================\r\n\r\n");
}

/**
 * @brief 格式化打印，直接通过 HAL 发送，不经过 printf 缓冲
 */
void uart_log_print(const char *fmt, ...)
{
    char buf[LOG_BUF_SIZE];
    va_list args;
    va_start(args, fmt);
    int len = vsnprintf(buf, sizeof(buf), fmt, args);
    va_end(args);
    HAL_UART_Transmit(&huart1, (uint8_t *)buf, len, HAL_MAX_DELAY);
}