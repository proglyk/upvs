#include "upvs_conf.h"
#include "upvs_mqtt_clt.h"
#include "net_if.h"
#include <string.h>
#include "dbg.h"
#include "rtc_usec.h"

// Определение типов

// Тип упр. структуры
typedef struct {
  // локальные
  s32_t slCountAlive;
  u32_t ulCountItem;
  u32_t ulCountNew;
  u8_t  acPath[UPVS_TOPICPATH_SIZE];
  u8_t  value[UPVS_VALUE_SIZE];
  u8_t  acError[UPVS_CLT_ERROR_SIZE];
  bool  bSrvConn;
  // взаимодействие с верхними уровнями
  mqtt_clt_t *pxMqtt; // Экземпляр объекта (клиент MQTTClient)
} ctx_t;

// Объявления функций

// Взаимодейтсвие с юриной частью
extern void if_upvs__update(void);
extern int  if_upvs__check_err(void);
// Создание, инициализация и удаление экземпляра упр. структуры
static void *conn_init(sess_init_cb_ptr_t, net_if_data_t *, void*);
static signed long conn_do(void*);
static void  conn_del(sess_del_cb_ptr_t, void*);
// Функции передачи по сети
static s32_t publish_err(ctx_t *, u32_t);
static s32_t publish_prm(ctx_t *, u32_t);

// Переменные

// Реализация интерфейса net_if
net_if_fn_t xFnCltUpvs = {
  .pvUpperInit =   (upper_init_ptr_t)NULL,
  .pvUpper =       (void *)NULL,
  .ppvSessInit =   (sess_init_ptr_t)conn_init,
  .ppvSessInitCb = (sess_init_cb_ptr_t)NULL,
  .pslSessDo =     (sess_do_ptr_t)conn_do,
  .pvSessDel =     (sess_del_ptr_t)conn_del,
  .pvSessDelCb =   (sess_del_cb_ptr_t)NULL,
};

// Определения функций

// Функции создания, инициализации и удаления экземпляра упр. структуры

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
  if (upvs_mqtt_clt__init(ctx->pxMqtt, pdata->slSock) < 0) {
    DBG_PRINT( NET_DEBUG, ("Can't connect to remote, in '%s' /UPVS2/upvs_clt_conn.c:%d\r\n", 
     __FUNCTION__, __LINE__) );
    return NULL;
  }
  //ctx->bSrvConn = true;
    
  //FIXME
  ctx->ulCountItem = UPVS_CLT_CNT_INIT; //27;
  ctx->ulCountNew = UPVS_CLT_CNT_INIT;
  //pctx->state = CLT_BUSY;
  //pctx->error = false; //FIXME сейчас поле error дублируется в других местах
  
  DBG_PRINT( NET_DEBUG, ("%s: Conn created (sock=%d), in '%s' /UPVS2/upvs_clt_conn.c:%d\r\n", 
             rtc__print_str(), pdata->slSock, __FUNCTION__, __LINE__) );
  
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
  s32_t rc;
  ctx_t *ctx = (ctx_t *)argv;
  mqtt_clt_t *mqtt;
  
  // проверка арг-тов
  if (!ctx) return -1;
  // используем mqtt для краткости
  mqtt = ctx->pxMqtt;
  
  // проверка на наличие соединения
  if (!upvs_mqtt_clt__is_conntd(mqtt)) goto exit;
  
  // проверяем данные с обмена по CAN на наличие аварийных флагов
  if_upvs__check_err();
  
  // передаем очередной параметр
  for (u32_t i = 0; i < UPVS_ERR_LIST_LENGHT; i++) {
    if (upvs_clt__is_err_new(clt_inst(mqtt), i)) {
      //accept_new_error(ctx, i);
      rc = publish_err(ctx, i);
      if (rc < 0) {
        DBG_PRINT( NET_DEBUG, ("Can't get error desc '%s', in '%s' /UPVS2/upvs_clt_conn.c:%d\r\n", 
          ctx->acError, __FUNCTION__, __LINE__) );
      }
      // после отправки очищаем флаг bNew.
      upvs_clt__set_err_new(clt_inst(mqtt), i, false);
      // Не трогаем ulCode и slValue если авария "Активна" и зачищаем их если
      // авария сброшена
      if (upvs_clt__is_err_act(clt_inst(mqtt), i)) {
        DBG_PRINT( NET_DEBUG, ("Error (%d) is active, in '%s' /UPVS2/upvs_clt_conn.c:%d\n", 
          upvs_clt__get_err_code(clt_inst(mqtt), i), __FUNCTION__, __LINE__) );
      }
      else {
        DBG_PRINT( NET_DEBUG, ("Error (%d) is inactive, in '%s' /UPVS2/upvs_clt_conn.c:%d\n", 
          upvs_clt__get_err_code(clt_inst(mqtt), i), __FUNCTION__, __LINE__) );
        upvs_clt__reset_err(clt_inst(mqtt), i);
      }
    }
  }
  
  // забор данных с обмена по CAN
  if (ctx->ulCountNew == UPVS_CLT_CNT_INIT) {
    if_upvs__update(); //upvs_param_clt__update(pparam);
  }
  // Проверка на наличие изменений в значениях параметров
  if (upvs_clt__get_prm_new(clt_inst(mqtt), ctx->ulCountNew)) {
    // обрабатываем изменение
    rc = publish_prm(ctx, ctx->ulCountNew);
    if (rc < 0) {
      DBG_PRINT( NET_DEBUG, ("Can't publish change to %s, in '%s' /UPVS2/upvs_clt_conn.c:%d\r\n", 
        ctx->acPath, __FUNCTION__, __LINE__) );
    }
    // затираем флаг для любого статуса исполнения 'accept_param_change'
    upvs_clt__edit_prm_new(clt_inst(mqtt), ctx->ulCountNew, false);
  }
  
   // Проверка на наличие запроса 'get_all' со стороны брокера (сервера)
  if (clt_inst(mqtt)->bReqSendAll) {
    // опрос данных с CAN
    if_upvs__update();
    //
    rc = publish_prm(ctx, ctx->ulCountItem);
    if (rc < 0) {
      // сбрасываем флаг запроса 'get_all' - оч сомнительная операция FIXME
      clt_inst(mqtt)->bReqSendAll = false;
      // Скидываем счетчик до индекса начального параметра - тоже вопрос FIXME
      ctx->ulCountItem = UPVS_CLT_CNT_INIT;
      DBG_PRINT( NET_DEBUG, ("Can't get/publish param %s, in '%s' /UPVS2/upvs_clt_conn.c:%d\r\n", 
        ctx->acPath, __FUNCTION__, __LINE__) );
    }
    // ставим флаг FIXME
    //mqtt->ucSta |= STA_PUBLISHED;
    //считает только при наличии команды
    if (ctx->ulCountItem < UPVS_CLT_CNT_MAX) //FIXME
      ctx->ulCountItem += 1;
    else {
      clt_inst(mqtt)->bReqSendAll = false;
      ctx->ulCountItem = UPVS_CLT_CNT_INIT;
    }
  }
  
  // FIXME Test
