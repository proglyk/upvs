#ifndef UPVS_CLT_H
#define UPVS_CLT_H

#include "userint.h"
#include <stdbool.h>
#include "upvs_prm.h"
#include "upvs_err.h"

typedef struct {
  u32_t Digit;  // 001..999
  u32_t Month;  // 01..12
  u32_t Year;   // ...
} ser_num_t;

typedef struct {
  //param_ptr_t *pxParamPtrList
  bool bReqSendAll;
  upvs_param_t *pxPrm;
  upvs_err_t   *pxErr;
  //upvs_errdb_cell_t axErrDb[ERRDB_LENGHT];
  u8_t acBuf[20];
} upvs_clt_t;

// Функции создания, инициализации, удаления экземпляра упр. структуры
void *upvs_clt__create(void);
int   upvs_clt__init(upvs_clt_t *);
void  upvs_clt__del(upvs_clt_t *);
// Основные функции клиентской части
int   upvs_clt__set(upvs_clt_t *, const u8_t *, u32_t, const u8_t *, u32_t);
s32_t upvs_clt__get_prm(upvs_clt_t *, u8_t *, u8_t *, u32_t);
int   upvs_clt__get_err(upvs_clt_t *, u8_t *, u8_t *, u32_t);
// Вспомогательные функции
void  upvs_clt__set_sernum(u8_t *, ser_num_t *);
void  upvs_clt__set_mac(u8_t *, u8_t *, u32_t);
// Функции-обёртки (wrappers)
bool  upvs_clt__is_err_new(upvs_clt_t *, u32_t);
void  upvs_clt__set_err_new(upvs_clt_t *, u32_t, bool);
bool  upvs_clt__is_err_act(upvs_clt_t *, u32_t);
void  upvs_clt__set_err_act(upvs_clt_t *, u32_t, bool);
void  upvs_clt__reset_err(upvs_clt_t *, u32_t);
s32_t upvs_clt__get_err_code(upvs_clt_t *, u32_t);
bool  upvs_clt__get_prm_new(upvs_clt_t *, u32_t);
s32_t upvs_clt__edit_prm_new(upvs_clt_t *, u32_t, bool);

#endif