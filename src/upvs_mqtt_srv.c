
#include "upvs_conf.h"
#include "upvs_mqtt_srv.h"
#include <stdbool.h>
#include <string.h>
#include "dbg.h"


// Функции, доступные извне


// Функции, ограниченные областью видимости данного файла

// Обработка входящих запросов
static err_enum_t	incoming_con(mqtt_srv_t *);
static err_enum_t	incoming_discon(mqtt_srv_t *);
static err_enum_t	incoming_ping_req(mqtt_srv_t *);
static err_enum_t	incoming_ping_resp(data_t *);
static err_enum_t	incoming_puback(data_t *);
static err_enum_t	incoming_pubcomp(data_t *);
static err_enum_t	incoming_pub(mqtt_srv_t *);
static err_enum_t	incoming_pubrec(mqtt_srv_t *);
static err_enum_t	incoming_pubrel(mqtt_srv_t *);
static err_enum_t	incoming_sub(mqtt_srv_t *);
static err_enum_t	incoming_unsub(mqtt_srv_t *);

// Формирование исходящих сообщений
static err_enum_t	outgoing_msg( mqtt_srv_t *, outmsg_t, u8_t, u16_t,
                                u8_t*, u32_t, u8_t*, u32_t );

// Работа с клиентами
static bool is_client_connected(mqtt_srv_t *, char *);
bool is_client_authorised (char*, char*);
static int client_getpos(mqtt_srv_t *, const char *);
static err_enum_t client_remove(mqtt_srv_t *, char*);
static inline bool can_broker_accept_next_client(mqtt_srv_t *);
static err_enum_t client_insert_v2(mqtt_srv_t *, con_msg_t *);
static int broker_first_free_pos_for_client(mqtt_srv_t *);

// Работа с буфером данных
static err_enum_t	buf_read_remaining(data_t *, u32_t *);
static err_enum_t	buf_write_remaining(data_t *, u32_t);
static err_enum_t	buf_write_bytes(data_t*, u8_t*, u32_t);
static err_enum_t buf_decode_connect(data_t *, con_msg_t *);
static err_enum_t buf_decode_publish(data_t *, pub_msg_t *);
static int        buf_decode_sub_unsubscribe(data_t *, sub_unsub_msg_t *, u8_t *);

// Работа с подписками
static int  subs_add_update_topic_list(sub_topic_t *, sub_unsub_msg_t *, u8_t, u8_t *);
static void subs_remove_topic_list(sub_topic_t *, sub_unsub_msg_t *, u8_t);
static bool subs_add_topic(sub_topic_t *, sub_topic_ptr_t*);
static void subs_remove_topic(sub_topic_t *, sub_topic_ptr_t *);
static int  subs_get_topic_pos(sub_topic_t *, sub_topic_ptr_t*);


/**	----------------------------------------------------------------------------
	* @brief ??? */
void *
  mqtt_srv__create(void) {
/*----------------------------------------------------------------------------*/
  mqtt_srv_t *self = malloc(sizeof(mqtt_srv_t));
  if (!self) return NULL;
  
  self->xData.rx.pcBuf = malloc(DECODER_BUFSIZE_RX * sizeof(u8_t));
  if (!self->xData.rx.pcBuf) return NULL;
  
  self->xData.tx.pcBuf = malloc(DECODER_BUFSIZE_TX * sizeof(u8_t));
  if (!self->xData.tx.pcBuf) return NULL;
  
  self->pxUpvs = upvs_srv__create();
  if (!self->pxUpvs) return NULL;
  
  self->pxClients = malloc(sizeof(tb_client_t));
  if (!self->pxClients) return NULL;
  
  return (void *)self;
}

/**	----------------------------------------------------------------------------
	* @brief */
int
  mqtt_srv__init(mqtt_srv_t *self, send_ptr_t cb) {
/*----------------------------------------------------------------------------*/
  if (!self) return -1;
  if (!cb) return -1;
  
  if (upvs_srv__init(self->pxUpvs) < 0) return -1;
  self->psSendCb = cb;
  
  self->xData.rx.usCar = 0;
  self->xData.rx.usSize = (u16_t)DECODER_BUFSIZE_RX;
  self->xData.tx.usCar = 0;
  self->xData.tx.usSize = (u16_t)DECODER_BUFSIZE_TX;
  
  return 0;
}

/**	----------------------------------------------------------------------------
	* @brief ??? */
