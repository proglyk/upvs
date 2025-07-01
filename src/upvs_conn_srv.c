#include "upvs_conf.h"
#include "upvs_conn_srv.h"
#include "lwip/err.h"
#include "net_if.h"

typedef struct conn_cxt_st ctx_t;

// интерфейсные
static void *conn_init(sess_init_cb_ptr_t, net_if_data_t *, void*);
static signed long conn_do(void*);
static void conn_del(sess_del_cb_ptr_t, void*);
//
static void TimerTick(xTimerHandle, void *);
static const char *get_err_str(s16_t);
static err_enum_t	receive( s32_t, s32_t, u8_t *, u32_t, u32_t *, 
                           struct sockaddr *, socklen_t * );
static err_enum_t	transmit( s32_t, u8_t *, u32_t, u32_t *,
                            struct sockaddr *, socklen_t, void * );

// привязки
net_if_fn_t xFnSrvUpvs = {
  .pvUpperInit =   (upper_init_ptr_t)NULL/* upvs_srv_init */,
  .pvUpper =       (void *)NULL/* &xBroker */,
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
  int timeval = 10000;
  if (!pdata) return NULL;
  
  ctx = malloc(sizeof(ctx_t));
  if (!ctx) return NULL;
  
  ctx->pxMqtt = mqtt_srv__create();
  if (!ctx->pxMqtt) return NULL;
  ctx->pxMqtt->pvCtx = (void *)ctx;
  // Инициализируем остальные уровни
  if (mqtt_srv__init(ctx->pxMqtt, (send_ptr_t)transmit) < 0) return NULL;
  
  // сохраняем сокет и настраиваем его на работу в НЕблокирующем режиме
  ctx->slSock = pdata->slSock;
  if (setsockopt(ctx->slSock, SOL_SOCKET, SO_RCVTIMEO, &timeval, sizeof(timeval))) {
    return NULL;
  }
  
  // Создаем таймер чтения внешних команд (со стороны ПК стенда)
  //ctx->xTimer = xTimerCreate("timer", 10 / portTICK_RATE_MS, pdTRUE, 0,  
  //  TimerTick, (void *)ctx);
  
  //xTimerStart(ctx->xTimer, 0);
  
  DBG_PRINT( NET_DEBUG, ("Sess created (sock=%d), in '%s' /UPVS/upvs_srv_sess.c:%d\r\n", 
    ctx->slSock, __FUNCTION__, __LINE__) );
  
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
  err_enum_t err;
  u32_t recvd;
  ctx_t *ctx = (ctx_t *)argv;
  
  // Ожидаем приема нового байта. Блокируемся.
  err = receive( SOCK_STREAM, ctx->slSock, ctx->pxMqtt->xData.rx.pcBuf,
                 DECODER_BUFSIZE_RX, &recvd, NULL, NULL );
  if ( err != ERR_OK) {
    goto exit; // Не знаю как обрабатывать
	}
  
  err = mqtt_srv__input(ctx->pxMqtt, recvd);
  if (err != ERR_OK) {
    goto exit; // Не знаю как обрабатывать
  } 
  
	return 0;
  
  exit:
  LWIP_DEBUGF( NET_DEBUG, ("Breaking recv ('%s'), in '%s' /UPVS/"             \
    "upvs_srv_sess3.c:%d\r\n", get_err_str(err), __FUNCTION__, __LINE__) );
  return -1; //FIXME возвращаю 0 временно.
}

/**	----------------------------------------------------------------------------
	* @brief ??? */
static void
  conn_del(sess_del_cb_ptr_t pFn, void *argv) {
/*----------------------------------------------------------------------------*/
  ctx_t* ctx = (ctx_t *)argv;
  
  if (!ctx) return;
  if (!ctx->pxMqtt) return;
   
  // Удалить дочернюю задачу
  //if (ctx->pvPrintTask) vTaskDelete(ctx->pvPrintTask);
  
  mqtt_srv__del(ctx->pxMqtt);
  
  // выполнить обратную связь
  if (pFn) pFn();
  
  //LWIP_DEBUGF( NET_DEBUG, ("Shell sess 's=%d' closed, in '%s' /SHELL/shell_sess.c:%d\r\n", 
  //  ctx->xTS.sock, __FUNCTION__, __LINE__) );
  
  free(ctx);
}

