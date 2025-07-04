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

// Константы

//static const char* pcText = "АО";
static const char* pcTypeState1 = "ready";
static const char* pcTypeState2 = "not_ready";
static const char* pcTypeState3 = "work";
static const char* pcTypeState4 = "alarm";
static const char* pcTypeState5 = "APV";
static const char* pcTypeState6 = "off";
static const char* pcTypePower1 = "grid380v";
static const char* pcTypePower2 = "diesel";
static const char* pcTypePower3 = "line3kv";
static const char* pcTypePower4 = "ab110v";
static const char* pcTypeAb1 = "90KGL375P 60A";
static const char* pcTypeAb2 = "56PzV385P 60A";
static const char* pcTypeAb3 = "56PzV385P 40A";
static const char* pcTypeAb4 = "No battery";

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
  // vsp
  upvs_prm__set_type(self->paxList[6].xValue.mag.ac, 1); // state
  // nsp
  upvs_prm__set_type(self->paxList[16].xValue.mag.ac, 1); // state
  // ch1
  upvs_prm__set_type(self->paxList[28].xValue.mag.ac, 1); // state
  // ch2
  upvs_prm__set_type(self->paxList[37].xValue.mag.ac, 1); // state
  // ch3
  upvs_prm__set_type(self->paxList[45].xValue.mag.ac, 1); // state
  // ch4
  upvs_prm__set_type(self->paxList[52].xValue.mag.ac, 1); // state
  
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
	* @brief ??? */
void
	upvs_prm__set_attr_new(param_t *px, bool sta) {
/*----------------------------------------------------------------------------*/
  if (!px) return;
  px->xValue.bNew = sta;
}

/**	----------------------------------------------------------------------------
	* @brief ??? */
bool
	upvs_prm__get_attr_new(param_t *px) {
/*----------------------------------------------------------------------------*/
  // проверка аргументов
  return px->xValue.bNew;
}

/**	----------------------------------------------------------------------------
	* @brief Возвращает строковое представление типа
	* @param code: Код типа в числовом представлении
	* @retval Указатель на строку */
u32_t
  upvs_prm__enc_type(const u8_t *pstr) {
/*----------------------------------------------------------------------------*/
  u32_t code = 0;

  if      (!strcmp((const char *)pstr, pcTypeState1)) code = 1;
  else if (!strcmp((const char *)pstr, pcTypeState2)) code = 2;
  else if (!strcmp((const char *)pstr, pcTypeState3)) code = 3;
  else if (!strcmp((const char *)pstr, pcTypeState4)) code = 4;
  else if (!strcmp((const char *)pstr, pcTypeState5)) code = 5;
  else if (!strcmp((const char *)pstr, pcTypeState6)) code = 6;
  else if (!strcmp((const char *)pstr, pcTypePower1)) code = 7;
  else if (!strcmp((const char *)pstr, pcTypePower2)) code = 8;
  else if (!strcmp((const char *)pstr, pcTypePower3)) code = 9;
  else if (!strcmp((const char *)pstr, pcTypePower4)) code = 10;
  else if (!strcmp((const char *)pstr, pcTypeAb1)) code = 11;
  else if (!strcmp((const char *)pstr, pcTypeAb2)) code = 12;
  else if (!strcmp((const char *)pstr, pcTypeAb3)) code = 13;
  else if (!strcmp((const char *)pstr, pcTypeAb4)) code = 14;
  
  return code;
}

/**	----------------------------------------------------------------------------
	* @brief Возвращает строковое представление типа
	* @param code: Код типа в числовом представлении
	* @retval Указатель на строку */
const char *
  upvs_prm__dec_type(u32_t code) {
/*----------------------------------------------------------------------------*/
  const char *ptr = NULL;
  
  if      (code ==  1) ptr = pcTypeState1;
  else if (code ==  2) ptr = pcTypeState2;
  else if (code ==  3) ptr = pcTypeState3;
  else if (code ==  4) ptr = pcTypeState4;
  else if (code ==  5) ptr = pcTypeState5;
  else if (code ==  6) ptr = pcTypeState6;
  else if (code ==  7) ptr = pcTypePower1;
  else if (code ==  8) ptr = pcTypePower2;
  else if (code ==  9) ptr = pcTypePower3;
  else if (code == 10) ptr = pcTypePower4;
  else if (code == 11) ptr = pcTypeAb1;
  else if (code == 12) ptr = pcTypeAb2;
  else if (code == 13) ptr = pcTypeAb3;
  else if (code == 14) ptr = pcTypeAb4;
  
  return ptr;
}

/**	----------------------------------------------------------------------------
	* @brief ???
  * @param pDest: ???
	* @param code: ???
	* @retval ??? */
s32_t
  upvs_prm__set_type(u8_t *pDest, u32_t code) {
/*----------------------------------------------------------------------------*/  
  if (!pDest) return -1;
  
  switch (code) {
    case  1: strcpy((char *)pDest, pcTypeState1); break;
    case  2: strcpy((char *)pDest, pcTypeState2); break;
    case  3: strcpy((char *)pDest, pcTypeState3); break;
    case  4: strcpy((char *)pDest, pcTypeState4); break;
    case  5: strcpy((char *)pDest, pcTypeState5); break;
    case  6: strcpy((char *)pDest, pcTypeState6); break;
    case  7: strcpy((char *)pDest, pcTypePower1); break;
    case  8: strcpy((char *)pDest, pcTypePower2); break;
    case  9: strcpy((char *)pDest, pcTypePower3); break;
    case 10: strcpy((char *)pDest, pcTypePower4); break;
    case 11: strcpy((char *)pDest, pcTypeAb1); break;
    case 12: strcpy((char *)pDest, pcTypeAb2); break;
    case 13: strcpy((char *)pDest, pcTypeAb3); break;
    case 14: strcpy((char *)pDest, pcTypeAb4); break;
    default: return -1;
  }
  
  return 0;
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

