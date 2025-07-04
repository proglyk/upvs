#include "upvs_clt.h"
#include "upvs_prm.h"
#include <string.h>
#include <stdlib.h>
#include "dbg.h"
#include "cJSON.h"


// Константы
static const char* pcText = "АО";

static int param_set( upvs_clt_t *, const u8_t *, u32_t, const u8_t * );
static int typecast(struct cJSON *, value_ptr_t *);

// Общедоступные (public) функции

// Функции создания, инициализации, удаления экземпляра упр. структуры

/**	----------------------------------------------------------------------------
	* @brief ??? */
void *
  upvs_clt__create(void) {
/*----------------------------------------------------------------------------*/
  upvs_clt_t *self = malloc(sizeof(upvs_clt_t));
  if (!self) return NULL;
  self->pxPrm = upvs_prm__create();
  if (!self->pxPrm) return NULL;
  self->pxErr = upvs_err__create();
  if (!self->pxErr) return NULL;
  return (void *)self;
}

/**	----------------------------------------------------------------------------
	* @brief ??? */
int
  upvs_clt__init(upvs_clt_t *self) {
/*----------------------------------------------------------------------------*/
  if (!self) return -1;
  if (upvs_prm__init(self->pxPrm) < 0) return -1;
  if (upvs_err__init(self->pxErr) < 0) return -1;
  return 0;
}

/**	----------------------------------------------------------------------------
	* @brief */
void
  upvs_clt__del(upvs_clt_t *self) {
/*----------------------------------------------------------------------------*/
  if (!self) return;
  upvs_prm__del(self->pxPrm);
  upvs_err__del(self->pxErr);
  free(self);
  self = NULL;
}

// Основные функции клиентской части

/**	----------------------------------------------------------------------------
	* @brief ??? */
int
  upvs_clt__set( upvs_clt_t *self, const u8_t *pcPath, u32_t path_len,
                 const u8_t *pPld, u32_t pld_len ) {
/*----------------------------------------------------------------------------*/
  static const char* pcRootPath = "action/CSC";
  static u8_t acStrAll[4];
  //upvs_clnt_t *pclt = upvs_clt__inst();
  
  // проверка арг-ов
  if ((!pcPath) || (!pPld)) return -1;

  //
  if (!strcmp((const char *)pcPath, pcRootPath)) {
    if (pld_len < 3) return -1;
    memcpy( (void *)acStrAll,(const void *)pPld, 3);
    acStrAll[3] = '\0';
    if ( strcmp((const char *)acStrAll, "all") ) return -1;
    self->bReqSendAll = true; //FIXME
    DBG_PRINT( NET_DEBUG, ("Received cmd 'all', in '%s' /UPVS/upvs_clt.c:%d\r\n", 
      __FUNCTION__, __LINE__) );
  } else {
    // 1ое условие: устройство поддерживает прием только в корневой топик "action"
    if (!strstr((const char *)pcPath, "action")) return -1;
    // 2ое условие: класс устрйоства должен быть CSC (подвагонники)
    if (!strstr((const char *)pcPath, "CSC")) return -1;
    // хз какие еще проверки
    param_set( self, pcPath, path_len, pPld );
    DBG_PRINT( NET_DEBUG, ("Received action %s, in '%s' /UPVS/upvs_clt.c:%d\r\n", 
      pcPath, __FUNCTION__, __LINE__) );
  }
  
  return 0;
}

/**	----------------------------------------------------------------------------
	* @brief ??? */
