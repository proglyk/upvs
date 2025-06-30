#include "upvs_srv.h"
#include <stdlib.h>
#include "cJSON.h"
#include <string.h>
#include "dbg.h"

static int prm_set( upvs_srv_t *, parser_t *, const u8_t * );
static int error_set( const u8_t * );
static s32_t error_remove(s32_t);
static s32_t error_insert(cJSON *root, s32_t);
static s32_t error_update(s32_t idx, const u8_t *pBuf);
static int pvs_value_typecast(struct cJSON *, value_ptr_t *);

/**	----------------------------------------------------------------------------
	* @brief ??? */
void *
  upvs_srv__create(void) {
/*----------------------------------------------------------------------------*/
  upvs_srv_t *self = malloc(sizeof(upvs_srv_t));
  if (!self) return NULL;
  
  self->pxParam = upvs_prm__create();
  if (!self->pxParam) return NULL;
  
  return (void *)self;
}

/**	----------------------------------------------------------------------------
	* @brief ??? */
int
  upvs_srv__init(upvs_srv_t *self) {
/*----------------------------------------------------------------------------*/
  if (!self) return -1;
  
  if (upvs_prm__init(self->pxParam) < 0) return -1;
  
  return 0;
}

/**	----------------------------------------------------------------------------
	* @brief */
void
  upvs_srv__del(upvs_srv_t *self) {
/*----------------------------------------------------------------------------*/
  if (!self) return;
  if (!self->pxParam) return;
  
  upvs_prm__del(self->pxParam);
  free(self);
}

/**	----------------------------------------------------------------------------
	* @brief ???
	* @retval Статус выполнения */
s32_t
	upvs_srv__get(u32_t item, u8_t *pPath, u8_t *pValue, u32_t value_len) {
/*----------------------------------------------------------------------------*/
  /*cJSON* root = NULL;
  upvs_param_t *pax;
  bool rc;
  s32_t sta;
  //
  bool bVar;
  float fVar;
  
   // проверка арг-ов
  if (!pPath || !pValue) return -1;
  // ук-тель на параметром с индексом item
  pax = (upvs_param_t *)(upvs_param__inst() + item);
  if (!pax) return -1;
  // формируем json
  root = cJSON_CreateObject();
  if (!root) return -1;
  
  // Получаем значение в зависимости от типа 'type' параметра 
  switch (pax->xValue.type) {
    case (BOOL):
      sta = upvs_param__get_b(pax, &bVar);
      if (sta != -1) {
        cJSON_AddItemToObject( root, (const char *)pax->pcName, 
                               cJSON_CreateBool(bVar) );
        // После чего формируем json-строку { "some_key": some_value } сразу во
        // внешний буфер
        rc = cJSON_PrintPreallocated(root, (char *)pValue, value_len, false);
        if (!rc) goto errexit;
      }
    break;
    case (FLOAT):
      sta = upvs_param__get_f(pax, &fVar);
      if (sta != -1) {
        cJSON_AddItemToObject( root, (const char *)pax->pcName, 
                               cJSON_CreateFloat(fVar) );
        // После чего формируем json-строку { "some_key": some_value } сразу во
        // внешний буфер
        rc = cJSON_PrintPreallocated(root, (char *)pValue, value_len, false);
        if (!rc) goto errexit;
      }
    break;
    case (GETALL):
      strcpy((char *)pValue, (const char *)pax->pcName);
    break;
    case (DATETIME):
      //FIXME
    break;
    default:
			goto errexit;
    break;
  }

  // pcTitle
  strcpy((char *)pPath, (const char *)pax->pcTopic);
  
	//cJSON_Delete(root);*/
  return 0;
  
  /* errexit:
  if (!root)
    cJSON_Delete(root);
  return rc; */
}

/**	----------------------------------------------------------------------------
	* @brief ??? */