/*   if (ctx->ulCountNew == UPVS_CLT_CNT_INIT) {
    rc = publish_prm(ctx, UPVS_CLT_CNT_INIT);
    if (rc < 0) {
      //DBG_PRINT( NET_DEBUG, ("Can't publish change to %s, in '%s' /UPVS2/upvs_clt_conn.c:%d\r\n", 
      //  ctx->acPath, __FUNCTION__, __LINE__) );
      DBG_PRINT( NET_DEBUG, ("%s: Can't publish change to %s, in '%s' /UPVS2/upvs_clt_conn.c:%d\r\n", 
        rtc__print_str(), ctx->acPath, __FUNCTION__, __LINE__) );
    }
  } */
  // считает всегда
  if (ctx->ulCountNew < UPVS_CLT_CNT_MAX)
    ctx->ulCountNew += 1;
  else
    ctx->ulCountNew = UPVS_CLT_CNT_INIT;
  
	vTaskDelay(250);
	return 0;
  
  exit:
  // выход в случае потери связи
  return -1;
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

// Функции передачи по сети

/**	----------------------------------------------------------------------------
	* @brief ??? */
static s32_t
  publish_err(ctx_t *ctx, u32_t idx) {
/*----------------------------------------------------------------------------*/
  s32_t rc;
  
  // готовим буфер передачи
  memset(ctx->acPath, '\0', sizeof(ctx->acPath));
  memset(ctx->acError, '\0', sizeof(ctx->acError));
  // берем ошибку
  rc = upvs_clt__get_err(clt_inst(ctx->pxMqtt), ctx->acPath, ctx->acError, idx);
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
  publish_prm(ctx_t *ctx, u32_t idx) {
/*----------------------------------------------------------------------------*/
  s32_t rc = 0;
  // готовим строки под размещение топика и значения параметра
  memset(ctx->acPath, '\0', sizeof(ctx->acPath));
  memset(ctx->value, '\0', sizeof(ctx->value));
  // по индексу idx получаем искомый парамтер и заполняем строки
  rc = upvs_clt__get_prm(clt_inst(ctx->pxMqtt), ctx->acPath, ctx->value, 
                         sizeof(ctx->value), idx);
  if (rc < 0) return -1;
  // Полученные имя топика и значение шлём (Publish) брокеру
  rc = upvs_mqtt_clt__send( ctx->pxMqtt, (const u8_t *)ctx->acPath, 
                            (const u8_t *)ctx->value );
  if (rc < 0) return -1;
  
  return 0;
}