s32_t
  upvs_clt__get_prm( upvs_clt_t *self, u8_t *pPath, u8_t *pValue, u32_t idx ) {
/*----------------------------------------------------------------------------*/
  bool rc;
  //int idx;
  s32_t sta;
  cJSON* root = NULL;
  param_t *prm;
  //upvs_param_t *param = upvs_param__inst() + item;
  //
  bool bVar;
  s32_t slVar;
  float fVar;
  
  if (!self || !pPath || !pValue) return -1;
  // получаем элемент с индексом idx из списка параметров
  prm = upvs_prm__get_item(self->pxPrm, idx);
  // если запрошенный индекс больше размера массива
  // FIXME не знаю как получить размер массива параметров
  //if (item > (sizeof(*ppsrc)/sizeof(upvs_param_t)))
  //  return -1;
  //param = upvs_param__inst() + item;
  
  // проверка json
  root = cJSON_CreateObject();
  if (!root) return -1;
  // Получаем значение в зависимости от типа параметра 'type'
  switch (prm->xValue.type) {
    case (BOOL):
      sta = upvs_prm__get_b(prm, &bVar);
      if (sta != -1) {
        cJSON_AddItemToObject( root, (const char *)prm->pcName, 
                               cJSON_CreateBool(bVar) );
      }
    break;
    case (INT32):
      sta = upvs_prm__get_sl(prm, &slVar);
      if (sta != -1) {
        cJSON_AddItemToObject( root, (const char *)prm->pcName, 
                               cJSON_CreateInteger(slVar) );
      }
    break;
    case (FLOAT):
      sta = upvs_prm__get_f(prm, &fVar);
      if (sta != -1) {
        cJSON_AddItemToObject( root, (const char *)prm->pcName, 
                               cJSON_CreateFloat(fVar) );
      }
    break;
    case (STRING):
      cJSON_AddItemToObject( root, (const char *)prm->pcName, 
        cJSON_CreateString((const char*)prm->xValue.mag.ac) );
    break;
    case (DATETIME):
      //FIXME
    break;
    default:
			goto errexit;
    break;
  }
  // После чего формируем json-строку { "some_key": some_value } сразу во
  // внешний буфер
  rc = cJSON_PrintPreallocated(root, (char *)pValue, sizeof(pValue), false);
  if (!rc) goto errexit;
  
  // фрмируем pPath и копируем во внешний буфер
  // pcTitle
  strcat((char *)pPath, (const char *)prm->pcTitle);
  idx = strlen((const char *)pPath);
  pPath[idx] = '/'; pPath[idx+1] = '\0';
  //
  strcat((char *)pPath, (const char *)"CSC");
  idx = strlen((const char *)pPath);
  pPath[idx] = '/'; pPath[idx+1] = '\0';
  // pcTitle
  strcat((char *)pPath, (const char *)prm->pcSrvc);
  idx = strlen((const char *)pPath);
  pPath[idx] = '/'; pPath[idx+1] = '\0';
  // pcTitle
  strcat((char *)pPath, (const char *)prm->pcName);
  
  cJSON_Delete(root);
  
  return 0;
  
  errexit:
  DBG_PRINT( NET_DEBUG, ("errexit, in '%s' /UPVS/upvs_clt.c:%d\r\n", 
    __FUNCTION__, __LINE__) );
  if (!root)
    cJSON_Delete(root);
  return -1;
}

/**	----------------------------------------------------------------------------
	* @brief ??? */
int
  upvs_clt__get_err( upvs_clt_t *self, u8_t *path, u8_t *desc, u32_t idx) {
/*----------------------------------------------------------------------------*/
  cJSON* root = NULL;
  char* jsonstr = NULL;
  upvs_err_buf_t *pBuf = NULL;
  err_item_t *item;
  if (!self || !path || !desc) return -1;  
  
  // получаем аварийную запись
  item = upvs_err__get_item(self->pxErr, idx);
  if (!item) return -1;
  // проверка на валидный код аварии
  pBuf = upvs_err__get_buf( self->pxErr, item );
  if (!pBuf) return -1;
  
  // создаем корневой объект
  root = cJSON_CreateObject();
  if (!root) return -1;
  // авария активна
  if (item->bActive) {
    // "code": 100
    cJSON_AddItemToObject(root,"code",cJSON_CreateInteger(pBuf->err.code));
    // "status": True
    cJSON_AddItemToObject(root,"status",cJSON_CreateBool(true));
    // "priority": ?
    cJSON_AddItemToObject(root,"priority", 
      cJSON_CreateInteger(pBuf->err.priority));
    // "describe": ?
    cJSON_AddItemToObject(root,"describe", 
      cJSON_CreateString((const char *)pBuf->err.pcDescribe));
    // "influence": ?
    cJSON_AddItemToObject(root,"influence",
      cJSON_CreateString((const char *)pBuf->err.pcInfluence));
    // "help": ?
    cJSON_AddItemToObject(root,"help", 
      cJSON_CreateString((const char *)pBuf->err.pcHelp));
  }
  // авария неактивна
  else {
    // "code": 100
    cJSON_AddItemToObject(root,"code",cJSON_CreateInteger(pBuf->err.code));
    // "status": False
    cJSON_AddItemToObject(root,"status",cJSON_CreateBool(false));
  }
  // формируем строку
  jsonstr = cJSON_PrintUnformatted(root);
  if (!jsonstr) {
    goto errexit;
  }
  // копируем во внешний буфер path и jsonstr
  strcpy((char *)path, (const char *)"value/CSC/error/code");
  strcpy((char *)desc, (const char *)jsonstr);
  
  cJSON_Delete(root);
  return 0;
  
  errexit:
  //printf("Some shit has happened\n");
  if (!root)
    cJSON_Delete(root);
  return -1;
}

// Вспомогательные функции

/**	----------------------------------------------------------------------------
	* @brief ??? */
void
  upvs_clt__set_sernum(u8_t *pDest, ser_num_t *px) {
/*----------------------------------------------------------------------------*/  
  u8_t buf[6];
  
  if (!pDest || !px) return;
  
  // порядковый номер
  sprintf((char *)(pDest), "%03d", px->Digit);
  // текстовая вставка
  strcat((char *)pDest, pcText);
  // месяц произ-ва
  sprintf((char *)(buf), "%02d", px->Month);
  strcat((char *)pDest, (const char *)buf);
  // год произ-ва
  sprintf((char *)(buf), "%04d", px->Year);
  strcat((char *)pDest, (const char *)buf);
}

/**	----------------------------------------------------------------------------
	* @brief ??? */