err_enum_t
  mqtt_srv__input(mqtt_srv_t *self, u32_t recvd) {
/*----------------------------------------------------------------------------*/
  u8_t byte;
  err_enum_t rc = ERR_OK;
  data_t* pbuf;
  // buf_t *pbufrx = &self->xBuf.rx;
  // buf_t *pbuftx = &self->xBuf.tx;
  
  // проверка арг-ов
  if (!self) return NULL;
  pbuf = &(self->xData);
  
  pbuf->rx.usCar = 0;
  pbuf->rx.usSize = (u16_t)recvd;
  pbuf->tx.usCar = 0;
  
    // 1.2 Разбор сообщения по типу
  BUF_READ_BYTE1(pbuf, &byte, false);
  switch(byte&0xF0) {
    case CONNECT:     rc = incoming_con(self); break;
    case DISCONNECT:  rc = incoming_discon(self); break;
    case PINGREQ:     rc = incoming_ping_req(self); break;
    case PINGRESP:    rc = incoming_ping_resp(pbuf); break;
    case PUBACK:      rc = incoming_puback(pbuf); break;
    case PUBCOMP:     rc = incoming_pubcomp(pbuf); break;
    // Ответ cmd=PUBACK,PUBREC
    case PUBLISH:     rc = incoming_pub(self); break;
    // Ответ cmd=PUBREL
    case PUBREC:      rc = incoming_pubrec(self); break;
    // ответ cmd=PUBCOMP
    case PUBREL:      rc = incoming_pubrel(self); break;
    // ответ cmd=SUBACK
    case SUBSCRIBE:   rc = incoming_sub(self); break;
    // ответ cmd=UNSUBACK
    case UNSUBSCRIBE: rc = incoming_unsub(self); break;
    // FIXME - do something?
    default: break;
  }
  
  return rc;
}

/**	----------------------------------------------------------------------------
	* @brief */
void
  mqtt_srv__del(mqtt_srv_t *self) {
/*----------------------------------------------------------------------------*/
  if (!self) return;
  if (!self->pxUpvs) return;
  
  upvs_srv__del(self->pxUpvs);
  free(self->pxClients);
  free(self->xData.rx.pcBuf);
  free(self->xData.tx.pcBuf);
  free(self);
}


// Функции, ограниченные областью видимости файла

// Обработка входящих запросов

/**	----------------------------------------------------------------------------
	* @brief ???
	* @retval error: Статус выполнения функции. */
static err_enum_t
	incoming_con(mqtt_srv_t *self) {
/*----------------------------------------------------------------------------*/
  //srverr_t rslt;
  bool session_present = false;
  int connect_code=0;
  err_enum_t rc = ERR_OK;
  
  if (!self) return ERR_VAL;
  
  data_t *pbuf = &(self->xData);
  con_msg_t* con_msg = &(self->xDecod.con);
  
  // Разбор входящего сообщения
  if (rc = buf_decode_connect(pbuf, con_msg)) {
    connect_code = CONN_ACK_BAD_PROTO;
    goto exit;
  }
  // Return code 2 is sent if the client id is not between 1 and 23 in length.
  if ((con_msg->payload.client_id_len==0)|(con_msg->payload.client_id_len>23)) {
    connect_code = CONN_ACK_BAD_ID;
    goto exit;
  }
  // Проверка соответствия версии V3.1.0 (MQIsdp). Если условие не выполняется,
  // то отправляется сообщение CONNACK с флагом 0x01	"unacceptable protocol 
  // version"
  if (memcmp( (const char *)con_msg->head.var.proto_name,
              PROTOCOL_NAME,
              con_msg->head.var.proto_len) )
  {
    connect_code = CONN_ACK_BAD_PROTO;
    goto exit;
  }
  // Проверка соответствия версии V3 (временно 0x83 почему-то)
  if (con_msg->head.var.proto_ver != PROTOCOL_VERSION) {
    connect_code = CONN_ACK_BAD_PROTO;
    goto exit;
  }
  // наличие логина и пароля при активных флагах
  if (con_msg->head.var.flags.bit.user_name & (con_msg->payload.usr_name_len==0)) {
    connect_code = CONN_ACK_AUTH_MALFORM;
    goto exit;
  }
  if (con_msg->head.var.flags.bit.pswd & (con_msg->payload.pswd_len==0)) {
    connect_code = CONN_ACK_AUTH_MALFORM;
    goto exit;
  }
  // Авторизация через пару логин/пароль
  if ( !is_client_authorised( (char *)con_msg->payload.usr_name, 
                              (char *)con_msg->payload.pswd) )
  {
    connect_code = CONN_ACK_BAD_AUTH;
    goto exit;
  }
  
  // если запрошена "чистая сессия" и клиент уже есть в списке подключенных, 
  // то удалить его
  if ( (con_msg->head.var.flags.bit.cleans_session) & 
       (client_getpos(self, (const char *)con_msg->payload.client_id) >= 0) )
  {
    client_remove(self, (char *)con_msg->payload.client_id);
    session_present = true;
  }
  // вне зависимости от флага "cleans_session" проверить, есть ли возможность 
  // добавить нового клиента...
  if (!can_broker_accept_next_client(self)) {
    connect_code = CONN_ACK_NOT_AVBL;
    goto exit;
  }
  // ...и если да, то создать новый экземпляр клиента
  if (rc = client_insert_v2(self, con_msg)) {
    connect_code = CONN_ACK_NOT_AVBL;
    goto exit;
  }
  memcpy(self->id, (const char *)con_msg->payload.client_id, con_msg->payload.client_id_len);
  
  DBG_PRINT( NET_DEBUG, ("Client UPVS '%s' has added, in '%s' /UPVS/upvs_srv_sess.c:%d\r\n", 
    self->id, __FUNCTION__, __LINE__) );
  
  
  // FIXME не хватает session_present
  return outgoing_msg( self, MSG_MID, (u8_t)CONNACK, CONN_ACK_OK,
                       NULL, 0, NULL, 0 );
  
  exit:
  rc = outgoing_msg( self, MSG_MID, (u8_t)CONNACK, connect_code,
                     NULL, 0, NULL, 0 );
  if (rc != ERR_OK) return rc;
  return ERR_VAL;
}

