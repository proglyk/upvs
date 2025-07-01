#include "upvs_err.h"
#include <stdlib.h>
#include <string.h>

// Extern
//extern upvs_errdesc_t const *pxErrdesc;
extern const upvs_errdesc_t errdesc[];

// public
#if   (defined CLT)

#elif (defined SRV)
s32_t func_desc_1113_1307_1407_1506_1606( u8_t *, u32_t, const u8_t * );
s32_t func_desc_1202(u8_t *, u32_t, const u8_t *);
s32_t func_help_1103(u8_t *, u32_t, const u8_t *);
s32_t func_help_1106(u8_t *, u32_t, const u8_t *);
s32_t func_help_1112(u8_t *, u32_t, const u8_t *);
s32_t func_help_1113_1307_1407_1506_1606( u8_t *, u32_t, const u8_t * );
s32_t func_help_1114(u8_t *, u32_t, const u8_t *);
s32_t func_help_1202(u8_t *, u32_t, const u8_t *);
s32_t func_help_1304_1404(u8_t *, u32_t, const u8_t *);
s32_t func_help_1504(u8_t *, u32_t, const u8_t *);
s32_t func_help_1305_1405(u8_t *, u32_t, const u8_t *);
s32_t func_help_1102(u8_t *, u32_t, const u8_t *);
s32_t func_help_1107(u8_t *, u32_t, const u8_t *);
static void func_bits_1102_1107(u8_t *, u32_t);
s32_t func_help_1301(u8_t *, u32_t, const u8_t *);
s32_t func_help_1401(u8_t *, u32_t, const u8_t *);
s32_t func_help_1501(u8_t *, u32_t, const u8_t *);
s32_t func_help_1601(u8_t *, u32_t, const u8_t *);
#endif // CLT or SRV

// static
#if   (defined CLT)
static s32_t insert_item(upvs_err_t *, u32_t, s32_t);
static s32_t remove_item(upvs_err_t *, u32_t, s32_t);
#elif (defined SRV)
static u8_t *get_desc(u8_t *, upvs_errdesc_t *, u32_t);
static u8_t *get_help(u8_t *, upvs_errdesc_t *, u32_t);
static void func_bits_1301_1401_1501_1601( u8_t *, u32_t, const u8_t *, 
                                           const u8_t *, const u8_t *,
                                           const u8_t * );
#endif // CLT or SRV

// �������, ��������� �����

/**	----------------------------------------------------------------------------
	* @brief ??? */
void *
  upvs_err__create() {
/*----------------------------------------------------------------------------*/
  upvs_err_t *self = malloc(sizeof(upvs_err_t));
  if (!self) return NULL;
  return (void *)self;
}

/**	----------------------------------------------------------------------------
	* @brief ??? */
int
  upvs_err__init(upvs_err_t *self) {
/*----------------------------------------------------------------------------*/
  if (!self) return -1;
  self->paxDesc = &(errdesc[0]); //pxErrdesc;
  return 0;
}

/**	----------------------------------------------------------------------------
	* @brief */
void
  upvs_err__del(upvs_err_t *self) {
/*----------------------------------------------------------------------------*/
  if (!self) return;
  free(self);
}

/**	----------------------------------------------------------------------------
	* @brief ???
	* @retval error: ������ ���������� �������. */
s32_t
  upvs_err__get_free_pos(upvs_err_t *self) {
/*----------------------------------------------------------------------------*/
	if (!self) return -1;
  for (uint8_t i = 0; i < ITEMS_LENGHT; i++) {
		if (self->axItems[i].ulCode == 0) {
      return (int)i;
    }
	}
	return -1;
}

#if (defined CLT)

/**	----------------------------------------------------------------------------
	* @brief ???
	* @retval error: ������ ���������� �������. */
s32_t
  upvs_err__check_state(upvs_err_t *self, u32_t code, bool state, s32_t value) {
/*----------------------------------------------------------------------------*/
	if (!px) return -1;
  
  // �������, ��� ���� state==1 ������ �������� � ������ ������ � �� ��������
  // ����� ������ � ����� 'code' � ��������� 'value'.
  // �������, ��� ������ ��������� ���� state==0 � ������� ������
  if (state)
    insert_item(self, code, value);
  else
    remove_item(self, code, value);
  
	return 0;
}

#elif (defined SRV)

// ������� ����������� payload ���������� � �����

/**	----------------------------------------------------------------------------
	* @brief ��������� � ����������� � ��������� ������������� ��� (���� � � 
    ����������). ������������ ������ ����� ��������� ����������� � ������� */
