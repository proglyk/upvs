#include "upvs_prm.h"
#include <stdlib.h>
#include <string.h>

//const param_t axOriginList[UPVS_PARAM_LIST_LEN];
param_t axOriginList[UPVS_PARAM_LIST_LEN];

// Функции записи в поле "значение" параметра
static s32_t set(value_t *, value_ptr_t *, bool);
static s32_t set_b(value_t *, bool, bool);
static s32_t set_sl(value_t *, s32_t, bool);
static s32_t set_f(value_t *, float, bool);
static s32_t set_all(value_t *, bool, bool);
static s32_t set_str(value_t *, const u8_t *, bool);

// Функции, доступные извне

/**	----------------------------------------------------------------------------
	* @brief ??? */
void *
  upvs_prm__create(void) {
/*----------------------------------------------------------------------------*/
  upvs_param_t *self = malloc(sizeof(upvs_param_t));
  if (!self) return NULL;
  
  self->paxList = malloc(UPVS_PARAM_LIST_LEN * sizeof(param_t));
  if (!self->paxList) return NULL;
  
  return (void *)self;
}

/**	----------------------------------------------------------------------------
	* @brief ??? */
int
  upvs_prm__init(upvs_param_t *self) {
/*----------------------------------------------------------------------------*/
  u32_t i;
  
  if (!self) return -1;
  
  // FIXME убрать после доб upvs_param_list.c
  memset((void *)axOriginList, 0, UPVS_PARAM_LIST_LEN * sizeof(param_t));
  
  for (i=0; i<UPVS_PARAM_LIST_LEN; i++) {
    memcpy( (void *)(self->paxList+i),       // dest в области .heap
            (const void *)&axOriginList[i], // src в области .data
            sizeof(param_t) );              // size = n*sizeof(struct)
  }
  
  // копируем список параметров из переменной в области .data в .heap
  // memcpy( (void *)self->axList,         // dest в области .heap
          // (const void *)axOriginList,   // src в области .data
          // sizeof(axOriginList) );       // size = n*sizeof(struct)
  
  return 0;
}

/**	----------------------------------------------------------------------------
	* @brief */
void
  upvs_prm__del(upvs_param_t *self) {
/*----------------------------------------------------------------------------*/
  if (!self) return;
  if (!self->paxList) return;

  free(self->paxList);
  free(self);
}

/**	----------------------------------------------------------------------------
	* @brief */
param_t *
  upvs_prm__get_item(upvs_param_t *self, u32_t idx) {
/*----------------------------------------------------------------------------*/
  if (!self) return NULL;
  if (idx >= UPVS_PARAM_LIST_LEN) return NULL;
  
  return &(self->paxList[idx]);
}

/**	----------------------------------------------------------------------------
	* @brief ??? */
int
  upvs_prm__set( upvs_param_t *self, param_ptr_t *psrc, bool changes ) {
/*----------------------------------------------------------------------------*/
  int i=0;
  //upvs_param_t *px = upvs_param__inst();

  // проверка
  if (!psrc) return -1;

  // бегунок по элементам массива пока поле pcTopic не нуль
  do {
    // В случае совпадания полного пути:
    // FIXME
    if (!strcmp( (const char *)self->paxList[i].pcTopic,
                 (const char *)psrc->pcTopic ))
    {
      // Проверяем соответствие имени в полях ключа и полного пути...
      //if (strcmp(px[i].pcName, (const char *)psrc->pcName)) return -1;
      // ... и записываем новое значение.
      return set( &(self->paxList[i].xValue), &(psrc->xValue), changes );
    }
    i++;
  } while (self->paxList[i].pcName); //FIXME было pcTopic

  return -1;
}

/**	----------------------------------------------------------------------------
	* @brief ??? */
s32_t
	upvs_prm__set_b(param_t *ptr, bool var, bool changes) {
/*----------------------------------------------------------------------------*/
  if (!ptr) return -1;
  return set_b(&(ptr->xValue), var, changes);
}

/**	----------------------------------------------------------------------------
	* @brief ??? */
s32_t
	upvs_prm__set_sl(param_t *ptr, s32_t var, bool changes) {
/*----------------------------------------------------------------------------*/
  if (!ptr) return -1;
  return set_sl(&(ptr->xValue), var, changes);
}

/**	----------------------------------------------------------------------------
	* @brief ??? */
s32_t
	upvs_prm__set_f(param_t *ptr, f32_t var, bool changes) {
/*----------------------------------------------------------------------------*/
  if (!ptr) return -1;
  return set_f(&(ptr->xValue), var, changes);
}

/**	----------------------------------------------------------------------------
	* @brief ??? */
s32_t
	upvs_prm__set_all(param_t *ptr, bool var, bool changes) {
/*----------------------------------------------------------------------------*/
  if (!ptr) return -1;
  return set_all(&(ptr->xValue), var, changes);
}

/**	----------------------------------------------------------------------------
	* @brief ??? */
s32_t
	upvs_prm__get_b(param_t *px, bool *parg) {
/*----------------------------------------------------------------------------*/  
  // проверка аргументов
  if (!px) { *parg = false; return -1; }
  if (px->xValue.type != BOOL) { *parg = false; return -1; }
  // получение
  *parg = px->xValue.mag.b;
  return 0;
}

/**	----------------------------------------------------------------------------
	* @brief ??? */
