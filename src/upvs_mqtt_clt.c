#include "upvs_mqtt_clt.h"
#include <stdlib.h>
#include <string.h>
#include "MQTTInterface.h"
#include "dbg.h"

struct mqtt_clt_st {
  //s32_t count;
  //Network   *pxHooks;
  MQTTClient xControl;
  u8_t ucSndBuf[MQTT_BUFSIZE]; //mqtt send buffer
  u8_t ucRcvBuf[MQTT_BUFSIZE]; //mqtt receive buffer
  MQTTPacket_connectData xDataConn;
  upvs_clt_t  *pxUpvs;           // 
  u8_t acStrTemp[UPVS_TOPICPATH_SIZE];
  //u8_t  ucSta;
};

// привЯзки
extern Network xNetwork;

static void received(MessageData* msg, void *pld);

/**	----------------------------------------------------------------------------
	* @brief ??? */
void *
  upvs_mqtt_clt__create(void) {
/*----------------------------------------------------------------------------*/
  mqtt_clt_t *self = malloc(sizeof(mqtt_clt_t));
  if (!self) return NULL;
  
  self->pxUpvs = upvs_clt__create();
  if (!self->pxUpvs) return NULL;
  
  return (void *)self;
}

/**	----------------------------------------------------------------------------
	* @brief */
int
  upvs_mqtt_clt__init(mqtt_clt_t *self, s32_t sock) {
/*----------------------------------------------------------------------------*/
  int rc;
  if (!self) return -1;
  // if (!cb) return -1;
  
  if (upvs_clt__init(self->pxUpvs) < 0) return -1;
  
  //self->pxHooks = &xNetwork;
  //self->pxHooks->socket = sock;
  xNetwork.socket = sock;
  MQTTClientInit( &(self->xControl), &xNetwork, 1000,
                  self->ucSndBuf, sizeof(self->ucSndBuf),
                  self->ucRcvBuf, sizeof(self->ucRcvBuf), (void *)self );
  
  // запуск еще одной задачи
  rc = MQTTStartTask(&(self->xControl));
  if (rc != MQTT_SUCCESS) {
    //DBG_PRINT( NET_DEBUG, ("Can't create MQTTStartTask, in '%s' /UPVS2/upvs_clt_sess.c:%d\r\n", 
    //  __FUNCTION__, __LINE__) );
    return -1;
  }
  
    // Подключение с блокировкой
  memset((void *)&(self->xDataConn), NULL, sizeof(MQTTPacket_connectData));
  memcpy((void *)&(self->xDataConn.struct_id), (const void *)"MQTC", 
    sizeof(self->xDataConn.struct_id));
  memcpy((void *)&(self->xDataConn.will.struct_id), (const void *)"MQTW", 
    sizeof(self->xDataConn.will.struct_id));
  self->xDataConn.willFlag = 0;
	self->xDataConn.MQTTVersion = 4;
	self->xDataConn.clientID.cstring = "STM32F4";
	self->xDataConn.username.cstring = "client";
	self->xDataConn.password.cstring = "public";
	self->xDataConn.keepAliveInterval = 5;
	self->xDataConn.cleansession = 1;
  rc = MQTTConnect(&(self->xControl), &(self->xDataConn));
  if (rc != MQTT_SUCCESS) {
    //DBG_PRINT( NET_DEBUG, ("Can't connect to ..., in '%s' /UPVS2/upvs_mqtt_clt.c:%d\r\n", 
    //  __FUNCTION__, __LINE__) );
    return -1;
  }
  // Ставим статус и выводим сообщение
  //self->ucSta |= STA_CONNECTED;
  //DBG_PRINT( NET_DEBUG, ("Connected to ..., in '%s' /UPVS2/upvs_mqtt_clt.c:%d\r\n", 
  //  __FUNCTION__, __LINE__) ); 
  
  // Подписка
	rc = MQTT_SUCCESS;
	rc |= MQTTSubscribe(&(self->xControl), "action/CSC", QOS0, received);
  rc |= MQTTSubscribe(&(self->xControl), "action/CSC/reset_faults", QOS0, received);
  rc |= MQTTSubscribe(&(self->xControl), "action/CSC/channel_1", QOS0, received);
  rc |= MQTTSubscribe(&(self->xControl), "action/CSC/channel_1_frequency", QOS0, received);
  rc |= MQTTSubscribe(&(self->xControl), "action/CSC/channel_2", QOS0, received);
  rc |= MQTTSubscribe(&(self->xControl), "action/CSC/channel_3", QOS0, received);
  rc |= MQTTSubscribe(&(self->xControl), "action/CSC/datetime", QOS0, received);
  if (rc != MQTT_SUCCESS) {
    //DBG_PRINT( NET_DEBUG, ("Can't subscribe to ..., in '%s' /UPVS2/upvs_mqtt_clt.c:%d\r\n", 
    //  __FUNCTION__, __LINE__) );
    return -1;
  }
  //self->ucSta |= STA_SUBSCRIBED;
  //DBG_PRINT( NET_DEBUG, ("Subscribed to ..., in '%s' /UPVS2/upvs_mqtt_clt.c:%d\r\n", 
  //  __FUNCTION__, __LINE__) );
  
  return 0;
}