s32_t
  upvs_err__get_buf(upvs_err_t *self, u32_t code, u32_t status) {
/*----------------------------------------------------------------------------*/
  upvs_errdesc_t *pxerr = NULL;
  int i=0;
  
  // ���� ������� � ����� "code"
  while ((self->paxDesc+i)->code != 0) {
    if ((self->paxDesc+i)->code == code)
      pxerr = (upvs_errdesc_t *)(self->paxDesc+i);
    i++;
  }
  if (!pxerr) return -1;
  
  // ��������� ���� xErrBuf
  self->xErrBuf.err.code = pxerr->code;
  self->xErrBuf.err.priority = pxerr->priority;
  //if (get_desc(xErrBuf.acDescrDyn, pxerr, status)) return NULL;
  self->xErrBuf.err.pcDescribe = get_desc(self->xErrBuf.dyn_buf.acDescr, pxerr, status);
  self->xErrBuf.err.pcInfluence = pxerr->pcInfluence;
  //if (get_help(xErrBuf.acHelpDyn, pxerr, status)) return NULL;
  self->xErrBuf.err.pcHelp = get_help(self->xErrBuf.dyn_buf.acHelp, pxerr, status);
  
  return 0;
}

/**	----------------------------------------------------------------------------
	* @brief */
err_item_t *
  upvs_err__get_item(upvs_err_t *self, u32_t idx) {
/*----------------------------------------------------------------------------*/  
  if ((!self)|(idx >= ITEMS_LENGHT)) return NULL;
  return &(self->axItems[idx]);
}

/**	----------------------------------------------------------------------------
	* @brief */
err_item_t *
  upvs_err__next(upvs_err_t *self) {
/*----------------------------------------------------------------------------*/  
  if (self->ulIdx >= ITEMS_LENGHT) self->ulIdx = 0;
  else self->ulIdx += 1;
  return &(self->axItems[self->ulIdx]);
}

/**	----------------------------------------------------------------------------
	* @brief ???
	* @retval error: ������ ���������� �������. */
s32_t
  upvs_err__get_item_idx(upvs_err_t *self, s32_t code) {
/*----------------------------------------------------------------------------*/
  if (!self) return -1;
  
  for (s32_t i = 0; i < ITEMS_LENGHT; i++) {
		if ( (self->axItems[i].ulCode == code) ) {
      return i;
    }
	}
	return -1;
}

/**	----------------------------------------------------------------------------
	* @brief ???
	* @retval error: ������ ���������� �������. */
bool
  upvs_err__is_help_equal(upvs_err_t *self, s32_t idx, const u8_t *pBuf) {
/*----------------------------------------------------------------------------*/
	if ( !strcmp((const char *)pBuf, (const char *)self->axItems[idx].acHelp) ) {
		return true;
	}
	return false;
}

// ������� ������������ ������ ���������� ���������

/**	----------------------------------------------------------------------------
	* @brief ���.���������� ��� 1113_1307_1407_1506_1606. */
s32_t
func_desc_1113_1307_1407_1506_1606(u8_t *pcDest, u32_t status, const u8_t *pcSrc) {
/*----------------------------------------------------------------------------*/
  //static const u8_t *acLow = "��������� ������ �� ������� �������� ���������� ";
  //static const u8_t *acHigh = "��������� ������ �� ���������� �������� ���������� ";
  static const u8_t *acLow = "������ ";
  static const u8_t *acHigh = "������� ";
  
  if ((!pcSrc)|(!pcDest)) return -1;
  //upvs_errdesc_t* px = (upvs_errdesc_t *)pvErrDesc;
  
  if (status == 0)
    strcpy((char *)(pcDest+0), (const char *)acLow);
  else
    strcpy((char *)(pcDest+0), (const char *)acHigh);
  strcpy((char *)(pcDest + strlen((const char *)pcDest)), (const char *)pcSrc);

  return 0;
}

/**	----------------------------------------------------------------------------
	* @brief ���.���������� ��� 1202. */
s32_t
func_desc_1202(u8_t *pcDest, u32_t status, const u8_t *pcSrc) {
/*----------------------------------------------------------------------------*/
  static const u8_t *acStart = "������������� ������������ ������ ";
  static const u8_t *acEnd = " ���";
  bool comma = false;
  
  if ((!pcSrc)|(!pcDest)) return -1;
  
  strcpy((char *)pcDest, (const char *)acStart);
  // ��������� ��������
  if (status & (1<<0)) {
    sprintf((char *)pcDest + strlen((const char *)pcDest), "%d", 1);
    comma = true;
  }
  if (status & (1<<1)) {
    if (comma) strcpy((char *)pcDest + strlen((const char *)pcDest), ",");
    sprintf((char *)pcDest + strlen((const char *)pcDest), "%d", 2);
    comma = true;
  }
  if (status & (1<<2)) {
    if (comma) strcpy((char *)pcDest + strlen((const char *)pcDest), ",");
    sprintf((char *)pcDest + strlen((const char *)pcDest), "%d", 3);
    comma = true;
  }
  if (status & (1<<3)) {
    if (comma) strcpy((char *)pcDest + strlen((const char *)pcDest), ",");
    sprintf((char *)pcDest + strlen((const char *)pcDest), "%d", 4);
  }
  strcpy((char *)pcDest + strlen((const char *)pcDest), (const char *)acEnd);

  return 0;
}

