#include "upvs_conf.h"
#include "upvs_mqtt_clt.h"
#include "net_if.h"
#include <string.h>
#include "dbg.h"

typedef struct {
//s32_t        slSock;  // Сокет подключения
  mqtt_clt_t *pxMqtt;  // Экземпляр сервера MQTT
//xTimerHandle xTimer;  // программный таймер для исходящих сообщ
  s32_t slCountAlive;
  s32_t slCountItem;
  s32_t slCountNew;
  u8_t acPath[UPVS_TOPICPATH_SIZE];
  u8_t value[UPVS_VALUE_SIZE];
  u8_t acError[UPVS_CLT_ERROR_SIZE];
} ctx_t;

// public


// static
// интерфейсные
static void *conn_init(sess_init_cb_ptr_t, net_if_data_t *, void*);
static signed long conn_do(void*);
static void conn_del(sess_del_cb_ptr_t, void*);
static s32_t accept_new_error(ctx_t *, u32_t);
static s32_t accept_param_change(ctx_t *);
static s32_t accept_request_get_all(ctx_t *);

// привязки
net_if_fn_t xFnCltUpvs = {
  .pvUpperInit =   (upper_init_ptr_t)NULL,
  .pvUpper =       (void *)NULL,
  .ppvSessInit =   (sess_init_ptr_t)conn_init,
  .ppvSessInitCb = (sess_init_cb_ptr_t)NULL,
  .pslSessDo =     (sess_do_ptr_t)conn_do,
  .pvSessDel =     (sess_del_ptr_t)conn_del,
  .pvSessDelCb =   (sess_del_cb_ptr_t)NULL,
};


// Функции, доступные извне

/**	----------------------------------------------------------------------------
	* @brief Connection init */
static void *
  conn_init(sess_init_cb_ptr_t pFn, net_if_data_t *pdata, void* pvPld) {
/*----------------------------------------------------------------------------*/
  ctx_t *ctx;
  u32_t rcvt = UPVS_SOCK_RCVTIME;
  if (!pdata) return NULL;
  
  ctx = malloc(sizeof(ctx_t));
  if (!ctx) return NULL;
  //ctx->pxUpvs = upvs_clt__create();
  //if (!ctx->pxUpvs) return NULL;
  ctx->pxMqtt = upvs_mqtt_clt__create();
  if (!ctx->pxMqtt) return NULL;
  
  // оставляем сокет в блокирующем режиме - для клента это не имеет значения
  if (upvs_mqtt_clt__init(ctx->pxMqtt, pdata->slSock) < 0) return NULL;
    
  //FIXME
  ctx->slCountItem = UPVS_CLT_CNT_INIT; //27;
  ctx->slCountNew = UPVS_CLT_CNT_INIT;
  //pctx->state = CLT_BUSY;
  //pctx->error = false; //FIXME сейчас поле error дублируется в других местах
  
  DBG_PRINT( NET_DEBUG, ("Conn created (sock=%d), in '%s' /UPVS2/upvs_clt_conn.c:%d\r\n", 
                        pdata->slSock, __FUNCTION__, __LINE__) );
  
  // возвращаем указат для дальнейшего исп.-ния
  return (void *)ctx;
}

/**	----------------------------------------------------------------------------
	* @brief 
	* @param sock: Сокет подключения.
	* @param ptr: Указатель на упр.структуру. //TODO Надо новый 
	* @retval error: Статус выполнения. */
static signed long
  conn_do(void* argv) {
/*----------------------------------------------------------------------------*/
  //err_enum_t err;
  //u32_t recvd;
  s32_t rc;
  ctx_t *ctx = (ctx_t *)argv;
  
  // проверяем данные с обмена по CAN на наличие аварийных флагов
  //if_upvs__check_err(pclt);
  
  // передаем очередной параметр
  for (u32_t i = 0; i < UPVS_ERR_LIST_LENGHT; i++) {
    if (upvs_clt__is_err_new(ctx->pxMqtt->pxUpvs, i)) {
      accept_new_error(ctx, i);
      // после отправки очищаем флаг bNew.
      upvs_clt__set_err_new(ctx->pxMqtt->pxUpvs, i, false);
      // Не трогаем ulCode и slValue если авария "Активна" и зачищаем их если
      // авария сброшена
      if (upvs_clt__is_err_act(ctx->pxMqtt->pxUpvs, i)) {
        DBG_PRINT( NET_DEBUG, ("Error (%d) is active, in '%s' /UPVS/upvs_clt_sess.c:%d\n", 
          upvs_clt__get_err_code(ctx->pxMqtt->pxUpvs, i), __FUNCTION__, __LINE__) );
      }
      else {
        DBG_PRINT( NET_DEBUG, ("Error (%d) is inactive, in '%s' /UPVS/upvs_clt_sess.c:%d\n", 
          upvs_clt__get_err_code(ctx->pxMqtt->pxUpvs, i), __FUNCTION__, __LINE__) );
        upvs_clt__reset_err(ctx->pxMqtt->pxUpvs, i);
      }
    }
  }
  
  // забор даных с обмена по CAN
  if (ctx->slCountNew == UPVS_CLT_CNT_INIT)
    // FIXME if_upvs__update(pparam); //upvs_param_clt__update(pparam);
  // отправка
  if (upvs_clt__get_prm_new(ctx->pxMqtt->pxUpvs, ctx->slCountNew)) {
    // обрабатываем изменение
    rc = accept_param_change(ctx);
    if (rc < 0) {
      DBG_PRINT( NET_DEBUG, ("Can't publish change to %s, in '%s' /UPVS2/upvs_clt_conn.c:%d\r\n", 
        ctx->acPath, __FUNCTION__, __LINE__) );
    }
    // затираем флаг для любого статуса исполнения 'accept_param_change'
    upvs_clt__edit_prm_new(ctx->pxMqtt->pxUpvs, ctx->slCountNew, false);
  }
  
  // Передача списка Параметров по запросу СКДУ
  if (ctx->pxMqtt->pxUpvs->bReqSendAll) {
    // опрос данных с CAN
    // FIXME if_upvs__update(pparam);
    //
    rc = accept_request_get_all(ctx);
    if (rc < 0) {
      // сбрасываем флаг запроса 'get_all' - оч сомнительная операция FIXME
      ctx->pxMqtt->pxUpvs->bReqSendAll = false;
      // Скидываем счетчик до индекса начального параметра - тоже вопрос FIXME
      ctx->slCountItem = UPVS_CLT_CNT_INIT;
      DBG_PRINT( NET_DEBUG, ("Can't get/publish param %s, in '%s' /UPVS2/upvs_clt_conn.c:%d\r\n", 
        ctx->acPath, __FUNCTION__, __LINE__) );
    }
    // ставим флаг FIXME
    //pctx->status |= STA_PUBLISHED;
    //считает только при наличии команды
    if (ctx->slCountItem < UPVS_CLT_CNT_MAX) //FIXME
      ctx->slCountItem += 1;
    else {
      ctx->pxMqtt->pxUpvs->bReqSendAll = false;
      ctx->slCountItem = UPVS_CLT_CNT_INIT;
    }
  }

  // считает всегда
  if (ctx->slCountNew < UPVS_CLT_CNT_MAX)
    ctx->slCountNew += 1;
  else
    ctx->slCountNew = UPVS_CLT_CNT_INIT;
  
	vTaskDelay(250);
	return 0;
}

