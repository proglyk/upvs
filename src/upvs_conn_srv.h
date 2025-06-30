#ifndef _UPVS_SRV_SESS3_H_
#define _UPVS_SRV_SESS3_H_

#include "mqtt_srv2.h"
#include "FreeRTOS.h"
#include "timers.h"

// Connection context
struct conn_cxt_st {
  s32_t        slSock;  // Сокет подключения
  mqtt_srv_t  *pxMqtt;  // Экземпляр сервера MQTT
  xTimerHandle xTimer;  // программный таймер для исходящих сообщ
};

#endif //_UPVS_SRV_SESS3_H_