/**	----------------------------------------------------------------------------
	* @brief ���.���������� ��� 1102. ��������� ������ � ����������� �� ������ 
    ���������� ����� ���. */
s32_t
  func_help_1102(u8_t *pcDest, u32_t status, const u8_t *pcSrc) {
/*----------------------------------------------------------------------------*/
  static const char *acStartText = "��������� ������ ��������(-��) IGBT ";
  static const char *acFinalText = " ���. ���������� ��������� ������ IGBT "  \
    "������ �� ������� ����������� � �������� �� � ������ �������������; "    \
    "��������� ������� ������ � �������� ��� � ������ ��������� "             \
    "����������������";
  
  if ((!pcDest)|!(status & 0x3f)) return -1;
  // ��������� ����� ������
  strcpy((char *)pcDest, acStartText);
  // ������ ���, ������������� ��������� �����
  func_bits_1102_1107(pcDest, status);
  // �������� ����� ������
  strcpy((char *)(pcDest + strlen((const char *)pcDest)), acFinalText);

  return 0;
}

/**	----------------------------------------------------------------------------
	* @brief ���.���������� ��� 1107. ��������� ������ � ����������� �� ������ 
    ���������� ����� ���. */
s32_t
  func_help_1107(u8_t *pcDest, u32_t status, const u8_t *pcSrc) {
/*----------------------------------------------------------------------------*/
  static const char *acStartText = "���������� ������ ����� � ���������(-���)"\
    " IGBT ";
  static const char *acFinalText = " ���. ���������� ��������� ���������� "   \
    "����������� ���������� � ������ ���������; ������ ��������� ��������, "  \
    "���� �������� ���������; ��������� ����������� ��������� �� ������ "     \
    "� �������� �����������, ���� �������� ����� �������";
  
  if ((!pcDest)|!(status & 0x3f)) return -1;
  // ��������� ����� ������
  strcpy((char *)pcDest, acStartText);
  // ������ ���, ������������� ��������� �����
  func_bits_1102_1107(pcDest, status);
  // �������� ����� ������
  strcpy((char *)(pcDest + strlen((const char *)pcDest)), acFinalText);

  return 0;
}

/**	----------------------------------------------------------------------------
	* @brief ������������� ��������� �����
    ��� ������:   |.. b5 b4 b3 b2 b1 b0|
    ����� ���3�:  |   B2 B1 B2 B1 B2 B1|
    ���� �������: |   A8 A8 A7 A7 A6 A6| 
    ��������, ���� ������� ��� b0, �� ��������� ������ � ������ �1 (����� B1 
      ����� A6) */
/**	----------------------------------------------------------------------------
	* @brief ����������� ��������� ����� �� ���� */
static void
  func_bits_1102_1107(u8_t *pcDest, u32_t status) {
/*----------------------------------------------------------------------------*/
  static const char *acBlock = " ����� ";
  // ���������� ����� �� � ������ ������������� ��������� if ������� �������
  bool prev_exist = false;
  
  // ��������� ��������, ���� �6
  if (status & (1<<0)) {
    strcpy((char *)(pcDest + strlen((const char *)pcDest)), "B1");
    strcpy((char *)(pcDest + strlen((const char *)pcDest)), acBlock);
    strcpy((char *)(pcDest + strlen((const char *)pcDest)), "A6");
    prev_exist |= true;
  }
  if (status & (1<<1)) {
    if (prev_exist) {
      strcpy((char *)(pcDest + strlen((const char *)pcDest)), ", ");
    }
    strcpy((char *)(pcDest + strlen((const char *)pcDest)), "B2");
    strcpy((char *)(pcDest + strlen((const char *)pcDest)), acBlock);
    strcpy((char *)(pcDest + strlen((const char *)pcDest)), "A6");
    prev_exist |= true;
  }
  
  // ��������� ��������, ���� �7
  if (status & (1<<2)) {
    if (prev_exist) {
      strcpy((char *)(pcDest + strlen((const char *)pcDest)), ", ");
    }
    strcpy((char *)(pcDest + strlen((const char *)pcDest)), "B1");
    strcpy((char *)(pcDest + strlen((const char *)pcDest)), acBlock);
    strcpy((char *)(pcDest + strlen((const char *)pcDest)), "A7");
    prev_exist |= true;
  }
  if (status & (1<<3)) {
    if (prev_exist) {
      strcpy((char *)(pcDest + strlen((const char *)pcDest)), ", ");
    }
    strcpy((char *)(pcDest + strlen((const char *)pcDest)), "B2");
    strcpy((char *)(pcDest + strlen((const char *)pcDest)), acBlock);
    strcpy((char *)(pcDest + strlen((const char *)pcDest)), "A7");
    prev_exist |= true;
  }
  // ��������� ��������, ���� �8
  if (status & (1<<4)) {
    if (prev_exist) {
      strcpy((char *)(pcDest + strlen((const char *)pcDest)), ", ");
    }
    strcpy((char *)(pcDest + strlen((const char *)pcDest)), "B1");
    strcpy((char *)(pcDest + strlen((const char *)pcDest)), acBlock);
    strcpy((char *)(pcDest + strlen((const char *)pcDest)), "A8");
    prev_exist |= true;
  }
  if (status & (1<<5)) {
    if (prev_exist) {
      strcpy((char *)(pcDest + strlen((const char *)pcDest)), ", ");
    }
    strcpy((char *)(pcDest + strlen((const char *)pcDest)), "B2");
    strcpy((char *)(pcDest + strlen((const char *)pcDest)), acBlock);
    strcpy((char *)(pcDest + strlen((const char *)pcDest)), "A8");
    prev_exist |= true;
  }
}