/**	----------------------------------------------------------------------------
	* @brief ???
	* @retval error: Статус выполнения функции. */
static err_enum_t
	incoming_discon(mqtt_srv_t *self) {
/*----------------------------------------------------------------------------*/
  if (strcmp(self->id, "")) {
    if (is_client_connected(self, self->id)) {
      DBG_PRINT( NET_DEBUG, ("Client UPVS %s has removed, in '%s' /UPVS/upvs_srv_sess.c:%d\r\n", 
        self->id, __FUNCTION__, __LINE__) );
      client_remove(self, self->id);
    }
  }
  return ERR_OK;
}

/**	----------------------------------------------------------------------------
	* @brief ???
	* @retval error: Статус выполнения функции. */
static err_enum_t
	incoming_ping_req(mqtt_srv_t *self) {
/*----------------------------------------------------------------------------*/
  err_enum_t rslt;
  u32_t remainlen;
  data_t* pbuf = &(self->xData);
  
  if (rslt = buf_read_remaining(pbuf, &remainlen)) return rslt;
  return outgoing_msg( self, MSG_SIMPLE, (u8_t)PINGRESP, 0, NULL, 0, NULL, 0);
}

/**	----------------------------------------------------------------------------
	* @brief ???
	* @retval error: Статус выполнения функции. */
static err_enum_t
	incoming_ping_resp(data_t* pbuf) {
/*----------------------------------------------------------------------------*/
  u32_t remainlen;
  
  return buf_read_remaining(pbuf, &remainlen);
}

/**	----------------------------------------------------------------------------
	* @brief ???
	* @retval error: Статус выполнения функции. */
static err_enum_t
	incoming_puback(data_t* pbuf) {
/*----------------------------------------------------------------------------*/
  err_enum_t rslt;
  u32_t remainlen;
  u16_t mid;
  
  if (rslt = buf_read_remaining(pbuf, &remainlen)) return rslt;
  BUF_READ_WORD(pbuf, &mid);
  
  if(mid){
		//if(mqtt3_db_message_delete(context->id, mid, md_out)) return 1;
	}
  return ERR_OK;
}

/**	----------------------------------------------------------------------------
	* @brief ???
	* @retval error: Статус выполнения функции. */
static err_enum_t
	incoming_pubcomp(data_t* pbuf) {
/*----------------------------------------------------------------------------*/
  err_enum_t rslt;
  u32_t remainlen;
  u16_t mid;
  
  if (rslt = buf_read_remaining(pbuf, &remainlen)) return rslt;
  BUF_READ_WORD(pbuf, &mid);
  
  if(mid){
		//if(mqtt3_db_message_delete(context->id, mid, md_out)) return 1;
	}
  
  return ERR_OK;
}

/**	----------------------------------------------------------------------------
	* @brief ???
	* @retval error: Статус выполнения функции. */