int
  upvs_srv__set( upvs_srv_t *self, const u8_t *pcPath, u32_t path_len, const u8_t *pcPld ) {
/*----------------------------------------------------------------------------*/
  static const char* pcErrorPath = "value/CSC/error/code";
  static parser_t xParser;
  int rc = 0;
  
  // проверка арг-ов
  if (!pcPath) return -1;
  
  // берем топик для дальнейшей идентификации содержимого
  memcpy((void *)xParser.acTopic, (const void *)pcPath, path_len);
  xParser.acTopic[path_len] = '\0';
  
  // 3. определить тип данных:
  // 3.1 авария
  if ( !strcmp((const void *)xParser.acTopic, pcErrorPath) ) {
    rc = error_set( pcPld );
  } else {
    rc = prm_set( self, &xParser, pcPld );
  }
  
  return rc;
}

/**	----------------------------------------------------------------------------
	* @brief */
param_t *
  upvs_srv__get_prm(upvs_srv_t *self, u32_t idx) {
/*----------------------------------------------------------------------------*/
  if (!self) return NULL;
  if (!self->bActive) return NULL;
  return upvs_prm__get_item(self->pxParam, idx);
}

/**	----------------------------------------------------------------------------
	* @brief */
s32_t
  upvs_srv__set_prm_b(upvs_srv_t *self, u32_t idx, bool var, bool ch) {
/*----------------------------------------------------------------------------*/
  if (!self) return -1;
  if (!self->bActive) return -1;
  return (upvs_prm__set_b(upvs_prm__get_item(self->pxParam, idx), var, ch));
}

/**	----------------------------------------------------------------------------
	* @brief */
s32_t
  upvs_srv__set_prm_sl(upvs_srv_t *self, u32_t idx, s32_t var, bool ch) {
/*----------------------------------------------------------------------------*/
  if (!self) return -1; 
  if (!self->bActive) return -1;
  return (upvs_prm__set_sl(upvs_prm__get_item(self->pxParam, idx), var, ch));
}

/**	----------------------------------------------------------------------------
	* @brief */
s32_t
  upvs_srv__set_prm_f(upvs_srv_t *self, u32_t idx, f32_t var, bool ch) {
/*----------------------------------------------------------------------------*/
  if (!self) return -1;
  if (!self->bActive) return -1;
  return (upvs_prm__set_f(upvs_prm__get_item(self->pxParam, idx), var, ch));
}

/**	----------------------------------------------------------------------------
	* @brief */
s32_t
  upvs_srv__set_prm_all(upvs_srv_t *self, u32_t idx, bool var, bool ch) {
/*----------------------------------------------------------------------------*/
  if (!self) return -1;
  if (!self->bActive) return -1;
  return (upvs_prm__set_all(upvs_prm__get_item(self->pxParam, idx), var, ch));
}

/**	----------------------------------------------------------------------------
	* @brief */
s32_t
  upvs_srv__get_prm_b(upvs_srv_t *self, u32_t idx, bool *parg) {
/*----------------------------------------------------------------------------*/
  if (!self) return -1;
  if (!self->bActive) return -1;
  return (upvs_prm__get_b(upvs_prm__get_item(self->pxParam, idx), parg));
}

/**	----------------------------------------------------------------------------
	* @brief */
s32_t
  upvs_srv__get_prm_sl(upvs_srv_t *self, u32_t idx, s32_t *parg) {
/*----------------------------------------------------------------------------*/
  if (!self) return -1;
  if (!self->bActive) return -1;
  return (upvs_prm__get_sl(upvs_prm__get_item(self->pxParam, idx), parg));
}

/**	----------------------------------------------------------------------------
	* @brief */
s32_t
  upvs_srv__get_prm_f(upvs_srv_t *self, u32_t idx, f32_t *parg) {
/*----------------------------------------------------------------------------*/
  if (!self) return -1;
  if (!self->bActive) return -1;
  return (upvs_prm__get_f(upvs_prm__get_item(self->pxParam, idx), parg));
}


// Функции, ограниченные областью видимости данного файла

/**	----------------------------------------------------------------------------
	* @brief ??? */
