#ifndef _UPVS_ERR_H_
#define _UPVS_ERR_H_

#include "proj_conf.h"
#include "upvs_conf.h"
#include "userint.h"
#include <stdbool.h>

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
} err_cell_t;

typedef struct {
  // аварийные записи
  err_cell_t  axCells[ERR_CAPACITY];
  // указатель на аварию в контексте текущего обмена по modbus tcp
  err_cell_t *pxCell;
} upvs_err_t;

void *upvs_err__init();

#endif //_UPVS_ERR_H_