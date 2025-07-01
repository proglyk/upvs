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
  // ���. ������ � server
  GETALL
} type_t;

// ���������� ��� ���. ������

// ���������� �� ����� ���, ��� � mag ���� � �.�. 'u8_t *pc'
typedef struct {
  // ��������
  struct {
    u8_t *pc;
    bool b;
    s32_t sl;
    float f;
  } mag;
  // ���
  type_t type;
} value_ptr_t;

typedef struct {
  // ��� ��������� (����)
  u8_t *pcName;
  // ��������� �� ������ ���� � ����
  u8_t *pcTopic;
  // �������� ��������� (����� ���������� � ����������� �� ����)
  value_ptr_t xValue;
} param_ptr_t;

typedef struct {
  // ������ ���� � ����
  u8_t acTopic[UPVS_TOPICPATH_SIZE];
  param_ptr_t xParam;
  //error_ptr_t xError;
} parser_t;
// ���������� �� ����� ���, ��� � mag ���� � �.�. 'u8_t *pc'

// ���������� ��� ���. �����

typedef struct {
  // ��������
  struct {
    u8_t ac[UPVS_PARAM_VALUE_SIZE];
    bool b;
    s32_t sl;
    float f;
  } mag;
  // ���
  type_t type;
  bool bNew; // ���������� ����������, ������ ��� "��������" (actions)
} value_t;

typedef struct {
  // ������
  //const u8_t *pcSrvc;
  // ��� ��������� (����)
  //const char *pcName;
  // ������ ���� � ����
  const u8_t *pcTitle;  // ������ ��������� �����
  const u8_t *pcSrvc;   // ������ ������
  const u8_t *pcName;   // ��� ��������� (����)
  const u8_t *pcTopic;  // ������ ����
  value_t xValue;       // �������� ���������
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