/**	----------------------------------------------------------------------------
	* @brief ??? */
static void
  conn_del(sess_del_cb_ptr_t pFn, void *argv) {
/*----------------------------------------------------------------------------*/
  ctx_t* ctx = (ctx_t *)argv;
  
  if (!ctx) return;
  //if (!ctx->pxUpvs) return;
   
  // Удалить дочернюю задачу
  //if (ctx->pvPrintTask) vTaskDelete(ctx->pvPrintTask);
  
  //mqtt_srv__del(ctx->pxMqtt);
  
  // выполнить обратную связь
  if (pFn) pFn();
  
  //LWIP_DEBUGF( NET_DEBUG, ("Shell sess 's=%d' closed, in '%s' /SHELL/shell_sess.c:%d\r\n", 
  //  ctx->xTS.sock, __FUNCTION__, __LINE__) );
  
  upvs_mqtt_clt__del(ctx->pxMqtt);
  free(ctx);
}

/**	----------------------------------------------------------------------------
	* @brief ??? */
static s32_t
  accept_new_error(ctx_t *ctx, u32_t idx) {
/*----------------------------------------------------------------------------*/
  s32_t rc;
  
  // готовим буфер передачи
  memset(ctx->acPath, '\0', sizeof(ctx->acPath));
  memset(ctx->acError, '\0', sizeof(ctx->acError));
  // берем ошибку
  rc = upvs_clt__get_err( ctx->pxMqtt->pxUpvs, idx, ctx->acPath, ctx->acError );
  if (rc < 0) return -1;
  // Берем имя топика и шлём (Publish) брокеру полученное сообщение
  rc = upvs_mqtt_clt__send( ctx->pxMqtt, (const u8_t *)ctx->acPath, 
                            (const u8_t *)ctx->acError );
  if (rc) return -1;
  
  return 0;
}

/**	----------------------------------------------------------------------------
	* @brief ??? */
static s32_t
  accept_param_change(ctx_t *ctx) {
/*----------------------------------------------------------------------------*/
  s32_t rc = 0;
  // готовим буфер передачи
  memset(ctx->acPath, '\0', sizeof(ctx->acPath));
  memset(ctx->value, '\0', sizeof(ctx->value));
  // передаем очередной параметр
  //rc = upvs_clt__param_get(upvs_clt__inst(), pctx->slCountNew, acPath,
  //                           value, sizeof(value));
  if (rc < 0) return -1;
  
  // Берем имя топика и шлём (Publish) брокеру полученное сообщение
  rc = upvs_mqtt_clt__send( ctx->pxMqtt, (const u8_t *)ctx->acPath, 
                            (const u8_t *)ctx->value );
  if (rc < 0) return -1;
  
  return 0;
}

/**	----------------------------------------------------------------------------
	* @brief ??? */
static s32_t
  accept_request_get_all(ctx_t *ctx) {
/*----------------------------------------------------------------------------*/
  s32_t rc = 0;
// готовим буфер передачи
  memset(ctx->acPath, '\0', sizeof(ctx->acPath));
  memset(ctx->value, '\0', sizeof(ctx->value));
  // передаем очередной параметр
  //rc = upvs_clt__param_get(upvs_clt__inst(), pctx->slCountItem, acPath,
  //                           value, sizeof(value));
  if (rc < 0) return -1;
  // Берем имя топика и шлём (Publish) брокеру полученное сообщение
  rc = upvs_mqtt_clt__send( ctx->pxMqtt, (const u8_t *)ctx->acPath, 
                            (const u8_t *)ctx->value );
  if (rc < 0) return -1;
  
  return 0;
}