/**	----------------------------------------------------------------------------
	* @brief ���.���������� ��� 1113_1307_1407_1506_1606. */
s32_t
func_help_1103(u8_t *pcDest, u32_t status, const u8_t *pcSrc) {
/*----------------------------------------------------------------------------*/
  static const u8_t *acOpt1 = "���������� ��������� � ������ ������� "        \
      "���������� A9 ��� (����������� ����� � ����). ";
  static const u8_t *acOpt2 = "���������� ��������� � ������ ������� "        \
      "���������� A9 ��� (������ ������ ����� � ����). ";
  
  if ((!pcSrc)|(!pcDest)) return -1;
  
  if (status == 0)
    strcpy((char *)(pcDest+0), (const char *)acOpt1);
  else
    strcpy((char *)(pcDest+0), (const char *)acOpt2);
  strcpy((char *)(pcDest + strlen((const char *)pcDest)), (const char *)pcSrc);

  return 0;
}

/**	----------------------------------------------------------------------------
	* @brief ���.���������� ��� 1113_1307_1407_1506_1606. */
s32_t
func_help_1106(u8_t *pcDest, u32_t status, const u8_t *pcSrc) {
/*----------------------------------------------------------------------------*/
  static const u8_t *acKm1 = "���������� ������������� ���������� "           \
      " KM1 (���������) ���. ";
  static const u8_t *acKm2 = "���������� ������������� ���������� "           \
      " KM2 (��������) ���. ";
  
  if ((!pcSrc)|(!pcDest)) return -1;
  
  if (status == 0)
    strcpy((char *)(pcDest+0), (const char *)acKm1);
  else
    strcpy((char *)(pcDest+0), (const char *)acKm2);
  strcpy((char *)(pcDest + strlen((const char *)pcDest)), (const char *)pcSrc);

  return 0;
}

/**	----------------------------------------------------------------------------
	* @brief ���.���������� ��� 1113_1307_1407_1506_1606. */
s32_t
func_help_1112(u8_t *pcDest, u32_t status, const u8_t *pcSrc) {
/*----------------------------------------------------------------------------*/
  static const u8_t *acLow = "��������� ������ �� ���������� �������� ����"   \
    " ��� (���������� ��������). ";
  static const u8_t *acHigh = "��������� ������ �� ���������� �������� ����"  \
    " ��� (���������� ��������). ";
  
  if ((!pcSrc)|(!pcDest)) return -1;
  
  if (status == 0)
    strcpy((char *)(pcDest+0), (const char *)acLow);
  else
    strcpy((char *)(pcDest+0), (const char *)acHigh);
  strcpy((char *)(pcDest + strlen((const char *)pcDest)), (const char *)pcSrc);

  return 0;
}

/**	----------------------------------------------------------------------------
	* @brief ���.���������� ��� 1113_1307_1407_1506_1606. */
s32_t
func_help_1113_1307_1407_1506_1606(u8_t *pcDest, u32_t status, const u8_t *pcSrc) {
/*----------------------------------------------------------------------------*/
  static const u8_t *acLow = "��������� ������ �� ������� �������� ���������� ";
  static const u8_t *acHigh = "��������� ������ �� ���������� �������� ���������� ";
  
  if ((!pcSrc)|(!pcDest)) return -1;
  
  if (status == 0)
    strcpy((char *)(pcDest+0), (const char *)acLow);
  else
    strcpy((char *)(pcDest+0), (const char *)acHigh);
  strcpy((char *)(pcDest + strlen((const char *)pcDest)), (const char *)pcSrc);

  return 0;
}

/**	----------------------------------------------------------------------------
	* @brief ���.���������� ��� 1113_1307_1407_1506_1606. */
