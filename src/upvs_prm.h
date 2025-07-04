#ifndef _UPVS_PRM_H_
#define _UPVS_PRM_H_

#include "upvs_conf.h"
#include "userint.h"
#include <stdbool.h>
#include <string.h>

#define UPVS_PRM__ADD_VAL_B(X, B)                         \
  do {                                                            \
    (X->xValue.mag.b != B) ? (X->xValue.bNew = true) : (X->xValue.bNew = false);     \
    X->xValue.mag.b = B;                                             \
  } while(0) 

#define UPVS_PRM__ADD_VAL_SL(X, SL)                       \
  do {                                                            \
    (X->xValue.mag.sl != SL) ? (X->xValue.bNew = true) : (X->xValue.bNew = false);   \
    X->xValue.mag.sl = SL;                                           \
  } while(0) 

#define UPVS_PRM__ADD_VAL_F_1(X, F, PTR)                  \
  do {                                                            \
		((*PTR = (roundf(F * 10UL) / 10U)) != X->xValue.mag.f) ?	        \
			(X->xValue.bNew = true) : (X->xValue.bNew = false);               \
    X->xValue.mag.f = *PTR;                                          \
  } while(0) 

/*#define UPVS_PARAM_CLT__ADD_VALUE_F_01(X, F, PTR)								  \
  do {                                                            \
		((*PTR = (roundf(F * 100UL) / 100U)) != X.xValue.mag.f) ?			  \
			(X.xValue.bNew = true) : (X.xValue.bNew = false);               \
    X.xValue.mag.f = *PTR;                                          \
  } while(0)*/

#define UPVS_PRM__ADD_VAL_S(X, S)                         \
  do {                                                            \
    (strcmp((const char *)X->xValue.mag.ac, (const char *)S) != 0) ? \
			(X->xValue.bNew = true) : (X->xValue.bNew = false);               \
    strcpy((char *)X->xValue.mag.ac, (const char *)S);               \
  } while(0)

#define UPVS_PRM__ADD_VAL_T(X, T)                                     \
  do {                                                                        \
    (strcmp((const char *)X->xValue.mag.ac, upvs_prm__dec_type(T)) != 0) ?   \
			(X->xValue.bNew = true) : (X->xValue.bNew = false);                       \
    upvs_prm__set_type(X->xValue.mag.ac, T);                                   \
  } while(0)

#define UPVS_PRM__ADD_VAL_VERS(X, PVERS, PBUF)            \
  do {                                                            \
    sprintf((char *)PBUF, "%01d.%01d", *(PVERS+0), *(PVERS+1));           \
    (strcmp((const char *)X->xValue.mag.ac, (const char *)PBUF) != 0) ?  \
			(X->xValue.bNew = true) : (X->xValue.bNew = false);               \
    strcpy((char *)X->xValue.mag.ac, (const char *)PBUF);            \
  } while(0)
 
typedef enum {
  UNK,
  BOOL,
  INT32,
  FLOAT,
  STRING,
  DATETIME,
  // исп. только в server
  GETALL
} type_t;

// Посмотреть еще раз. Начало

// ПЕРЕДЕЛАТЬ НА ОБЩИЙ ТИП, где в mag есть в т.ч. 'u8_t *pc'
typedef struct {
  // Значение
  struct {
    u8_t *pc;
    bool b;
    s32_t sl;
    float f;
  } mag;
  // Тип
  type_t type;
} value_ptr_t;

typedef struct {
  // Имя параметра (ключ)
  u8_t *pcName;
  // Указатель на полный путь к теме
  u8_t *pcTopic;
  // Значение параметра (может отличаться в зависимости от типа)
  value_ptr_t xValue;
} param_ptr_t;

typedef struct {
  // Полный путь к теме
  u8_t acTopic[UPVS_TOPICPATH_SIZE];
  param_ptr_t xParam;
  //error_ptr_t xError;
} parser_t;
// ПЕРЕДЕЛАТЬ НА ОБЩИЙ ТИП, где в mag есть в т.ч. 'u8_t *pc'

// Посмотреть еще раз. Конец

typedef struct {
  // Значение
  struct {
    u8_t ac[UPVS_PARAM_VALUE_SIZE];
    bool b;
    s32_t sl;
    float f;
  } mag;
  // Тип
  type_t type;
  bool bNew; // переменная изменилась, только для "Действий" (actions)
} value_t;

typedef struct {
  // Сервис
  //const u8_t *pcSrvc;
  // Имя параметра (ключ)
  //const char *pcName;
  // Полный путь к теме
  const u8_t *pcTitle;  // Только заглавный топик
  const u8_t *pcSrvc;   // Только сервис
  const u8_t *pcName;   // Имя параметра (ключ)
  const u8_t *pcTopic;  // Полный путь
  value_t xValue;       // Значение параметра
} prm_t;

#define prm_get_item(ARG1, ARG2) upvs_prm__get_item(ARG1, ARG2)

typedef struct {
  prm_t *paxList; //[UPVS_PARAM_LIST_LEN];
} upvs_prm_t;

void *upvs_prm__create(void);
int   upvs_prm__init(upvs_prm_t *);
void  upvs_prm__del(upvs_prm_t *);
// new
prm_t *upvs_prm__get_item(upvs_prm_t *self, u32_t idx);
int upvs_prm__set( upvs_prm_t *, param_ptr_t *, bool );
// set
s32_t	upvs_prm__set_b(prm_t *, bool, bool);
s32_t	upvs_prm__set_sl(prm_t *, s32_t, bool);
s32_t	upvs_prm__set_f(prm_t *, f32_t, bool);
s32_t	upvs_prm__set_all(prm_t *, bool, bool);
// get
s32_t upvs_prm__get_b(prm_t *, bool *);
s32_t	upvs_prm__get_sl(prm_t *, s32_t *);
s32_t	upvs_prm__get_f(prm_t *, f32_t *);
// attributes
void  upvs_prm__set_attr_new(prm_t *, bool);
bool  upvs_prm__get_attr_new(prm_t *);
// type decode
u32_t upvs_prm__enc_type(const u8_t *);
const char *upvs_prm__dec_type(u32_t);
s32_t upvs_prm__set_type(u8_t *, u32_t);

#endif //_UPVS_PRM_H_