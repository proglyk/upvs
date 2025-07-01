#ifndef _UPVS_SRV_H_
#define _UPVS_SRV_H_

#include "upvs_conf.h"
#include "upvs_prm.h"
#include "upvs_err.h"

typedef struct {
  bool bActive;
  upvs_param_t *pxPrm;
  upvs_err_t   *pxErr;
} upvs_srv_t;

void *upvs_srv__create(void);
int   upvs_srv__init(upvs_srv_t *);
void  upvs_srv__del(upvs_srv_t *);
// new
param_t *upvs_srv__get_prm(upvs_srv_t *self, u32_t idx);
int upvs_srv__set( upvs_srv_t *, const u8_t *, u32_t, const u8_t * );
s32_t	upvs_srv__get( upvs_srv_t *, u32_t, u8_t *, u8_t *, u32_t );

// setter
s32_t    upvs_srv__set_prm_b(upvs_srv_t *, u32_t, bool, bool);
s32_t    upvs_srv__set_prm_sl(upvs_srv_t *, u32_t, s32_t, bool);
s32_t    upvs_srv__set_prm_f(upvs_srv_t *, u32_t, f32_t, bool);
s32_t    upvs_srv__set_prm_all(upvs_srv_t *, u32_t, bool, bool);
// getter
s32_t    upvs_srv__get_prm_b(upvs_srv_t *, u32_t, bool *);
s32_t    upvs_srv__get_prm_sl(upvs_srv_t *, u32_t, s32_t *);
s32_t    upvs_srv__get_prm_f(upvs_srv_t *, u32_t, f32_t *);

#endif //_UPVS_SRV_H_