s32_t
func_help_1114(u8_t *pcDest, u32_t status, const u8_t *pcSrc) {
/*----------------------------------------------------------------------------*/
  static const u8_t *acLow = "��������� ������ �� ���������� ��������� ����"  \
    " ��� (���������� ��������). ";
  static const u8_t *acHigh = "��������� ������ �� ���������� ��������� ����" \
    " ��� (���������� ��������). ";
   
  if ((!pcSrc)|(!pcDest)) return -1;
  
  if (status == 0)
    strcpy((char *)(pcDest+0), (const char *)acLow);
  else
    strcpy((char *)(pcDest+0), (const char *)acHigh);
  strcpy((char *)(pcDest + strlen((const char *)pcDest)), (const char *)pcSrc);

  return 0;
}

/**	----------------------------------------------------------------------------
	* @brief ���.���������� ��� 1202. */
s32_t
func_help_1202(u8_t *pcDest, u32_t status, const u8_t *pcSrc) {
/*----------------------------------------------------------------------------*/
  static const u8_t *acStart = "�������������� ����� ��������� ���� � "       \
    "��������� ���������� �������������";
  static const u8_t *acOne = " ������ 1 (����������)";
  static const u8_t *acTwo = " ������ 2 (����������)";
  static const u8_t *acThree = " ������ 3 (�����������)";
  static const u8_t *acFour = " ������ 4 (��������)";
  static const u8_t *acEnd = " ��� � ������ ������";
  bool comma = false;
   
  if ((!pcSrc)|(!pcDest)) return -1;

  strcpy((char *)pcDest, (const char *)acStart);
  // ��������� ��������
  if (status & (1<<0)) {
    strcpy((char *)pcDest + strlen((const char *)pcDest), (const char *)acOne);
    comma = true;
  }
  if (status & (1<<1)) {
    if (comma) strcpy((char *)pcDest + strlen((const char *)pcDest), ",");
    strcpy((char *)pcDest + strlen((const char *)pcDest), (const char *)acTwo);
    comma = true;
  }
  if (status & (1<<2)) {
    if (comma) strcpy((char *)pcDest + strlen((const char *)pcDest), ",");
    strcpy((char *)pcDest + strlen((const char *)pcDest), (const char *)acThree);
    comma = true;
  }
  if (status & (1<<3)) {
    if (comma) strcpy((char *)pcDest + strlen((const char *)pcDest), ",");
    strcpy((char *)pcDest + strlen((const char *)pcDest), (const char *)acFour);
  }
  strcpy((char *)pcDest + strlen((const char *)pcDest), (const char *)acEnd);
  strcpy((char *)(pcDest + strlen((const char *)pcDest)), (const char *)pcSrc);

  return 0;
}

/**	----------------------------------------------------------------------------
	* @brief ���.���������� ��� 1301. ��������� ������ � ����������� �� ������ 
    ���������� �������� ������ 1 */
s32_t
func_help_1301(u8_t *pcDest, u32_t status, const u8_t *pcSrc) {
/*----------------------------------------------------------------------------*/
  static const char *acStartText = "��������� ������ ��������(-��) IGBT ";
  static const char *acFinalText = " �����a 1 (��������� �����������) ���. "  \
    "���������� ��������� ��������� ������� IGBT � �������� �� � ������ "     \
    "�������������; ��������� ����� �������� IGBT � �������� � � ������ "    \
    "��������� ����������������; ��������� ���������� � ������� ����������� " \
    "�� ���������� ��������� ���������";
  
  if ((!pcDest)|!(status & 0xff)) return -1;
  // ��������� ����� ������
  strcpy((char *)pcDest, acStartText);
  // ������ ���, ������������� ��������� �����
  // ��������� ��������� �������� �����
  func_bits_1301_1401_1501_1601(pcDest, status, "A20", "A21", "A22", NULL);
  // �������� ����� ������
  strcpy((char *)(pcDest + strlen((const char *)pcDest)), acFinalText);

  return 0;
}

/**	----------------------------------------------------------------------------
	* @brief ���.���������� ��� 1401. ��������� ������ � ����������� �� ������ 
    ���������� �������� ������ 2 */
s32_t
func_help_1401(u8_t *pcDest, u32_t status, const u8_t *pcSrc) {
/*----------------------------------------------------------------------------*/
  static const char *acStartText = "��������� ������ ��������(-��) IGBT ";
  static const char *acFinalText = " �����a 2 (��������� �����������) ���. "  \
    "���������� ��������� ��������� ������� IGBT � �������� �� � ������ "     \
    "�������������; ��������� ����� �������� IGBT � �������� � � ������ "    \
    "��������� ����������������; ��������� ���������� � ������� ����������� " \
    "�� ���������� ��������� ���������";
  
  if ((!pcDest)|!(status & 0xff)) return -1;
  // ��������� ����� ������
  strcpy((char *)pcDest, acStartText);
  // ������ ���, ������������� ��������� �����
  // ��������� ��������� �������� �����
  func_bits_1301_1401_1501_1601(pcDest, status, "A23", "A24", "A25", NULL);
  // �������� ����� ������
  strcpy((char *)(pcDest + strlen((const char *)pcDest)), acFinalText);

  return 0;
}

