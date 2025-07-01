#ifndef _UPVS_MQTT_SRV_H_
#define _UPVS_MQTT_SRV_H_

#include "userint.h"
#include "lwip/err.h"
#include "lwip/sockets.h"
#include "upvs_srv.h"

/*configurations*/
// #define MAX_FRAME_SIZE                  (240)
// #define DEFAULT_BROKER_TIMEOUT  	      (100)
// #define ADDR_SIZE					              (4)
// #define PROTO_LEVEL_MQTT311 		        (4)
// #define MAX_PLD_SIZE				            (128)
#define MAX_SUBS_TOPIC  			          (8)
#define MAX_TOPIC_NAME_SIZE             (32)
#define MAX_WILL_MSG_SIZE			          (32)
#define MAX_USR_NAME_SIZE			          (32)
#define MAX_PSWD_NAME_SIZE			        (32)
#define MAX_ID_SIZE                     (32)
// #define NOT_FOUND					              (255)
#define MAX_CONN_CLIENTS			          (1)

// ack flags
#define SUB_ACK_FAIL				            (80)

// Директивы Message types

#define CONNECT                         0x10
#define CONNACK                         0x20
#define PUBLISH                         0x30
#define PUBACK                          0x40
#define PUBREC                          0x50
#define PUBREL                          0x60
#define PUBCOMP                         0x70
#define SUBSCRIBE                       0x80
#define SUBACK                          0x90
#define UNSUBSCRIBE                     0xA0
#define UNSUBACK                        0xB0
#define PINGREQ                         0xC0
#define PINGRESP                        0xD0
#define DISCONNECT                      0xE0

// Директивы Версии

// For version 3 of the MQTT protocol
//#define PROTOCOL_NAME     "MQIsdp"
//#define PROTOCOL_VERSION  0x83  //3 почему-то выдает 0x83 вместо 3
// For version 3.1.1 of the MQTT protocol
#define PROTOCOL_NAME                   "MQTT"
#define PROTOCOL_VERSION                (0x04)  //3 выдает 0x83 вместо 3

// Директивы Не помню

// only for CONNACK V3.1.0
// #define ACCEPTED                        (0x00UL)
// #define UNVERSION                       (0x01UL)
// #define IDREJECT                        (0x02UL)
// #define UNAVAILABLE                     (0x03UL)
// only for CONNACK V3.1.1
#define CONN_ACK_OK                     (0)
#define CONN_ACK_BAD_PROTO              (1)
#define CONN_ACK_BAD_ID                 (2)
#define CONN_ACK_NOT_AVBL               (3)
#define CONN_ACK_AUTH_MALFORM           (4)
#define CONN_ACK_BAD_AUTH               (5)
#define CONN_ACK_OK_SESS_PRESENT        (0)


// Macros for accessing the MSB and LSB of a uint16_t
#define MQTT_MSB(A) (u8_t)((A & 0xFF00) >> 8)
#define MQTT_LSB(A) (u8_t)(A & 0x00FF)

// Macro. работа с буфером RX

#define BUF_READ_PTR(_PBUF_, _PTR_, _STEP_)                         \
  do {                                                              \
    (_PTR_) = (u8_t*)(_PBUF_->rx.pcBuf + _PBUF_->rx.usCar);         \
    _PBUF_->rx.usCar += (_STEP_);                                   \
  } while(0)
   
#define BUF_READ_BYTE1(PBUF, BYTE, INCREMENT)                                 \
  do {                                                                        \
    (*BYTE) = (u8_t)(PBUF->rx.pcBuf[PBUF->rx.usCar]);                         \
    PBUF->rx.usCar += (u16_t)INCREMENT;                                       \
  } while(0)


#define BUF_READ_WORD(_PBUF_, _WORD_)                               \
  do {                                                              \
    (*_WORD_) = (u16_t)((_PBUF_->rx.pcBuf[_PBUF_->rx.usCar] << 8)   \
      + _PBUF_->rx.pcBuf[_PBUF_->rx.usCar+1]);                      \
    _PBUF_->rx.usCar += 2;                                          \
  } while(0)

// Макрос для работы с буфером TX

#define BUF_WRITE_BYTE(_PBUF_, _BYTE_)                              \
  do {                                                              \
    _PBUF_->tx.pcBuf[_PBUF_->tx.usCar++] = (u8_t)_BYTE_;            \
  } while(0)

#define BUF_WRITE_WORD(_PBUF_, _WORD_)                              \
  do {                                                              \
    _PBUF_->tx.pcBuf[_PBUF_->tx.usCar++] = (u8_t)MQTT_MSB(_WORD_);  \
    _PBUF_->tx.pcBuf[_PBUF_->tx.usCar++] = (u8_t)MQTT_LSB(_WORD_);  \
  } while(0)                                                        \


