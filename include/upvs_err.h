#ifndef _UPVS_ERR_H_
#define _UPVS_ERR_H_

#include "proj_conf.h"
#include "upvs_conf.h"
#include "userint.h"
#include <stdbool.h>
#if (defined SRV)
#include "ipc.h"
#endif // SRV

typedef struct {
  u32_t ulCode;
  bool  bActive;
#if   (defined CLT)
  s32_t slValue;
  bool  bNew;
#elif (defined SRV)
  u32_t ulPrio;
  u8_t  acDesc[DESC_LENGHT];
  u8_t  acInfl[INFL_LENGHT];
  u8_t  acHelp[HELP_LENGHT];
#else
#error "Must be defined CLT or SRV"
#endif
} err_item_t;

typedef struct {
  struct {
    // аварийные данные
    int code;
    int priority;
    u8_t* pcDescribe;
    u8_t* pcInfluence;
    u8_t* pcHelp;
  } err;
  struct {
    // Буферы динамич. текста
    u8_t acDescr[128];
    u8_t acHelp[512];
  } dyn_buf;
} upvs_err_buf_t;

typedef s32_t (*dyntext_fn_t)(u8_t *, u32_t, const u8_t *);

typedef struct {
  // Код
  int code;
  // Приоритет
  int priority;
  // Неисправность
  //char* pcName;
  // Краткое описание для машиниста
  u8_t* pcDescribe;
  // Влияние на работу
  u8_t* pcInfluence;
  // Помощь для ПЭМа
  u8_t* pcHelp;
  // доп.функционал
  dyntext_fn_t pvDynDesc;
  dyntext_fn_t pvDynHelp;
} upvs_errdesc_t;

typedef struct upvs_err_st upvs_err_t;

// Объявления функций

// Функции создания, инициализации и удаления экземпляра упр. структуры
void *upvs_err__create(void);
int   upvs_err__init(upvs_err_t *);
void  upvs_err__del(upvs_err_t *);
// Предоставление доступа к параметрам
err_item_t *upvs_err__get_item(upvs_err_t *, u32_t);
s32_t upvs_err__get_free_pos(upvs_err_t *);
// Функции синхронизации доступа к разделяемым ресурсам
s32_t  upvs_err__lock(upvs_err_t *);
s32_t  upvs_err__unlock(upvs_err_t *);
// ???
#if   (defined CLT)
s32_t upvs_err__check_state(upvs_err_t *, u32_t, bool, s32_t);
upvs_err_buf_t *upvs_err__get_buf(upvs_err_t *, err_item_t *);
bool upvs_err__is_new(upvs_err_t *, u32_t);
void upvs_err__set_new(upvs_err_t *, u32_t, bool);
bool upvs_err__is_act(upvs_err_t *, u32_t);
void upvs_err__set_act(upvs_err_t *, u32_t, bool);
void upvs_err__reset(upvs_err_t *, u32_t);
s32_t upvs_err__get_code(upvs_err_t *, u32_t);
#elif (defined SRV)
u32_t upvs_err__get_idx(upvs_err_t *);
err_item_t *upvs_err__next(upvs_err_t *);
bool  upvs_err__is_help_equal(upvs_err_t *, s32_t , const u8_t *);
s32_t upvs_err__get_item_idx(upvs_err_t *, s32_t);
#endif

#endif //_UPVS_ERR_H_