/**	----------------------------------------------------------------------------
	* @brief ���.���������� ��� 1501. ��������� ������ � ����������� �� ������ 
    ���������� �������� ������ 2 */
s32_t
func_help_1501(u8_t *pcDest, u32_t status, const u8_t *pcSrc) {
/*----------------------------------------------------------------------------*/
  static const char *acStartText = "��������� ������ ��������(-��) IGBT ";
  static const char *acFinalText = " �����a 3 (����������� ������) ���. "     \
    "���������� ��������� ��������� ������� IGBT � �������� �� � ������ "     \
    "�������������; ��������� ����� �������� IGBT � �������� � � ������ "    \
    "��������� ����������������; ��������� ���������� � ������� ����������� " \
    "�� ���������� ��������� ���������";
  
  if ((!pcDest)|!(status & 0xff)) return -1;
  // ��������� ����� ������
  strcpy((char *)pcDest, acStartText);
  // ������ ���, ������������� ��������� �����
  // ��������� ��������� �������� �����
  func_bits_1301_1401_1501_1601(pcDest, status, "A28", "A29", "A26", "A27");
  // �������� ����� ������
  strcpy((char *)(pcDest + strlen((const char *)pcDest)), acFinalText);

  return 0;
}

/**	----------------------------------------------------------------------------
	* @brief ���.���������� ��� 1501. ��������� ������ � ����������� �� ������ 
    ���������� �������� ������ 2 */
s32_t
func_help_1601(u8_t *pcDest, u32_t status, const u8_t *pcSrc) {
/*----------------------------------------------------------------------------*/
  static const char *acStartText = "��������� ������ ��������(-��) IGBT ";
  static const char *acFinalText = " �����a 4 (����� ��) ���. "               \
    "���������� ��������� ��������� ������� IGBT � �������� �� � ������ "     \
    "�������������; ��������� ����� �������� IGBT � �������� � � ������ "    \
    "��������� ����������������; ��������� ���������� � ������� ����������� " \
    "�� ���������� ��������� ���������";
  
  if ((!pcDest)|!(status & 0xff)) return -1;
  // ��������� ����� ������
  strcpy((char *)pcDest, acStartText);
  // ������ ���, ������������� ��������� �����
	// ��������� ��������� �������� �����
  func_bits_1301_1401_1501_1601(pcDest, status, "A19", "A17", "A18", NULL);
  // �������� ����� ������
  strcpy((char *)(pcDest + strlen((const char *)pcDest)), acFinalText);

  return 0;
}

/**	----------------------------------------------------------------------------
	* @brief ������������� ��������� �����
    ��� ������:   |... b7  b6  b5  b4  b3  b2  b1  b0 |
    ��� ������:   |    POW POW POW POW FLT FLT FLT FLT|
    ���� �������: |    #4  #3  #2  #1  #4  #3  #2  #1 |
    ��������, ���� ������� ��� b0, �� ��������� ������ �� FLT � ����� �1 
      (����� A20 ������ 1 / ����� A23 ������ 2 � �.�.).
    ���� ������� ��� b4, �� ��������� ������ �� ������� POW � ����� �1 � �.�.
      */
/**	----------------------------------------------------------------------------
	* @brief ����������� ��������� ����� �� ���� */
static void
  func_bits_1301_1401_1501_1601(u8_t *pcDest, u32_t status, const u8_t *pcName1,
  const u8_t *pcName2, const u8_t *pcName3, const u8_t *pcName4) {
/*----------------------------------------------------------------------------*/
  // ���������� ����� �� � ������ ������������� ��������� if ������� �������
  bool prev_exist = false;
  
  // ��������� ��������, ������� FLT ��� POW ����� ���2� �1 (pcName1) ������ X
  if ((status & (1<<0)) | (status & (1<<4))) {
    strcpy((char *)(pcDest + strlen((const char *)pcDest)), (char *)pcName1);
    prev_exist |= true;
  }
  // ��������� ��������, ������� FLT ��� POW ����� ���2� �2 (pcName2) ������ X
  if ((status & (1<<1)) | (status & (1<<5))) {
    if (prev_exist) {
      strcpy((char *)(pcDest + strlen((const char *)pcDest)), ", ");
    }
    strcpy((char *)(pcDest + strlen((const char *)pcDest)), (char *)pcName2);
    prev_exist |= true;
  }
  // ��������� ��������, ������� FLT ��� POW ����� ���2� �3 (pcName3) ������ X
  if ((status & (1<<2)) | (status & (1<<6))) {
    if (prev_exist) {
      strcpy((char *)(pcDest + strlen((const char *)pcDest)), ", ");
    }
    strcpy((char *)(pcDest + strlen((const char *)pcDest)), (char *)pcName3);
    prev_exist |= true;
  }
  if (!pcName4) return;
  // ��������� ��������, ������� FLT ��� POW ����� ���2� �4 (pcName4) ������ X
  if ((status & (1<<3)) | (status & (1<<7))) {
    if (prev_exist) {
      strcpy((char *)(pcDest + strlen((const char *)pcDest)), ", ");
    }
    strcpy((char *)(pcDest + strlen((const char *)pcDest)), (char *)pcName4);
  }
}