static err_enum_t
	incoming_pub(mqtt_srv_t *self) {
/*----------------------------------------------------------------------------*/
  err_enum_t rc = ERR_OK;
  s32_t sta = 0;
  //err_enum_t rslt;
  data_t *pbuf = &(self->xData);
  pub_msg_t* pub_msg = &(self->xDecod.pub);
  
  // Разбор входящего сообщения
  rc = buf_decode_publish(pbuf, pub_msg);
  if (rc != ERR_OK) return rc;
  //if (rslt = buf_decode_publish(pbuf, pub_msg)) return rslt;
  // Должен быть правильный id клиента
  if (!strcmp(self->id, "")) return ERR_VAL;
  // Клиент должен присутствовать в списке подключенных
  if (!is_client_connected(self, self->id)) return ERR_TIMEOUT;

  // после успешных проверок необходимо расозлать полученное сообщение 
  // подписчикам. Так как в стенде их нет, то мы этот шаг опускаем
  //publish_msg_to_subscribers(broker, &pub_pck);
  
  // Нужно разобрать JSON-содержимое сообщения ...
  // FIXME
  sta = upvs_srv__set( self->pxUpvs,
                       (const u8_t *)pub_msg->head.var.topic_name,
                       (u32_t)pub_msg->head.var.topic_len, 
                       (const u8_t *)pub_msg->payload );
	if (sta < 0) {
    DBG_PRINT( NET_DEBUG, ("Can't process ingoing PUB (sta=%02d), in '%s' " \
      "/UPVS/upvs_srv_sess.c:%d\r\n", sta, __FUNCTION__, __LINE__) );
    return ERR_VAL;
	}
  
  // ...и затем отправить его в экранную форму стенда
  // send_to_stend_form FIXME

  // ответ в зависимости от QoS
  switch(pub_msg->head.fix.ctrl.bit.QoS) {
		case 0: {
			// ничего
			break;
    }
		case 1: {
      rc = outgoing_msg( self, MSG_MID, (u8_t)PUBACK, pub_msg->head.var.packet_id,
                         NULL, 0, NULL, 0 );
      break;
    }
		case 2: {
      rc = outgoing_msg( self, MSG_MID, (u8_t)PUBREC, pub_msg->head.var.packet_id,
                         NULL, 0, NULL, 0 );
      break;
    }
	}
 
  return rc; //FIXME восстановить
}

/**	----------------------------------------------------------------------------
	* @brief ???
	* @retval error: Статус выполнения функции. */
static err_enum_t
	incoming_pubrec(mqtt_srv_t *self) {
/*----------------------------------------------------------------------------*/
  err_enum_t rslt;
  u32_t remainlen;
  u16_t mid;
  data_t* pbuf = &(self->xData);
  
  if (rslt = buf_read_remaining(pbuf, &remainlen)) return rslt;
  BUF_READ_WORD(pbuf, &mid);
  // if(mqtt3_db_message_update(context->id, mid, md_out, ms_wait_pubcomp)) return 1;
  return outgoing_msg( self, MSG_MID, (u8_t)PUBREL, mid, NULL, 0, NULL, 0 );
}

/**	----------------------------------------------------------------------------
	* @brief ???
	* @retval error: Статус выполнения функции. */
static err_enum_t
	incoming_pubrel(mqtt_srv_t *self) {
/*----------------------------------------------------------------------------*/
  err_enum_t rslt;
  u32_t remainlen;
  u16_t mid;
  data_t* pbuf = &(self->xData);
  
  if (rslt = buf_read_remaining(pbuf, &remainlen)) return rslt;
  BUF_READ_WORD(pbuf, &mid);
  // if(mqtt3_db_message_release(context->id, mid, md_in)) return 1;
  return outgoing_msg( self, MSG_MID, (u8_t)PUBCOMP, mid, NULL, 0, NULL, 0 );
}

/**	----------------------------------------------------------------------------
	* @brief ???
	* @retval error: Статус выполнения функции. */
static err_enum_t
  incoming_sub(mqtt_srv_t *self) {
/*----------------------------------------------------------------------------*/
  data_t *pbuf = &(self->xData);
  sub_unsub_msg_t* sub_msg = &(self->xDecod.sub);
  // Кол-во топиков для подписки
  u8_t topics_num, sub_result[MAX_SUBS_TOPIC];
  int client_pos;
  sub_topic_t *plist;
  
  // Разбор входящего сообщения
  if (buf_decode_sub_unsubscribe(pbuf, sub_msg, &topics_num)) goto miss;
  // FIXME эта часть мне не нравится - self->id как-будто не к месту
  // Должен быть правильный id клиента
  if (!strcmp(self->id, "")) goto miss;
  // Клиент должен присутствовать в списке подключенных
  if (!is_client_connected(self, self->id)) return ERR_CLSD;
  // получаем номер клиента в списке подписчиков
  if ((client_pos = client_getpos(self, self->id)) == -1) goto miss;
  plist = (self->pxClients + client_pos)->axTopicList;
  // Добавить новую либо обновить QoS существующей подписки.
  subs_add_update_topic_list(plist, sub_msg, topics_num, sub_result);
  
  DBG_PRINT( NET_DEBUG, ("Client UPVS %s has subscribed, in '%s' /UPVS/upvs_srv_sess.c:%d\r\n", 
    self->id, __FUNCTION__, __LINE__) );

  // формируем и отправляем ответ "SUBACK"
  return outgoing_msg( self, MSG_PAYLOAD, (u8_t)SUBACK,
                       sub_msg->head.var.packet_id,
                       NULL, 0, sub_result, topics_num );
  
  miss:
  return ERR_OK;
}

/**	----------------------------------------------------------------------------
	* @brief ???
	* @retval error: Статус выполнения функции. */