void
  upvs_clt__set_mac(u8_t *pDest, u8_t *pSrc, u32_t len) {
/*----------------------------------------------------------------------------*/  
  u8_t i;
  
  if (!pDest || !pSrc) return;
  if (len != 6) return;
  
  for (i=0; i<len; i++) {
    if (i!=5)
      sprintf((char *)(pDest+3*i), "%02d:", *(pSrc+i));
    else
      sprintf((char *)(pDest+3*i), "%02d", *(pSrc+i));
  }
}

// Функции-обёртки (wrappers)

/**	----------------------------------------------------------------------------
	* @brief */
bool
  upvs_clt__is_err_new(upvs_clt_t *self, u32_t idx) {
/*----------------------------------------------------------------------------*/  
  return upvs_err__is_new(self->pxErr, idx);
}

/**	----------------------------------------------------------------------------
	* @brief */
void
  upvs_clt__set_err_new(upvs_clt_t *self, u32_t idx, bool sta) {
/*----------------------------------------------------------------------------*/  
  upvs_err__set_new(self->pxErr, idx, sta);
}

/**	----------------------------------------------------------------------------
	* @brief */
bool
  upvs_clt__is_err_act(upvs_clt_t *self, u32_t idx) {
/*----------------------------------------------------------------------------*/  
  return upvs_err__is_act(self->pxErr, idx);
}

/**	----------------------------------------------------------------------------
	* @brief */
void
  upvs_clt__set_err_act(upvs_clt_t *self, u32_t idx, bool sta) {
/*----------------------------------------------------------------------------*/  
  upvs_err__set_act(self->pxErr, idx, sta);
}

/**	----------------------------------------------------------------------------
	* @brief */
void
  upvs_clt__reset_err(upvs_clt_t *self, u32_t idx) {
/*----------------------------------------------------------------------------*/  
  upvs_err__reset(self->pxErr, idx);
}

/**	----------------------------------------------------------------------------
	* @brief */
s32_t
  upvs_clt__get_err_code(upvs_clt_t *self, u32_t idx) {
/*----------------------------------------------------------------------------*/  
  return upvs_err__get_code(self->pxErr, idx);
}

/**	----------------------------------------------------------------------------
	* @brief */
bool
  upvs_clt__get_prm_new(upvs_clt_t *self, u32_t idx) {
/*----------------------------------------------------------------------------*/  
  return upvs_prm__get_attr_new(upvs_prm__get_item(self->pxPrm, idx));
}

/**	----------------------------------------------------------------------------
	* @brief */
s32_t
  upvs_clt__edit_prm_new(upvs_clt_t *self, u32_t idx, bool var) {
/*----------------------------------------------------------------------------*/
  if (!self) return -1;
  //if (!self->bActive) return -1; // TODO А нужно для клиента Active?
  upvs_prm__set_attr_new(upvs_prm__get_item(self->pxPrm, idx), var);
  return 0;
}


// Локальные (private) функции

/**	----------------------------------------------------------------------------
	* @brief ??? */
static int
  param_set(upvs_clt_t *self, const u8_t *pPath, u32_t path_len, const u8_t *pPld) {
/*----------------------------------------------------------------------------*/
  cJSON *root;
  static parser_t xParser;
	int rc;
  
  // проверка арг-ов
  if ((!pPath) || (!pPld)) return -1;
  // парсим пришедшую json-строку и заполняем объект "root",
  // представляющий собой связный список, содержимым строки
  root = cJSON_Parse((const char *)pPld);
  if (!root) {
    return -1;
  }
  // берем топик для дальнейшей идентификации содержимого
  memcpy((void *)xParser.acTopic, (const void *)pPath, path_len);
  xParser.acTopic[path_len] = '\0';
  // записываем адрес массива для дальнейшего использования
  xParser.xParam.pcTopic = xParser.acTopic;
  // Вытаскием из "root" имя параметра и присваиваем его полю pcName
  xParser.xParam.pcName = (u8_t *)root->child->string;
  // Вытаскием из "root" значение параметра и присваиваем его полю xValue
  rc = typecast(root->child, &(xParser.xParam.xValue));
  if (rc) goto exit;
  // осуществляем поиск параметра по имени и записываем новое значение
  //rc = upvs_param__set(&(xParser.xParam), false);
  rc = upvs_prm__set(self->pxPrm, &(xParser.xParam), false);
  if (rc) goto exit;
  
  exit:
  cJSON_Delete(root);
  return rc;
}

/**	----------------------------------------------------------------------------
	* @brief ???
	* @retval error: Статус выполнения функции. */
static int
  typecast(struct cJSON *pObj, value_ptr_t *pValue) {
/*----------------------------------------------------------------------------*/
  // проверка
  if ((!pObj) || (!pValue)) return -1;

  // ориентируясь на 'type', получаем значение
  switch (pObj->type) {
    case cJSON_True:
    case cJSON_False:
      pValue->type = BOOL;
      pValue->mag.b = (bool)pObj->valueint;
    break;
    case cJSON_Float:
      pValue->type = FLOAT;
      pValue->mag.f = pObj->valuefloat;
    break;
    // 
    /*case: DATETIME
    break;*/
    default:
      return -1;
    break;
  }

  return 0;
}