/**	----------------------------------------------------------------------------
	* @brief ���.���������� ��� 1202. */
s32_t
func_help_1304_1404(u8_t *pcDest, u32_t status, const u8_t *pcSrc) {
/*----------------------------------------------------------------------------*/
  static const u8_t *acStart1 = "���������� ������������ ���� ������ � ";
  static const u8_t *acStart2 = " ���� � ������� ";
  static const u8_t *acEnd = " c. ���������� ��������� �������� ����� ����������" \
    " ��������� ������������; ��������� ����������� ����� ��������";
  
  if ((!pcSrc)|(!pcDest)) return -1;

  strcpy((char *)pcDest, (const char *)pcSrc);
  strcpy((char *)pcDest + strlen((const char *)pcDest), (const char *)acStart1);
  // ��������� ��������. ������ �����
  if (status & (1<<0)) {
    sprintf((char *)pcDest + strlen((const char *)pcDest), "%.2f", 1.25f);
  }
  else if (status & (1<<1)) {
    sprintf((char *)pcDest + strlen((const char *)pcDest), "%.2f", 2.0f);
  }
  else if (status & (1<<2)) {
    sprintf((char *)pcDest + strlen((const char *)pcDest), "%.2f", 2.5f);
  }
  else if (status & (1<<3)) {
    sprintf((char *)pcDest + strlen((const char *)pcDest), "%.2f", 3.0f);
  }
  strcpy((char *)pcDest + strlen((const char *)pcDest), (const char *)acStart2);
  // ��������� ��������. ������ �����
  if (status & (1<<0)) {
    sprintf((char *)pcDest + strlen((const char *)pcDest), "%.1f", 10.0f);
  }
  else if (status & (1<<1)) {
    sprintf((char *)pcDest + strlen((const char *)pcDest), "%.1f", 2.0f);
  }
  else if (status & (1<<2)) {
    sprintf((char *)pcDest + strlen((const char *)pcDest), "%.1f", 1.0f);
  }
  else if (status & (1<<3)) {
    sprintf((char *)pcDest + strlen((const char *)pcDest), "%.1f", 0.1f);
  }
  strcpy((char *)pcDest + strlen((const char *)pcDest), (const char *)acEnd);

  return 0;
}

/**	----------------------------------------------------------------------------
	* @brief ���.���������� ��� 1504. */
s32_t
func_help_1504(u8_t *pcDest, u32_t status, const u8_t *pcSrc) {
/*----------------------------------------------------------------------------*/
  static const u8_t *acStart1 = "���������� ������������ ���� ������ � ";
  static const u8_t *acStart2 = " ���� � ������� ";
  static const u8_t *acEnd = " c. ���������� ��������� �������� ����� ����������" \
    " ��������� ������������; ��������� ����������� ����� ��������";
  
  if ((!pcSrc)|(!pcDest)) return -1;

  strcpy((char *)pcDest, (const char *)pcSrc);
  // ������ �����
  strcpy((char *)pcDest + strlen((const char *)pcDest), (const char *)acStart1);
  if (status & (1<<0))
    sprintf((char *)pcDest + strlen((const char *)pcDest), "%.2f", 2.0f);
  else if (status & (1<<1))
    sprintf((char *)pcDest + strlen((const char *)pcDest), "%.2f", 2.5f);
  else if (status & (1<<2))
    sprintf((char *)pcDest + strlen((const char *)pcDest), "%.2f", 3.0f);
  else
    sprintf((char *)pcDest + strlen((const char *)pcDest), "%.2f", 0.0f);

  // ������ �����
  strcpy((char *)pcDest + strlen((const char *)pcDest), (const char *)acStart2);
  if (status & (1<<0))
    sprintf((char *)pcDest + strlen((const char *)pcDest), "%.1f", 2.0f);
  else if (status & (1<<1))
    sprintf((char *)pcDest + strlen((const char *)pcDest), "%.1f", 1.0f);
  else if (status & (1<<2))
    sprintf((char *)pcDest + strlen((const char *)pcDest), "%.1f", 0.5f);
  else
    sprintf((char *)pcDest + strlen((const char *)pcDest), "%.2f", 0.0f);

  strcpy((char *)pcDest + strlen((const char *)pcDest), (const char *)acEnd);

  return 0;
}

/**	----------------------------------------------------------------------------
	* @brief ���.���������� ��� 1305_1405. */