static err_enum_t
	incoming_unsub(mqtt_srv_t *self) {
/*----------------------------------------------------------------------------*/
  u8_t topics_num;
  int client_pos;
  sub_topic_t *plist;
  data_t *pbuf = &(self->xData);
  sub_unsub_msg_t* unsub_msg = &(self->xDecod.unsub);

  // Разбор входящего сообщения
  if (buf_decode_sub_unsubscribe(pbuf, unsub_msg, &topics_num)) goto miss;
  // FIXME эта часть мне не нравится - self->id как-будто не к месту
  // Должен быть верный id и клиент должен быть в списке подключенных
  if (!strcmp(self->id, "") || !is_client_connected(self, self->id)) return ERR_CLSD;
  
  // получаем номер клиента в списке подписчиков
  if ((client_pos = client_getpos(self, self->id)) == -1) goto miss;
  plist = (self->pxClients + client_pos)->axTopicList;
  //subs_delete_topic(plist, unsub_msg);
  subs_remove_topic_list(plist, unsub_msg, topics_num);

  // формируем и отправляем ответ "UNSUBACK"
  return outgoing_msg( self, MSG_MID, (u8_t)UNSUBACK,
                       unsub_msg->head.var.packet_id, NULL, 0, NULL, 0 );
  miss:
  return ERR_OK;
}


// Работа с клиентами

/**	----------------------------------------------------------------------------
	* @brief ???
	* @retval error: Статус выполнения функции. */
static bool
  is_client_connected(mqtt_srv_t *self, char* client_id) {
/*----------------------------------------------------------------------------*/
	for (uint8_t i =0; i < MAX_CONN_CLIENTS; i++) {
		if ( (self->pxClients[i].connected) && (strcmp(self->pxClients[i].id, 
      client_id)==0) ) return true; // SRV_OK=0
	}
	return false; //discon=-3
}

/**	----------------------------------------------------------------------------
	* @brief ???
	* @retval error: Статус выполнения функции. */
__attribute__( ( weak ) ) bool
is_client_authorised (char* usr_name, char* pswd) {
/*----------------------------------------------------------------------------*/
	return true;
}

/**	----------------------------------------------------------------------------
	* @brief ???
	* @retval error: Статус выполнения функции. */
static int
	client_getpos(mqtt_srv_t* self, const char *client_id) {
/*----------------------------------------------------------------------------*/
  for (uint8_t i = 0; i < MAX_CONN_CLIENTS; i++) {
		if (strcmp(self->pxClients[i].id, client_id) ==0 ) {
			return i;
		}
	}
  return -1;
}

/**	----------------------------------------------------------------------------
	* @brief ???
	* @retval error: Статус выполнения функции. */
static err_enum_t
  client_remove(mqtt_srv_t *self, char *client_id) {
/*----------------------------------------------------------------------------*/
  int pos = client_getpos(self, client_id);
  if (pos < 0) return ERR_VAL;
  memset(&(self->pxClients[pos]), 0, sizeof(tb_client_t));
  return ERR_OK;
}

/**	----------------------------------------------------------------------------
	* @brief ???
	* @retval error: Статус выполнения функции. */
static inline bool
can_broker_accept_next_client(mqtt_srv_t *self) {
/*----------------------------------------------------------------------------*/
	for (uint8_t i = 0; i < MAX_CONN_CLIENTS; i++){
    if (!strcmp(self->pxClients[i].id, "")) {
      return true;
    }
	}
  return false;
}

/**	----------------------------------------------------------------------------
	* @brief ???
	* @retval error: Статус выполнения функции. */
static err_enum_t
  client_insert_v2(mqtt_srv_t *self, con_msg_t *pmsg) {
/*----------------------------------------------------------------------------*/
  int pos;
  
  if ( (pos = broker_first_free_pos_for_client(self)) < 0 ) return ERR_VAL;
  
  memset(&(self->pxClients[pos]), NULL, sizeof(tb_client_t));
  //memcpy(&new_client->sockaddr, &sockaddr, sizeof (sockaddr_t));
  strncpy(self->pxClients[pos].id, (const char *)pmsg->payload.client_id, 
    pmsg->payload.client_id_len);
  self->pxClients[pos].keepalive = pmsg->head.var.keep_alive;
  if (pmsg->head.var.flags.bit.will_retain) {
    self->pxClients[pos].will_retain = 1;
  }
  if (pmsg->head.var.flags.bit.last_will) {
		self->pxClients[pos].will_retain = 1;
		strncpy(self->pxClients[pos].will_topic, (const char *)pmsg->payload.will_topic, 
      pmsg->payload.will_topic_len);
		strncpy(self->pxClients[pos].will_msg, (const char *)pmsg->payload.will_msg,
      pmsg->payload.will_msg_len);
		self->pxClients[pos].will_qos = pmsg->head.var.flags.bit.will_qos;
	}
  if (pmsg->head.var.flags.bit.user_name){
		strncpy(self->pxClients[pos].username, (const char *)pmsg->payload.usr_name, 
      pmsg->payload.usr_name_len);
	}
	if (pmsg->head.var.flags.bit.pswd) {
		strncpy(self->pxClients[pos].password, (const char *)pmsg->payload.pswd, 
      pmsg->payload.pswd_len);
	}
  self->pxClients[pos].connected = true;
  
  return ERR_OK;
}

