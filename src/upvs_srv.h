#ifndef _UPVS_SRV_H_
#define _UPVS_SRV_H_

#include "proj_conf.h"
#include "upvs_conf.h"
#include "upvs_prm.h"
#include "upvs_err.h"
//#include "upvs_mqtt_srv.h"
#include "FreeRTOS.h"
#include "timers.h"

// cb in main.c
typedef void  (*upvs_cb_ptr_t)(void *);

typedef struct {
  bool bActive;
  upvs_cb_ptr_t pvCreated;
  upvs_cb_ptr_t pvDeleted;
  upvs_prm_t   *pxPrm;
  upvs_err_t   *pxErr;
  bool          bValid;
} upvs_srv_t;


// Общедоступные (public) функции

// Функции создания, инициализации, удаления экземпляра упр. структуры
void  *upvs_srv__create(void);
int    upvs_srv__init(upvs_srv_t *);
void   upvs_srv__del(upvs_srv_t *);
// Основные функции серверной части
int    upvs_srv__set( upvs_srv_t *, const u8_t *, u32_t, const u8_t * );
s32_t	 upvs_srv__get( upvs_srv_t *, u8_t *, u8_t *, u32_t, u32_t );
// Вспомогательные функции
// ...
// Функции-обёртки (wrappers)
#define get_prm(S, IDX) upvs_srv__get_prm(S, IDX)
prm_t *upvs_srv__get_prm(upvs_srv_t *, u32_t);
s32_t  upvs_srv__set_prm_b(upvs_srv_t *, u32_t, bool, bool);
s32_t  upvs_srv__set_prm_sl(upvs_srv_t *, u32_t, s32_t, bool);
s32_t  upvs_srv__set_prm_f(upvs_srv_t *, u32_t, f32_t, bool);
s32_t  upvs_srv__set_prm_all(upvs_srv_t *, u32_t, bool, bool);
s32_t  upvs_srv__get_prm_b(upvs_srv_t *, u32_t, bool *);
s32_t  upvs_srv__get_prm_sl(upvs_srv_t *, u32_t, s32_t *);
s32_t  upvs_srv__get_prm_f(upvs_srv_t *, u32_t, f32_t *);
bool   upvs_srv__is_prm_new(upvs_srv_t *, u32_t);
s32_t  upvs_srv__set_prm_new(upvs_srv_t *, u32_t, bool);
s32_t  upvs_srv__prm_lock(upvs_srv_t *);
void   upvs_srv__prm_unlock(upvs_srv_t *);
s32_t  upvs_srv__err_lock(upvs_srv_t *);
void   upvs_srv__err_unlock(upvs_srv_t *);

#endif //_UPVS_SRV_H_