static int
  prm_set( upvs_srv_t *self, parser_t *pxPrs, const u8_t *pPld) {
/*----------------------------------------------------------------------------*/
  cJSON *root;
  //static parser_t xParser;
	int rc;
  
  // проверка арг-ов
  if ((!pxPrs) || (!pPld)) return -1;
  // парсим пришедшую json-строку и заполняем объект "root",
  // представляющий собой связный список, содержимым строки
  root = cJSON_Parse((const char *)pPld);
  if (!root) {
    return -1;
  }
  // Вытаскием из "root" имя параметра и присваиваем его полю pcName
  pxPrs->xParam.pcName = (u8_t *)root->child->string;
  // Вытаскием из "root" значение параметра и присваиваем его полю xValue
  rc = pvs_value_typecast(root->child, &(pxPrs->xParam.xValue));
  if (rc) goto exit;
  // записываем адрес массива для дальнейшего использования
  pxPrs->xParam.pcTopic = pxPrs->acTopic;
  // осуществляем поиск параметра по имени и записываем новое значение
  rc = upvs_prm__set( self->pxParam, &(pxPrs->xParam), false );
  //if (rc) goto exit;
  
  exit:
  if (rc) {
    DBG_PRINT( NET_DEBUG, ("'%s' wasn't set (sta=%02d), in '%s' /UPVS/upvs_srv.c:%d\r\n", 
      pxPrs->xParam.pcName, rc, __FUNCTION__, __LINE__) );
  }
  cJSON_Delete(root);
  return rc;
}

/**	----------------------------------------------------------------------------
	* @brief ??? */
static int
  error_set( const u8_t *pcPld ) {
/*----------------------------------------------------------------------------*/
  s32_t rc = 0;
  /*cJSON *root;
	bool active;
	s32_t code;
	
	s32_t idx;
	//static u8_t acBuffer[HELP_LENGHT]; // под времен. размещ. содержимого Help
	upvs_err_db_t *pPoll = upvs_err__db_inst();
	if ( !pcPld ) goto exit;
  
  // парсим пришедшую json-строку и заполняем объект "root", представляющий 
  // собой связный список, содержащий строки
  root = cJSON_Parse((const char *)pcPld);
  if (!root) goto exit;
	
	// берём "code"
  if (!root->child) goto exit;
  if (strcmp((const void *)root->child->string, "code") != 0) goto exit;
  code = root->child->valueint;
	
	// берём "status"
	if (!root->child) goto exit;
	if (!root->child->next) goto exit;
	if (strcmp((const void *)root->child->next->string, "status") != 0) 
		goto exit;
	active = (bool)root->child->next->valueint;

  idx = upvs_err__get_cell_id(pPoll, code);
	// если авария с данными кодом и значением ранее были зафиксированы
  if (idx >= 0) {
		// проверяем статус 'active' нового сообщения
		if (active) {
      // берём и записываем "help" во временный буфер
      if (!root->child->next->next) goto exit;
      if (!root->child->next->next->next) goto exit;
      if (!root->child->next->next->next->next) goto exit;
      if (!root->child->next->next->next->next->next) goto exit;
      if (strcmp( (const void *)root->child->next->next->next->next->next->string,
                  "help" ) != 0) goto exit;
      //strcpy((void *)acBuffer, 
      //  (const void *)root->child->next->next->next->next->next->valuestring);
      //проверяем соответствие Help
			if (!upvs_err__is_help_equal( pPoll, idx,
        (const u8_t *)root->child->next->next->next->next->next->valuestring ))
      {
				// Если различаются, то обновляем
				rc = error_update( idx,
          (const u8_t *)root->child->next->next->next->next->next->valuestring );
			} else {
				// Если же help равны, то ничего не делаем
				asm("nop");
			}
		} else {
			// Если запись уже есть то эту запись удаляем
			rc = error_remove(idx);
		}
  }	else {
		// снова проверяем статус 'active'
		if (active) {
			// вставляем новую запись
			rc = error_insert(root, code);
		} else {
			// или ничего не делаем
			asm("nop");
		}
	}

  // удаляем объект
  cJSON_Delete(root);*/
  return rc;
  
  /*exit:
  DBG_PRINT( NET_DEBUG, ("Error while parsing JSON, "      \
    "in '%s' /UPVS/upvs_srv.c:%d\r\n", __FUNCTION__, __LINE__) );
  cJSON_Delete(root);
  return -1;*/
}

/**	----------------------------------------------------------------------------
	* @brief ??? */