/**	----------------------------------------------------------------------------
	* @brief ???
	* @retval error: Статус выполнения функции. */
static int
broker_first_free_pos_for_client(mqtt_srv_t* self) {
/*----------------------------------------------------------------------------*/
	for (uint8_t i = 0; i < MAX_CONN_CLIENTS; i++) {
		if (!strcmp(self->pxClients[i].id, "")) {
      return (int)i;
    }
	}
	return -1;
}

// Работа с буфером данных

/**	----------------------------------------------------------------------------
	* @brief ??? */
static err_enum_t
	buf_read_remaining(data_t* pbuf, u32_t* remaining) {
/*----------------------------------------------------------------------------*/
  u32_t multiplier = 1;
  u8_t digit;
  
  if (!remaining) return ERR_VAL;
  
  /* Algorithm for encoding taken from pseudo code at
	 * http://publib.boulder.ibm.com/infocenter/wmbhelp/v6r0m0/topic/com.ibm.etools.mft.doc/ac10870_.htm
	 */
  (*remaining) = 0;
	do {
    if (pbuf->rx.usCar >= DECODER_BUFSIZE_RX) return ERR_VAL;
    BUF_READ_BYTE1(pbuf, &digit, true);
		(*remaining) += (digit & 127) * multiplier;
		multiplier *= 128;
	} while((digit & 128) != 0);
  
	return ERR_OK;
}

/**	----------------------------------------------------------------------------
	* @brief ???
	* @retval error: Статус выполнения функции. */
static err_enum_t
	buf_write_remaining(data_t* pbuf, u32_t length) {
/*----------------------------------------------------------------------------*/
  u8_t digit;
  
  /* Algorithm for encoding taken from pseudo code at
	 * http://publib.boulder.ibm.com/infocenter/wmbhelp/v6r0m0/topic/com.ibm.etools.mft.doc/ac10870_.htm
	 */
	do {
		digit = length % 128;
		length = length / 128;
		/* If there are more digits to encode, set the top bit of this digit */
		if(length>0) {
			digit = digit | 0x80;
		}
    if (pbuf->tx.usCar >= pbuf->tx.usSize) return ERR_VAL;
    BUF_WRITE_BYTE(pbuf, digit);
	} while(length > 0);
  
	return ERR_OK;
}

/**	----------------------------------------------------------------------------
	* @brief ? */
static err_enum_t
	buf_write_bytes(data_t* pbuf, u8_t *bytes, u32_t len) {
/*----------------------------------------------------------------------------*/
  if (!bytes) return ERR_VAL;
  
  memcpy((void*)(pbuf->tx.pcBuf + pbuf->tx.usCar), (const void*)bytes, 
    (size_t)len);
  pbuf->tx.usCar += len;
  
  return ERR_OK;
}

/**	----------------------------------------------------------------------------
	* @brief ???
	* @retval Статус выполения типа srverr_t */
static err_enum_t
buf_decode_connect(data_t* pbuf, con_msg_t *pmsg) {
/*----------------------------------------------------------------------------*/
  err_enum_t rslt;
	
  BUF_READ_BYTE1(pbuf, &(pmsg->head.fix.ctrl), true);
  if (rslt = buf_read_remaining(pbuf, &(pmsg->head.fix.rem_len))) return rslt;
  BUF_READ_WORD(pbuf, &(pmsg->head.var.proto_len));
  BUF_READ_PTR(pbuf, pmsg->head.var.proto_name, pmsg->head.var.proto_len);	
  BUF_READ_BYTE1(pbuf, &(pmsg->head.var.proto_ver), true);	
  BUF_READ_BYTE1(pbuf, &(pmsg->head.var.flags.all), true);	
  BUF_READ_WORD(pbuf, &(pmsg->head.var.keep_alive));	
  BUF_READ_WORD(pbuf, &(pmsg->payload.client_id_len));	
  BUF_READ_PTR(pbuf, pmsg->payload.client_id, pmsg->payload.client_id_len);

  if (pmsg->head.var.flags.bit.last_will) {
    BUF_READ_WORD(pbuf, &(pmsg->payload.will_topic_len));
    BUF_READ_PTR(pbuf, pmsg->payload.will_topic, pmsg->payload.will_topic_len);
    BUF_READ_WORD(pbuf, &(pmsg->payload.will_msg_len));
    BUF_READ_PTR(pbuf, pmsg->payload.will_msg, pmsg->payload.will_msg_len);
  }
  if (pmsg->head.var.flags.bit.user_name) {
    BUF_READ_WORD(pbuf, &(pmsg->payload.usr_name_len));
    BUF_READ_PTR(pbuf, pmsg->payload.usr_name, pmsg->payload.usr_name_len);
  }
  if (pmsg->head.var.flags.bit.pswd) {
    BUF_READ_WORD(pbuf, &(pmsg->payload.pswd_len));
    BUF_READ_PTR(pbuf, pmsg->payload.pswd, pmsg->payload.pswd_len);
  }
  return ERR_OK;
}