s32_t
	upvs_prm__get_sl(param_t *px, s32_t *parg) {
/*----------------------------------------------------------------------------*/
	// проверка аргументов
  if (!px) { *parg = 0; return -1; }
  if (px->xValue.type != INT32) { *parg = 0; return -1; }
  // получение
  *parg = px->xValue.mag.sl;
  return 0;
}

/**	----------------------------------------------------------------------------
	* @brief ??? */
s32_t
	upvs_prm__get_f(param_t *px, f32_t *parg) {
/*----------------------------------------------------------------------------*/
  // проверка аргументов
  if (!px) { *parg = 0.0f; return -1; }
  if (px->xValue.type != FLOAT) { *parg = 0.0f; return -1; }
  // получение
  *parg = px->xValue.mag.f;
  return 0;
}

/**	----------------------------------------------------------------------------
	* @brief Возвращает строковое представление типа
	* @param code: Код типа в числовом представлении
	* @retval Указатель на строку */
u32_t
  upvs_prm__enc_type(const u8_t *pstr) {
/*----------------------------------------------------------------------------*/
  u32_t code = 0;

  if      (!strcmp((const char *)pstr, "ready")) code = 1;
  else if (!strcmp((const char *)pstr, "not_ready")) code = 2;
  else if (!strcmp((const char *)pstr, "work")) code = 3;
  else if (!strcmp((const char *)pstr, "alarm")) code = 4;
  else if (!strcmp((const char *)pstr, "APV")) code = 5;
  else if (!strcmp((const char *)pstr, "off")) code = 6;
  else if (!strcmp((const char *)pstr, "grid380v")) code = 7;
  else if (!strcmp((const char *)pstr, "diesel")) code = 8;
  else if (!strcmp((const char *)pstr, "line3kv")) code = 9;
  else if (!strcmp((const char *)pstr, "ab110v")) code = 10;
  else if (!strcmp((const char *)pstr, "90KGL375P 60A")) code = 11;
  else if (!strcmp((const char *)pstr, "56PzV385P 60A")) code = 12;
  else if (!strcmp((const char *)pstr, "56PzV385P 40A")) code = 13;
  else if (!strcmp((const char *)pstr, "No battery")) code = 14;
  
  return code;
}

// Функции, ограниченные областью видимости данного файла

/**	----------------------------------------------------------------------------
	* @brief ??? */
static s32_t
  set(value_t *pDest, value_ptr_t *pSrc, bool changes) {
/*----------------------------------------------------------------------------*/
  s32_t rc = 0;
  // проверка аргументов
  if ((!pDest) || (!pSrc)) return -1;
  // ориентируясь на 'type', получаем значение
  switch (pSrc->type) {
    case BOOL: rc = set_b(pDest, pSrc->mag.b, changes); break;
    case INT32: rc = set_sl(pDest, pSrc->mag.sl, changes); break;
    case FLOAT: rc = set_f(pDest, pSrc->mag.f, changes); break;
    case STRING: rc = set_str(pDest, (const u8_t *)pSrc->mag.pc, changes); break;
    case DATETIME: /* TODO */ break;
    default: rc = -1; break;
  }
  
  return rc;
}

/**	----------------------------------------------------------------------------
	* @brief ??? */
static s32_t
	set_b(value_t *px, bool var, bool changes) {
/*----------------------------------------------------------------------------*/
	// проверка аргументов
  if (!px) return -1;
  if (px->type != BOOL) return -1;
  // отслеживание изменений
  if (changes) {
    (px->mag.b != var) ? (px->bNew = true) : (px->bNew = false); 
  }
  // запись
  px->mag.b = var;
  return 0;
}

/**	----------------------------------------------------------------------------
	* @brief ??? */
static s32_t
	set_sl(value_t *px, s32_t var, bool changes) {
/*----------------------------------------------------------------------------*/
 	// проверка аргументов
  if (!px) return -1;
  if (px->type != INT32) return -1;
  // отслеживание изменений
  if (changes) {
    (px->mag.sl != var) ? (px->bNew = true) : (px->bNew = false); 
  }
  // запись
  px->mag.sl = var;
  return 0;
}

/**	----------------------------------------------------------------------------
	* @brief ??? */
static s32_t
	set_f(value_t *px, f32_t var, bool changes) {
/*----------------------------------------------------------------------------*/
 	// проверка аргументов
  if (!px) return -1;
  if (px->type != FLOAT) return -1;
  // отслеживание изменений
  if (changes) {
    (px->mag.f != var) ? (px->bNew = true) : (px->bNew = false); 
  }
  // запись
  px->mag.f = var;
  return 0;
}

/**	----------------------------------------------------------------------------
	* @brief ??? */
static s32_t
	set_all(value_t *px, bool var, bool changes) {
/*----------------------------------------------------------------------------*/
 	// проверка аргументов
  if (!px) return -1;
  if (px->type != GETALL) return -1;
  // отслеживание изменений
  if (changes) {
    (px->mag.b != var) ? (px->bNew = true) : (px->bNew = false); 
  }
  // запись
  px->mag.b = var;
  return 0;
}

/**	----------------------------------------------------------------------------
	* @brief ??? */
static s32_t
	set_str(value_t *px, const u8_t *var, bool changes) {
/*----------------------------------------------------------------------------*/
 	// проверка аргументов
  if (!px) return -1;
  if (px->type != STRING) return -1;
  // отслеживание изменений
  if (changes) {
    (strcmp((const char *)px->mag.ac, (const char *)var) != NULL) ?
      (px->bNew = true) : (px->bNew = false); 
  }
  // запись
  strcpy((char *)px->mag.ac, (const char *)var);
  return 0;
}

