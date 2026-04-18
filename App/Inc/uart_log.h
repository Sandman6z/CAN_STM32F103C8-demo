#ifndef __UART_LOG_H__
#define __UART_LOG_H__

#include "usart.h"

void uart_log_init(void);
void uart_log_print(const char *fmt, ...);

#endif /* __UART_LOG_H__ */