// Исходящие сообщ

typedef enum {
	MSG_SIMPLE,
	MSG_MID,
	MSG_PAYLOAD,
  MSG_PUBLISH
} outmsg_t;


// Разбор входящ сообщений

// CONNECT
typedef union {
	u8_t all;
	struct {
    u8_t reserved 	      :1;
    u8_t cleans_session   :1;
    u8_t last_will        :1;
    u8_t will_qos         :2;
    u8_t will_retain      :1;
    u8_t pswd             :1;
    u8_t user_name        :1;
  }	bit;
} conn_flags_t;

typedef struct {
  struct {
    struct {
      u8_t  ctrl;
      u32_t rem_len;
    } fix;
    struct {
      u16_t proto_len;
      u8_t  *proto_name;
      u8_t  proto_ver;
      conn_flags_t flags;
      u16_t keep_alive;
    } var;
  } head;
  struct {
    u16_t   client_id_len;
    u8_t    *client_id;
    u16_t   will_topic_len;
    u8_t    *will_topic;
    u16_t   will_msg_len;
    u8_t    *will_msg;
    u16_t   usr_name_len;
    u8_t    *usr_name;
    u16_t   pswd_len;
    u8_t    *pswd;
  } payload;
} con_msg_t;

typedef union {
	u8_t 					all;
	struct {
    u8_t retain :1;
    u8_t QoS    :2;
    u8_t dup    :1;
    u8_t type   :4;
  }	bit;
} pub_ctrl_t;

// PUBLISH
typedef struct {
  struct {
    struct {
      pub_ctrl_t ctrl;
      u32_t rem_len;
    } fix;
    struct {      
      u16_t topic_len;
      u8_t *topic_name;
      u16_t packet_id;
    } var;
  } head;
  u8_t *payload;
} pub_msg_t;

typedef struct{
	u16_t len;
	u8_t *name;
	u8_t  qos;
} sub_topic_ptr_t, unsub_topic_ptr_t;

// SUBSCRIBE, UNSUBSCRIBE
typedef struct {
  struct {
    struct {
      u8_t  ctrl;
      u32_t rem_len;
    } fix;
    struct {      
      u16_t packet_id;
    } var;
  } head;
  sub_topic_ptr_t axTopicPtrList[MAX_SUBS_TOPIC];
} sub_unsub_msg_t;

typedef struct {
  con_msg_t       con;
  pub_msg_t       pub;
  sub_unsub_msg_t sub;
  sub_unsub_msg_t unsub;
} decoder_t;

/*---------broker-start---------------*/
typedef struct{
	u16_t len;
	char  name[MAX_TOPIC_NAME_SIZE];
	u8_t  qos;
} sub_topic_t;

typedef struct {
	//sockaddr_t sockaddr;
	char  id[MAX_ID_SIZE];
	u16_t keepalive;
	char  username[MAX_USR_NAME_SIZE];
	char  password[MAX_PSWD_NAME_SIZE];
	bool  last_will;
	char  will_topic[MAX_TOPIC_NAME_SIZE];
	char  will_msg[MAX_WILL_MSG_SIZE];
	u8_t  will_qos;
	u8_t  will_retain;
	sub_topic_t axTopicList[MAX_SUBS_TOPIC];
	bool  connected;
}  tb_client_t;
/*---------broker-end-----------------*/

// Буфер

typedef struct {
  u16_t usSize;
  u16_t usCar;
  u8_t *pcBuf; //DECODER_BUFSIZE_TX
} buf_t;

typedef struct {
  buf_t rx;
  buf_t tx;
} data_t;

typedef	err_enum_t (*send_ptr_t)( s32_t, u8_t *, u32_t, u32_t *, 
                                  struct sockaddr *, socklen_t, void * );

	// sock_send(s32_t type, s32_t sock, u8_t *pBuf, u32_t len, u32_t *written, 
    // struct sockaddr *to, socklen_t tolen)

// Базовая управляющая структура

typedef struct {
  data_t       xData;             //
  char         id[MAX_ID_SIZE];  // Идентиф. клиента, уник. для разных сессий
  tb_client_t *pxClients;        // указатель на список подл.клиентов
  decoder_t    xDecod;           // Разбор вход сообщений
  upvs_srv_t  *pxUpvs;           // 
  send_ptr_t   psSendCb;          //
  void        *pvCtx;           //  в кач-ве payload исп. "родитель" ctx_t
} mqtt_srv_t;

void       *mqtt_srv__create(void);
int         mqtt_srv__init(mqtt_srv_t *, send_ptr_t);
err_enum_t  mqtt_srv__input(mqtt_srv_t *, u32_t);
void        mqtt_srv__del(mqtt_srv_t *);

#endif //_UPVS_MQTT_SRV_H_