/**	----------------------------------------------------------------------------
	* @brief ???
	* @retval Статус выполения типа srverr_t */
static err_enum_t
buf_decode_publish(data_t* pbuf, pub_msg_t *pmsg) {
/*----------------------------------------------------------------------------*/
  err_enum_t rslt;
	
  BUF_READ_BYTE1(pbuf, &(pmsg->head.fix.ctrl.all), true);
  if (rslt = buf_read_remaining(pbuf, &(pmsg->head.fix.rem_len))) return rslt;
  BUF_READ_WORD(pbuf, &(pmsg->head.var.topic_len));
  BUF_READ_PTR(pbuf, pmsg->head.var.topic_name, pmsg->head.var.topic_len);
  if (pmsg->head.fix.ctrl.bit.QoS > 0) {
    BUF_READ_WORD(pbuf, &(pmsg->head.var.packet_id));
  }
  BUF_READ_PTR(pbuf, pmsg->payload, 0);

  return ERR_OK;
}

/**	----------------------------------------------------------------------------
	* @brief ???
	* @retval Статус выполения типа srverr_t */
static int
buf_decode_sub_unsubscribe(data_t* pbuf, sub_unsub_msg_t *pmsg, u8_t *pnum) {
/*----------------------------------------------------------------------------*/
  u8_t fix_head_size = 2;
	u8_t topic_nb = 0;
  sub_topic_ptr_t *ptopic;
	
  BUF_READ_BYTE1(pbuf, &(pmsg->head.fix.ctrl), true);

  if (buf_read_remaining(pbuf, &(pmsg->head.fix.rem_len))) return -1;
  BUF_READ_WORD(pbuf, &(pmsg->head.var.packet_id));
  
  while (pbuf->rx.usCar < (pmsg->head.fix.rem_len + fix_head_size)) {
    // присваиваем локальной переменной новый адрес
    ptopic = &(pmsg->axTopicPtrList[topic_nb]);
    // записываем
    BUF_READ_WORD(pbuf, &(ptopic->len));
    BUF_READ_PTR(pbuf, ptopic->name, ptopic->len);
    BUF_READ_BYTE1(pbuf, &(ptopic->qos), true);
    topic_nb++;
  }
  *pnum = topic_nb;

  return 0;
}


// Формирование исходящих сообщений

/**	----------------------------------------------------------------------------
	* @brief ???
	* @retval error: Статус выполнения функции. */
static err_enum_t
	outgoing_msg( mqtt_srv_t *self, outmsg_t msgtype, u8_t cmd, u16_t mid, 
                u8_t* topic, u32_t topiclen, u8_t* pld, u32_t pldlen ) {
/*----------------------------------------------------------------------------*/
  err_enum_t rslt;
  u32_t written;
  data_t *pbuf = &(self->xData);
  
  switch (msgtype) {
    case MSG_SIMPLE:
      BUF_WRITE_BYTE(pbuf, cmd);
      BUF_WRITE_BYTE(pbuf, 0);
    break;
    case MSG_MID:
      BUF_WRITE_BYTE(pbuf, cmd);
      if (rslt = buf_write_remaining(pbuf, 2)) return rslt;
      BUF_WRITE_WORD(pbuf, mid);
    break;
    case MSG_PAYLOAD:
      BUF_WRITE_BYTE(pbuf, cmd);
      if (rslt = buf_write_remaining(pbuf, pldlen+2)) return rslt;
      BUF_WRITE_WORD(pbuf, mid);
      if (!pld) return ERR_VAL;
      if (rslt = buf_write_bytes(pbuf, pld, pldlen)) return rslt;
    break;
    case MSG_PUBLISH:
      BUF_WRITE_BYTE(pbuf, cmd);
      if (rslt = buf_write_remaining(pbuf, topiclen+2+pldlen)) return rslt;
      if (!topic || !pld) return ERR_VAL;
      BUF_WRITE_WORD(pbuf, (u16_t)topiclen);
      if (rslt = buf_write_bytes(pbuf, topic, topiclen)) return rslt;
      //BUF_WRITE_WORD(pbuf, (u16_t)pldlen);
      if (rslt = buf_write_bytes(pbuf, pld, pldlen)) return rslt;
    break;
    default:
    break;
  }
  
  return self->psSendCb( SOCK_STREAM, pbuf->tx.pcBuf, (u32_t)pbuf->tx.usCar, 
                         &written, NULL, 0, self->pvCtx );
}


// Работа с подписками