/**	----------------------------------------------------------------------------
	* @brief ??? */
static void
  TimerTick(xTimerHandle xTimer, void *argv) {
/*----------------------------------------------------------------------------*/
  
}

/**	----------------------------------------------------------------------------
	* @brief Функция чтения буфера входных данных LwIP с блокировкой
	* @param sock: Сокет подключения.
	* @param pBuf: Указатель на массив куда считывать.
	* @param len: Запрошенная длина массива байт для чтения.
	* @param received: Число успешно прочитанных из общего запрошенного числа байт.
	* @retval error: Статус выполнения функции. */
static err_enum_t
	receive( s32_t type, s32_t sock, u8_t* pBuf, u32_t len, u32_t* recvd,
           struct sockaddr* pTo, socklen_t* pTolen ) {
/*----------------------------------------------------------------------------*/
	s32_t bytes;

  // SOCK_STREAM
  if (type == 1) {
    // Ожидаем приема нового пакета. Блокируемся.
    bytes = lwip_recv(sock, pBuf, len, 0);
  }
  // SOCK_DGRAM
  else if (type == 2) {
    // Ожидаем приема нового пакета. Блокируемся.
    bytes = lwip_recvfrom(sock, pBuf, len, 0, (struct sockaddr *)pTo, pTolen);
  }
  else {
    return ERR_VAL;
  }	
	// Условия проверки статуса выполнения lwip_recvfrom
	// Случай вынимания разъема RJ45. Тогда приходит bytes=-1.
	if (bytes < 0)
		return ERR_TIMEOUT;
	// Случай отключения клиента. Тогда приходит bytes=0.
	if (bytes == 0)
		return ERR_CLSD;
	// Копируем считанную длину
  if (recvd != NULL)
    *recvd = bytes;
	return ERR_OK;
}

/**	----------------------------------------------------------------------------
	* @brief Функция записи в буфер выходных данных LwIP
	* @param sock: Сокет подключения.
	* @param pBuf: Указатель на массив откуда брать для записи.
	* @param len: Размер массива байт.
	* @param written: Число успешно записанных из общего числа байт.
	* @retval error: Статус выполнения функции. */
static err_enum_t
	transmit( s32_t type, u8_t *pBuf, u32_t len, u32_t *written, 
            struct sockaddr *to, socklen_t tolen, void *pvPld ) {
/*----------------------------------------------------------------------------*/
	s32_t bytes;
  ctx_t *ctx = (ctx_t *)pvPld;
  if (!ctx) return ERR_VAL;
  
  // SOCK_STREAM
  if (type == 1) {
    // Ожидаем приема нового пакета. Блокируемся.
    bytes = lwip_send(ctx->slSock, pBuf, len, 0);
  }
  // SOCK_DGRAM
  else if (type == 2) {
    // Ожидаем приема нового пакета. Блокируемся.
    bytes = lwip_sendto(ctx->slSock, pBuf, len, 0, (const struct sockaddr *)to, tolen);
  }
  else {
    return ERR_VAL;
  }
	
	
	// Условия проверки статуса выполнения lwip_recvfrom
	// Случай вынимания разъема RJ45. Тогда приходит bytes=-1.
	if (bytes < 0)
		return ERR_TIMEOUT;
	// Случай отключения клиента. Тогда приходит bytes=0.
	if (bytes == 0)
		return ERR_CLSD;

	// Копируем считанную длину
  if (written != NULL)
    *written = bytes;
	
	return ERR_OK;
}

/**	----------------------------------------------------------------------------
	* @brief ??? */
static const char *
  get_err_str(s16_t code) {
/*----------------------------------------------------------------------------*/
  u8_t *str = NULL;
  
  switch (code) {
    case  ERR_OK:       str = (u8_t *)"CON OK"; break;
    case  ERR_CLSD:     str = (u8_t *)"CON CLSD"; break;
    case  ERR_TIMEOUT:  str = (u8_t *)"CON TIMEOUT"; break;
    case  ERR_VAL:      str = (u8_t *)"ILLEGAL VALUE"; break;
    default:            str = (u8_t *)"UNK"; break;
  }
  return (const char *)str;
}