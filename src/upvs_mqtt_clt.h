#ifndef UPVS_MQTT_CLT_H
#define UPVS_MQTT_CLT_H

#include "userint.h"
#include "MQTTClient.h"
#include "upvs_conf.h"
#include "upvs_clt.h"

// Управляющая структура upvs_mqtt_srv

typedef struct {
  //s32_t count;
  //Network   *pxHooks;
  MQTTClient xControl;
  u8_t ucSndBuf[MQTT_BUFSIZE]; //mqtt send buffer
  u8_t ucRcvBuf[MQTT_BUFSIZE]; //mqtt receive buffer
  MQTTPacket_connectData xDataConn;
  // data_t       xData;             //
  // char         id[MAX_ID_SIZE];  // Идентиф. клиента, уник. для разных сессий
  // tb_client_t *pxClients;        // указатель на список подл.клиентов
  // decoder_t    xDecod;           // Разбор вход сообщений
  upvs_clt_t  *pxUpvs;           // 
  // send_ptr_t   psSendCb;          //
  // void        *pvCtx;           //  в кач-ве payload исп. "родитель" ctx_t
} mqtt_clt_t;

void *upvs_mqtt_clt__create(void);
int   upvs_mqtt_clt__init(mqtt_clt_t *, s32_t);
void  upvs_mqtt_clt__del(mqtt_clt_t *);
//s32_t	upvs_mqtt_clt__send( mqtt_clt_t *, const u8_t *, u32_t );
s32_t upvs_mqtt_clt__accept_err(mqtt_clt_t *, u32_t);

#endif //UPVS_MQTT_CLT_H