s32_t
func_help_1305_1405(u8_t *pcDest, u32_t status, const u8_t *pcSrc) {
/*----------------------------------------------------------------------------*/
  static const u8_t *acStart = "��������� ������ �� ����������� ��������� ����" \
    "(�����) ���� ";
  bool comma = false;
   
  if ((!pcSrc)|(!pcDest)) return -1;

  strcpy((char *)pcDest, (const char *)acStart);
  // ��������� ��������
  if (status & (1<<0)) {
    strcpy((char *)pcDest + strlen((const char *)pcDest), (const char *)"U");
    comma = true;
  }
  if (status & (1<<1)) {
    if (comma) strcpy((char *)pcDest + strlen((const char *)pcDest), "/");
    strcpy((char *)pcDest + strlen((const char *)pcDest), (const char *)"V");
    comma = true;
  }
  if (status & (1<<2)) {
    if (comma) strcpy((char *)pcDest + strlen((const char *)pcDest), "/");
    strcpy((char *)pcDest + strlen((const char *)pcDest), (const char *)"W");
  }
  strcpy((char *)(pcDest + strlen((const char *)pcDest)), (const char *)pcSrc);

  return 0;
}


#endif // SRV


// �������, ������������ �������� ��������� ������� �����

#if (defined CLT)

/**	----------------------------------------------------------------------------
	* @brief ??? */
static s32_t
  insert_item(upvs_err_t *self, u32_t code, s32_t value) {
/*----------------------------------------------------------------------------*/
  s32_t idx = -1, i = 0;
  if (!self) return -1;
  
  // ���� ���������� �� 'code' � 'value'.
  // ���� ������ � ������ ���������� ����������, �� ������� idx=-1
  // ���� ������ ������������, �� �������� idx �������� �� '-1' � �������
  for (i = 0; i < ITEMS_LENGHT; i++) {
		if ( (self->axItems[i].ulCode == code) & (self->axItems[i].slValue == value) )
    {
      idx = i;
    }
	}
  if (idx != -1) return 0;
  
  // ���� ��������� ������� � ��
  if ((idx = upvs_err__get_free_pos(self)) == -1) {
    DBG_PRINT( NET_DEBUG, ("Can't get free pos for %04d, in '%s' /UPVS/upvs_clt.c:%d\r\n", 
      code, __FUNCTION__, __LINE__) );
    return -1;
  }
  
  // ��������� �������� ���� �� ������� idx:
  // ��������� ��������� 'code' � 'value', �������� ������ ��� '��������' � 
  // '�����'. ��� � upvs_clt_sess.c ��������� �� ���� ����� �� � ������ �� 
  // ���������� ����� bNew � ��������� - ���� ����� ����������, �� ������� 
  // ����� Error (1 ���), ����� ���� ���� ���������, �� ���� ulCode � slValue 
  // �� ��������� � ������ � ����� �������� "�����" �� ������� ������ ������
  self->axItems[idx].ulCode = code;
  self->axItems[idx].slValue = value;
  self->axItems[idx].bActive = true;
  self->axItems[idx].bNew = true;
  
  return 0;
}

/**	----------------------------------------------------------------------------
	* @brief ??? */
static s32_t
  remove_item(upvs_err_t *self, u32_t code, s32_t value) {
/*----------------------------------------------------------------------------*/
  s32_t idx = -1, i = 0;
  //u8_t i = 0;
  if (!self) return -1;

  // ���� ������ � ������� ���, �� ������� � �������
  //if ( (idx = errdb_get_cell_id(px, code, value)) < 0 ) return -1;
  for (i = 0; i < ITEMS_LENGHT; i++) {
		if ( self->axItems[i].ulCode == code ) {
      idx = i;
    }
	}
  if (idx == -1) return -1;
  
  // �������� ���������� ������. ��������������� �������� ���������
  // ������ � ��������� ����� do_session(..), � �� �����
  self->axItems[idx].bActive = false;
  self->axItems[idx].bNew = true;
  
  return 0;
}

#elif (defined SRV)
  
/**	----------------------------------------------------------------------------
	* @brief ??? */
static u8_t*
get_desc(u8_t *pcDest, upvs_errdesc_t* px, u32_t status) {
/*----------------------------------------------------------------------------*/
    u8_t *pc = NULL;
  
  if (px->pvDynDesc) {
    if ( px->pvDynDesc(pcDest, status, px->pcDescribe) == 0 )
      pc = pcDest;
  } else {
    pc = (u8_t *)px->pcDescribe;
  }  
  
  return pc;
}

/**	----------------------------------------------------------------------------
	* @brief ??? */
static u8_t*
get_help(u8_t *pcDest, upvs_errdesc_t* px, u32_t status) {
/*----------------------------------------------------------------------------*/
  u8_t *pc = NULL;
  
  if (px->pvDynHelp) {
    if ( px->pvDynHelp(pcDest, status, px->pcHelp) == 0 )
      pc = pcDest;
  } else {
    pc = (u8_t *)px->pcHelp;
  }  
  
  return pc;
}


#endif // CLT or SRV