/**	----------------------------------------------------------------------------
  * @brief Добавить новую либо обновить QoS существующей подписки. В случае 
  * успешного выполнения в элементы массива sub_result записываются новые QoS в
  * порядке чередования имён топиков в запросе; если же что-то пошло не так, 
  * начиная с того элемента sub_result, на котором была выявлена ошибка, 
  * в массив начинаются записываться SUB_ACK_FAIL.
  * @param plist: список подписок, привязанный к одному подключенному клиенту,
  * обслуживаемому в рамках текущей сессии
  * @param pmsg: декодер входящего сообщения типа SUBSCRIBE
  * @param topics_num: число подписок для добавления/обновления
  * @param sub_result: перечень QoS
	* @retval статус выполнения */
static int
  subs_add_update_topic_list(sub_topic_t *plist, sub_unsub_msg_t *pmsg,
  u8_t topics_num, u8_t *sub_result) {
/*----------------------------------------------------------------------------*/
  int topic_pos;
  u8_t i=0, j;
  sub_topic_ptr_t *ptopic;
  bool res = true;
  
	while (i < topics_num) {
    // присваиваем локальной переменной новый адрес
    ptopic = &(pmsg->axTopicPtrList[i]);
    // находим позицию топика
		topic_pos = subs_get_topic_pos(plist, ptopic);
    // если позиция топика не найдена среди имеющихся подписок...
    if (topic_pos == -1) {
      /// ...  то добавляем его...
      res = subs_add_topic(plist, ptopic);
      //
      if (!res) {
				for (j = i; j < topics_num; j++){
					sub_result[j] = SUB_ACK_FAIL;
				}
			return -1;
			}
      // ... и записываем в результат.
			sub_result[i] = ptopic->qos;
    }
    // если имя топика найдено, то...
    else {
      // ... обновляем его QoS...
      plist->qos = ptopic->qos;
      // ... и записываем в результат.
			sub_result[i] = ptopic->qos;
    }
		i++;
	}
	return 0;
}

/**	----------------------------------------------------------------------------
	* @brief ???
  * @param plist: список подписок, привязанный к одному подключенному клиенту,
  * обслуживаемому в рамках текущей сессии
  * @param pmsg: декодер входящего сообщения типа UNSUBSCRIBE
  * @param topics_num: число подписок под удаление */
static void
  subs_remove_topic_list(sub_topic_t *plist, sub_unsub_msg_t* pmsg, u8_t topics_num) {
/*----------------------------------------------------------------------------*/
  u8_t i=0;

	while (i < topics_num) {
    subs_remove_topic(plist, (pmsg->axTopicPtrList+i));
		i++;
	}
}

/**	----------------------------------------------------------------------------
	* @brief Осуществляет пробежку по массиву подписок и ищет пустую позицию для 
  последующего размещения в ней новой подписки
  * @param plist: список подписок, привязанный к одному подключенному клиенту,
  * обслуживаемому в рамках текущей сессии TCP-соединения
  * @param ptopic: запись, предназначенная для добавления в массив */
static bool
subs_add_topic(sub_topic_t *plist, sub_topic_ptr_t* ptopic) {
/*----------------------------------------------------------------------------*/
  u8_t i;

  // пробежаться по всем подпискам текущего клиента
  for (i=0; i < MAX_SUBS_TOPIC; i++) {
    // найти пустую позицию
    if (!strcmp((plist+i)->name, "")) {
      memcpy((plist+i)->name,  ptopic->name, ptopic->len);
      (plist+i)->len = ptopic->len;
      (plist+i)->qos = ptopic->qos;
      return true;
    }
	}
	return false;
}

/**	----------------------------------------------------------------------------
	* @brief Осуществляет пробежку по массиву подписок и ищет запись, поле строкового 
  типа "name" которой совпадает с аналогичным полем аргумента ptopic
  * @param plist: список подписок, привязанный к одному подключенному клиенту,
  * обслуживаемому в рамках текущей сессии TCP-соединения
  * @param ptopic: запись, предназначенная для удаления из массива */
static void
  subs_remove_topic(sub_topic_t *plist, sub_topic_ptr_t* ptopic) {
/*----------------------------------------------------------------------------*/
  u8_t i;

  for (i = 0; i < MAX_SUBS_TOPIC; i++) {
    if (((plist+i)->name[0]) && (!memcmp((plist+i)->name, ptopic->name, ptopic->len))) {
      memset((plist+i), 0, sizeof(sub_topic_t));
		}
	}
}

/**	----------------------------------------------------------------------------
	* @brief ???
	* @retval error: Статус выполнения функции. */
static int
subs_get_topic_pos(sub_topic_t *plist, sub_topic_ptr_t* ptopic) {
/*----------------------------------------------------------------------------*/
	for (int i = 0; i < MAX_SUBS_TOPIC; i++) {
    if ((plist[i].name[0]) && (!memcmp(plist[i].name, ptopic->name, ptopic->len))) {
			return i;
		}
	}
	return -1;
}