static s32_t
  error_insert(cJSON *root, s32_t code) {
/*----------------------------------------------------------------------------*/
  /*s32_t idx;
  upvs_err_cell_t *pcell;
  upvs_err_db_t *pPoll = upvs_err__db_inst();
  if (!root) goto exit;
  
  // получаем индекс пустой записи в журнале. Будем записывать в неё данные
  idx = upvs_err__db_get_free_pos(pPoll);
  if (idx < 0) {
    DBG_PRINT( NET_DEBUG, ("Can't get free pos for Error '%d', in '%s' "  \
      "/UPVS/upvs_srv.c:%d\r\n", code, __FUNCTION__, __LINE__) );
    return -1;
  }
  
  // берём ссылку на место под запись
  pcell = (pPoll->axCells + idx);
  // записываем 'code', 'status'
  pcell->code = code;
  pcell->status = (s32_t)true;
  
  // берём и записываем "priority"
  if (!root->child->next->next) goto exit;
  if (strcmp((const void *)root->child->next->next->string, "priority") != 0) 
    goto exit;
  pcell->priority = root->child->next->next->valueint;

  // берём и записываем "describe"
  if (!root->child->next->next->next) goto exit;
  if (strcmp((const void *)root->child->next->next->next->string, "describe") != 0) 
    goto exit;
  strcpy((void *)pcell->acDesc, 
    (const void *)root->child->next->next->next->valuestring);
    
  // берём и записываем "influence"
  if (!root->child->next->next->next->next) goto exit;
  if (strcmp((const void *)root->child->next->next->next->next->string, "influence") != 0) 
    goto exit;
  strcpy((void *)pcell->acInfl, 
    (const void *)root->child->next->next->next->next->valuestring);

  // берём и записываем "help"
  if (!root->child->next->next->next->next->next) goto exit;
  if (strcmp((const void *)root->child->next->next->next->next->next->string, "help") != 0) 
    goto exit;
  strcpy((void *)pcell->acHelp, 
    (const void *)root->child->next->next->next->next->next->valuestring);
  
  return 0;
  
  exit:
  DBG_PRINT( NET_DEBUG, ("Error while parsing JSON, "      \
    "in '%s' /UPVS/upvs_srv.c:%d\r\n", __FUNCTION__, __LINE__) );
  return -1;*/ return 0;
}

/**	----------------------------------------------------------------------------
	* @brief ??? */
static s32_t
  error_remove(s32_t idx) {
/*----------------------------------------------------------------------------*/
  /*upvs_err_cell_t *pcell;
  upvs_err_db_t *pPoll = upvs_err__db_inst();
    
  // берём ссылку на место под запись
  pcell = (pPoll->axCells + idx);
  // зануляем поля
  pcell->code = 0;
  pcell->priority = 0;
  pcell->status = 0;
  memset((void *)pcell->acDesc, 0, sizeof(pcell->acDesc));
  memset((void *)pcell->acInfl, 0, sizeof(pcell->acInfl));
  memset((void *)pcell->acHelp, 0, sizeof(pcell->acHelp));*/
  
  return 0;
}

/**	----------------------------------------------------------------------------
	* @brief ??? */
static s32_t
  error_update(s32_t idx, const u8_t *pBuf) {
/*----------------------------------------------------------------------------*/
  /*upvs_err_cell_t *pcell;
  upvs_err_db_t *pPoll = upvs_err__db_inst();
  
  // берём ссылку на место под запись
  pcell = (pPoll->axCells + idx);

  // записываем "help"
  strcpy((void *)pcell->acHelp, (const void *)pBuf);*/
  
  return 0;
}

/**	----------------------------------------------------------------------------
	* @brief ???
	* @retval error: Статус выполнения функции. */
static int
  pvs_value_typecast(struct cJSON *pObj, value_ptr_t *pValue) {
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
    case cJSON_Integer:
      pValue->type = INT32;
      pValue->mag.sl = pObj->valueint;
    break;
    case cJSON_Float:
      pValue->type = FLOAT;
      pValue->mag.f = pObj->valuefloat;
    break;
    case cJSON_String:
      pValue->type = STRING;
      pValue->mag.pc = (u8_t *)pObj->valuestring;
    break;
    default:
      return -1;
    break;
  }

  return 0;
}
