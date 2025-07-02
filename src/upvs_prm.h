#ifndef _UPVS_PRM_H_
#define _UPVS_PRM_H_

#include "upvs_conf.h"
#include "userint.h"
#include <stdbool.h>

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
} param_t;

typedef struct {
  param_t *paxList; //[UPVS_PARAM_LIST_LEN];
} upvs_param_t;

void *upvs_prm__create(void);
int   upvs_prm__init(upvs_param_t *);
void  upvs_prm__del(upvs_param_t *);
// new
param_t *upvs_prm__get_item(upvs_param_t *self, u32_t idx);
int upvs_prm__set( upvs_param_t *, param_ptr_t *, bool );
// set
s32_t	upvs_prm__set_b(param_t *, bool, bool);
s32_t	upvs_prm__set_sl(param_t *, s32_t, bool);
s32_t	upvs_prm__set_f(param_t *, f32_t, bool);
s32_t	upvs_prm__set_all(param_t *, bool, bool);
// get
s32_t upvs_prm__get_b(param_t *, bool *);
s32_t	upvs_prm__get_sl(param_t *, s32_t *);
s32_t	upvs_prm__get_f(param_t *, f32_t *);
// type decode
u32_t upvs_prm__enc_type(const u8_t *);


#endif //_UPVS_PRM_H_