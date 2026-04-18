#ifndef __CAN_APP_H__
#define __CAN_APP_H__

#include "can.h"
#include <stdint.h>

// =============================================
//  ★ 烧录前修改这里区分两块板 ★
//  板A: #define BOARD_ID 0x101
//  板B: #define BOARD_ID 0x201
// =============================================
#define BOARD_ID  0x201

#if (BOARD_ID == 0x101)
  #define PEER_ID  0x201
#else
  #define PEER_ID  0x101
#endif

void can_app_init(void);
void can_app_send(void);
void can_app_receive(void);

#endif /* __CAN_APP_H__ */