/**	----------------------------------------------------------------------------
	* @brief */
void
  upvs_mqtt_clt__del(mqtt_clt_t *self) {
/*----------------------------------------------------------------------------*/
  if (!self) return;
  //if (!self->pxUpvs) return;
  
  if (self->pxUpvs) upvs_clt__del(self->pxUpvs);
  // free(self->pxClients);
  // free(self->xData.rx.pcBuf);
  // free(self->xData.tx.pcBuf);
  free(self);
}

/**	----------------------------------------------------------------------------
	* @brief */
upvs_clt_t *
  upvs_mqtt_clt__clt_inst(mqtt_clt_t *self) {
/*----------------------------------------------------------------------------*/
  return self->pxUpvs ? self->pxUpvs : NULL;
}

/**	----------------------------------------------------------------------------
	* @brief */
bool
  upvs_mqtt_clt__is_conntd(mqtt_clt_t *self) {
/*----------------------------------------------------------------------------*/
  return self->xControl.isconnected;
}

/**	----------------------------------------------------------------------------
	* @brief ???
	* @retval Статус выполнения */
static void
	received( MessageData* msg, void *pld ) {
/*----------------------------------------------------------------------------*/
  mqtt_clt_t *self = (mqtt_clt_t *)pld;
  if (!self) return;
  
  // Копируем топик лишь для того, чтобы привести его в 'строковый' вид
  // с null-терминальным символом (необходимо для функций из <string.h>)
  memcpy( (void *)self->acStrTemp, 
          (const void *)msg->topicName->lenstring.data,
          msg->topicName->lenstring.len);
  self->acStrTemp[msg->topicName->lenstring.len] = '\0';
  // вызов функции записи
  upvs_clt__set( self->pxUpvs, (const u8_t *)self->acStrTemp,
                 msg->topicName->lenstring.len, 
                 (const u8_t *)msg->message->payload,
                 msg->message->payloadlen );
}

/**	----------------------------------------------------------------------------
	* @brief ??? */
s32_t
  upvs_mqtt_clt__send(mqtt_clt_t *self, const u8_t *path, const u8_t *desc) {
/*----------------------------------------------------------------------------*/
  MQTTMessage message;
  
  // Берем имя топика и шлём (Publish) брокеру полученное сообщение
  message.qos = QOS1;
  message.payload = (void*)desc;
  message.payloadlen = strlen((const char *)desc);
  int ret = MQTTPublish(&self->xControl, (const char *)path, &message);
  if (ret != MQTT_SUCCESS) return -1;
  return 0;
}

