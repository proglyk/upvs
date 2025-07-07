#include "upvs_err.h"
#include <stdlib.h>
#include <string.h>

struct upvs_err_st {
  // аварийные записи
  err_item_t     axItems[UPVS_ERR_LIST_LENGHT];
  // указатель на аварию в контексте текущего обмена по modbus tcp
  u32_t          ulIdx;
  upvs_err_buf_t xErrBuf;
#if (defined CLT)
  upvs_errdesc_t const *paxDesc;
#endif
};

#if (defined CLT)
extern const upvs_errdesc_t errdesc[];
#endif

// public
#if   (defined CLT)
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
#elif (defined SRV)

#endif // CLT or SRV

// static
#if   (defined CLT)
static s32_t insert_item(upvs_err_t *, u32_t, s32_t);
static s32_t remove_item(upvs_err_t *, u32_t, s32_t);
static u8_t *get_desc(u8_t *, upvs_errdesc_t *, u32_t);
static u8_t *get_help(u8_t *, upvs_errdesc_t *, u32_t);
static void func_bits_1301_1401_1501_1601( u8_t *, u32_t, const u8_t *, 
                                           const u8_t *, const u8_t *,
                                           const u8_t * );
#elif (defined SRV)

#endif // CLT or SRV

// Функции, доступные извне

/**	----------------------------------------------------------------------------
	* @brief ??? */
void *
  upvs_err__create(void) {
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
  
#if (defined CLT)
  self->paxDesc = &(errdesc[0]); //pxErrdesc;
#endif
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
	* @brief */
err_item_t *
  upvs_err__get_item(upvs_err_t *self, u32_t idx) {
/*----------------------------------------------------------------------------*/  
  if ((!self)|(idx >= UPVS_ERR_LIST_LENGHT)) return NULL;
  return &(self->axItems[idx]);
}

/**	----------------------------------------------------------------------------
	* @brief ???
	* @retval error: Статус выполнения функции. */
s32_t
  upvs_err__get_free_pos(upvs_err_t *self) {
/*----------------------------------------------------------------------------*/
	if (!self) return -1;
  for (uint8_t i = 0; i < UPVS_ERR_LIST_LENGHT; i++) {
		if (self->axItems[i].ulCode == 0) {
      return (int)i;
    }
	}
	return -1;
}

#if (defined CLT)

/**	----------------------------------------------------------------------------
	* @brief ???
	* @retval error: Статус выполнения функции. */
s32_t
  upvs_err__check_state(upvs_err_t *self, u32_t code, bool state, s32_t value) {
/*----------------------------------------------------------------------------*/
	if (!self) return -1;
  
  // Считаем, что пока state==1 авария авктивна и делаем запрос к БД добавить
  // новую запись с кодом 'code' и значением 'value'.
  // Считаем, что авария устранена если state==0 и удаляем запись
  if (state)
    insert_item(self, code, value);
  else
    remove_item(self, code, value);
  
	return 0;
}

/**	----------------------------------------------------------------------------
	* @brief Сообщение с информацией о возникшей неисправности ПВС (либо о её 
    устранении). Используется только после успешного подключения к брокеру */
upvs_err_buf_t *
  upvs_err__get_buf(upvs_err_t *self, err_item_t *item) {
/*----------------------------------------------------------------------------*/
  upvs_errdesc_t *pxerr = NULL;
  int i=0;
  
  // Ищем позицию с кодом "code"
  while ((self->paxDesc+i)->code != 0) {
    if ((self->paxDesc+i)->code == item->ulCode)
      pxerr = (upvs_errdesc_t *)(self->paxDesc+i);
    i++;
  }
  if (!pxerr) return NULL;
  
  // заполняем поля xErrBuf
  self->xErrBuf.err.code = pxerr->code;
  self->xErrBuf.err.priority = pxerr->priority;
  //if (get_desc(xErrBuf.acDescrDyn, pxerr, status)) return NULL;
  self->xErrBuf.err.pcDescribe = get_desc( self->xErrBuf.dyn_buf.acDescr, pxerr,
                                           item->bActive );
  self->xErrBuf.err.pcInfluence = pxerr->pcInfluence;
  //if (get_help(xErrBuf.acHelpDyn, pxerr, status)) return NULL;
  self->xErrBuf.err.pcHelp = get_help( self->xErrBuf.dyn_buf.acHelp, pxerr,
                                       item->bActive);
  
  return &self->xErrBuf;
}

/**	----------------------------------------------------------------------------
	* @brief */
bool
  upvs_err__is_new(upvs_err_t *self, u32_t idx) {
/*----------------------------------------------------------------------------*/  
  if ((!self)|(idx >= UPVS_ERR_LIST_LENGHT)) return NULL;
  return self->axItems[idx].bNew ? true : false;
}

/**	----------------------------------------------------------------------------
	* @brief */
void
  upvs_err__set_new(upvs_err_t *self, u32_t idx, bool sta) {
/*----------------------------------------------------------------------------*/  
  if ((!self)|(idx >= UPVS_ERR_LIST_LENGHT)) return;
  self->axItems[idx].bNew = sta;
}

/**	----------------------------------------------------------------------------
	* @brief */
bool
  upvs_err__is_act(upvs_err_t *self, u32_t idx) {
/*----------------------------------------------------------------------------*/  
  if ((!self)|(idx >= UPVS_ERR_LIST_LENGHT)) return NULL;
  return self->axItems[idx].bActive ? true : false;
}

/**	----------------------------------------------------------------------------
	* @brief */
void
  upvs_err__set_act(upvs_err_t *self, u32_t idx, bool sta) {
/*----------------------------------------------------------------------------*/  
  if ((!self)|(idx >= UPVS_ERR_LIST_LENGHT)) return;
  self->axItems[idx].bActive = sta;
}

/**	----------------------------------------------------------------------------
	* @brief */
void
  upvs_err__reset(upvs_err_t *self, u32_t idx) {
/*----------------------------------------------------------------------------*/  
  if ((!self)|(idx >= UPVS_ERR_LIST_LENGHT)) return;
  self->axItems[idx].ulCode = 0;
  self->axItems[idx].slValue = 0;
}

/**	----------------------------------------------------------------------------
	* @brief */
s32_t
  upvs_err__get_code(upvs_err_t *self, u32_t idx) {
/*----------------------------------------------------------------------------*/  
  if ((!self)|(idx >= UPVS_ERR_LIST_LENGHT)) return -1;
  return (s32_t)self->axItems[idx].ulCode;
}

// Функции формирования текста аварийного сообщения

/**	----------------------------------------------------------------------------
	* @brief Доп.функционал для 1113_1307_1407_1506_1606. */
s32_t
func_desc_1113_1307_1407_1506_1606(u8_t *pcDest, u32_t status, const u8_t *pcSrc) {
/*----------------------------------------------------------------------------*/
  //static const u8_t *acLow = "Сработала защита по низкому входному напряжению ";
  //static const u8_t *acHigh = "Сработала защита по превышению входного напряжения ";
  static const u8_t *acLow = "Низкое ";
  static const u8_t *acHigh = "Высокое ";
  
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
	* @brief Доп.функционал для 1202. */
s32_t
func_desc_1202(u8_t *pcDest, u32_t status, const u8_t *pcSrc) {
/*----------------------------------------------------------------------------*/
  static const u8_t *acStart = "Неисправность оборудования Канала ";
  static const u8_t *acEnd = " НСП";
  bool comma = false;
  
  if ((!pcSrc)|(!pcDest)) return -1;
  
  strcpy((char *)pcDest, (const char *)acStart);
  // побитовая проверка
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
	* @brief Доп.функционал для 1102. Формирует строку в зависимости от номера 
    аварийного блока ВСП. */
s32_t
  func_help_1102(u8_t *pcDest, u32_t status, const u8_t *pcSrc) {
/*----------------------------------------------------------------------------*/
  static const char *acStartText = "Сработала защита драйвера(-ов) IGBT ";
  static const char *acFinalText = " ВСП. Необходимо проверить модули IGBT "  \
    "ячейки на наличие повреждений и заменить их в случае неисправности; "    \
    "проверить драйвер ячейки и заменить его в случае нарушения "             \
    "функционирования";
  
  if ((!pcDest)|!(status & 0x3f)) return -1;
  // Начальная часть текста
  strcpy((char *)pcDest, acStartText);
  // сверка бит, идентификация аварийной платы
  func_bits_1102_1107(pcDest, status);
  // Конечная часть текста
  strcpy((char *)(pcDest + strlen((const char *)pcDest)), acFinalText);

  return 0;
}

/**	----------------------------------------------------------------------------
	* @brief Доп.функционал для 1107. Формирует строку в зависимости от номера 
    аварийного блока ВСП. */
s32_t
  func_help_1107(u8_t *pcDest, u32_t status, const u8_t *pcSrc) {
/*----------------------------------------------------------------------------*/
  static const char *acStartText = "Обнаружена потеря связи с драйвером(-ами)"\
    " IGBT ";
  static const char *acFinalText = " ВСП. Необходимо проверить надежность "   \
    "подключения световодов к платам драйверов; заново выполнить разделку, "  \
    "либо заменить световоды; проверить целостность элементов на платах "     \
    "и заменить неисправные, либо заменить платы целиком";
  
  if ((!pcDest)|!(status & 0x3f)) return -1;
  // Начальная часть текста
  strcpy((char *)pcDest, acStartText);
  // сверка бит, идентификация аварийной платы
  func_bits_1102_1107(pcDest, status);
  // Конечная часть текста
  strcpy((char *)(pcDest + strlen((const char *)pcDest)), acFinalText);

  return 0;
}

/**	----------------------------------------------------------------------------
	* @brief Идентификация аварийной платы
    Бит аварии:   |.. b5 b4 b3 b2 b1 b0|
    Плата ПДВ3К:  |   B2 B1 B2 B1 B2 B1|
    Блок силовой: |   A8 A8 A7 A7 A6 A6| 
    Например, если активен бит b0, то случилась авария в ячейке №1 (плата B1 
      блока A6) */
/**	----------------------------------------------------------------------------
	* @brief Определение аварийной платы по биту */
static void
  func_bits_1102_1107(u8_t *pcDest, u32_t status) {
/*----------------------------------------------------------------------------*/
  static const char *acBlock = " блока ";
  // показывает нужно ли в каждом последующемем операторе if ставить запятую
  bool prev_exist = false;
  
  // побитовая проверка, блок А6
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
  
  // побитовая проверка, блок А7
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
  // побитовая проверка, блок А8
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
	* @brief Доп.функционал для 1113_1307_1407_1506_1606. */
s32_t
func_help_1103(u8_t *pcDest, u32_t status, const u8_t *pcSrc) {
/*----------------------------------------------------------------------------*/
  static const u8_t *acOpt1 = "Обнаружены нарушения в работе системы "        \
      "управления A9 ВСП (отсутствует обмен с ПЛИС). ";
  static const u8_t *acOpt2 = "Обнаружены нарушения в работе системы "        \
      "управления A9 ВСП (частая потеря связи с ПЛИС). ";
  
  if ((!pcSrc)|(!pcDest)) return -1;
  
  if (status == 0)
    strcpy((char *)(pcDest+0), (const char *)acOpt1);
  else
    strcpy((char *)(pcDest+0), (const char *)acOpt2);
  strcpy((char *)(pcDest + strlen((const char *)pcDest)), (const char *)pcSrc);

  return 0;
}

/**	----------------------------------------------------------------------------
	* @brief Доп.функционал для 1113_1307_1407_1506_1606. */
s32_t
func_help_1106(u8_t *pcDest, u32_t status, const u8_t *pcSrc) {
/*----------------------------------------------------------------------------*/
  static const u8_t *acKm1 = "Обнаружена неисправность контактора "           \
      " KM1 (зарядного) БРО. ";
  static const u8_t *acKm2 = "Обнаружена неисправность контактора "           \
      " KM2 (главного) БРО. ";
  
  if ((!pcSrc)|(!pcDest)) return -1;
  
  if (status == 0)
    strcpy((char *)(pcDest+0), (const char *)acKm1);
  else
    strcpy((char *)(pcDest+0), (const char *)acKm2);
  strcpy((char *)(pcDest + strlen((const char *)pcDest)), (const char *)pcSrc);

  return 0;
}

/**	----------------------------------------------------------------------------
	* @brief Доп.функционал для 1113_1307_1407_1506_1606. */
s32_t
func_help_1112(u8_t *pcDest, u32_t status, const u8_t *pcSrc) {
/*----------------------------------------------------------------------------*/
  static const u8_t *acLow = "Сработала защита по превышению входного тока"   \
    " ВСП (усреднённое значение). ";
  static const u8_t *acHigh = "Сработала защита по превышению входного тока"  \
    " ВСП (мгновенное значение). ";
  
  if ((!pcSrc)|(!pcDest)) return -1;
  
  if (status == 0)
    strcpy((char *)(pcDest+0), (const char *)acLow);
  else
    strcpy((char *)(pcDest+0), (const char *)acHigh);
  strcpy((char *)(pcDest + strlen((const char *)pcDest)), (const char *)pcSrc);

  return 0;
}

/**	----------------------------------------------------------------------------
	* @brief Доп.функционал для 1113_1307_1407_1506_1606. */
s32_t
func_help_1113_1307_1407_1506_1606(u8_t *pcDest, u32_t status, const u8_t *pcSrc) {
/*----------------------------------------------------------------------------*/
  static const u8_t *acLow = "Сработала защита по низкому входному напряжению ";
  static const u8_t *acHigh = "Сработала защита по превышению входного напряжения ";
  
  if ((!pcSrc)|(!pcDest)) return -1;
  
  if (status == 0)
    strcpy((char *)(pcDest+0), (const char *)acLow);
  else
    strcpy((char *)(pcDest+0), (const char *)acHigh);
  strcpy((char *)(pcDest + strlen((const char *)pcDest)), (const char *)pcSrc);

  return 0;
}

/**	----------------------------------------------------------------------------
	* @brief Доп.функционал для 1113_1307_1407_1506_1606. */
s32_t
func_help_1114(u8_t *pcDest, u32_t status, const u8_t *pcSrc) {
/*----------------------------------------------------------------------------*/
  static const u8_t *acLow = "Сработала защита по превышению выходного тока"  \
    " ВСП (усреднённое значение). ";
  static const u8_t *acHigh = "Сработала защита по превышению выходного тока" \
    " ВСП (мгновенное значение). ";
   
  if ((!pcSrc)|(!pcDest)) return -1;
  
  if (status == 0)
    strcpy((char *)(pcDest+0), (const char *)acLow);
  else
    strcpy((char *)(pcDest+0), (const char *)acHigh);
  strcpy((char *)(pcDest + strlen((const char *)pcDest)), (const char *)pcSrc);

  return 0;
}

/**	----------------------------------------------------------------------------
	* @brief Доп.функционал для 1202. */
s32_t
func_help_1202(u8_t *pcDest, u32_t status, const u8_t *pcSrc) {
/*----------------------------------------------------------------------------*/
  static const u8_t *acStart = "Несоответствие между командами СКДУ и "       \
    "сигналами управления оборудованием";
  static const u8_t *acOne = " Канала 1 (компрессор)";
  static const u8_t *acTwo = " Канала 2 (вентилятор)";
  static const u8_t *acThree = " Канала 3 (потребители)";
  static const u8_t *acFour = " Канала 4 (зарядный)";
  static const u8_t *acEnd = " НСП с пульта вагона";
  bool comma = false;
   
  if ((!pcSrc)|(!pcDest)) return -1;

  strcpy((char *)pcDest, (const char *)acStart);
  // побитовая проверка
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
	* @brief Доп.функционал для 1301. Формирует строку в зависимости от номера 
    аварийного драйвера Канала 1 */
s32_t
func_help_1301(u8_t *pcDest, u32_t status, const u8_t *pcSrc) {
/*----------------------------------------------------------------------------*/
  static const char *acStartText = "Сработала защита драйвера(-ов) IGBT ";
  static const char *acFinalText = " Каналa 1 (двигатель компрессора) НСП. "  \
    "Необходимо проверить состояние модулей IGBT и заменить их в случае "     \
    "неисправности; проверить плату драйвера IGBT и заменить её в случае "    \
    "нарушения функционирования; проверить внутренние и внешние подключения " \
    "на отсутствие короткого замыкания";
  
  if ((!pcDest)|!(status & 0xff)) return -1;
  // Начальная часть текста
  strcpy((char *)pcDest, acStartText);
  // сверка бит, идентификация аварийной платы
  // Нумерация выполнена согласно схеме
  func_bits_1301_1401_1501_1601(pcDest, status, "A20", "A21", "A22", NULL);
  // Конечная часть текста
  strcpy((char *)(pcDest + strlen((const char *)pcDest)), acFinalText);

  return 0;
}

/**	----------------------------------------------------------------------------
	* @brief Доп.функционал для 1401. Формирует строку в зависимости от номера 
    аварийного драйвера Канала 2 */
s32_t
func_help_1401(u8_t *pcDest, u32_t status, const u8_t *pcSrc) {
/*----------------------------------------------------------------------------*/
  static const char *acStartText = "Сработала защита драйвера(-ов) IGBT ";
  static const char *acFinalText = " Каналa 2 (двигатель вентилятора) НСП. "  \
    "Необходимо проверить состояние модулей IGBT и заменить их в случае "     \
    "неисправности; проверить плату драйвера IGBT и заменить её в случае "    \
    "нарушения функционирования; проверить внутренние и внешние подключения " \
    "на отсутствие короткого замыкания";
  
  if ((!pcDest)|!(status & 0xff)) return -1;
  // Начальная часть текста
  strcpy((char *)pcDest, acStartText);
  // сверка бит, идентификация аварийной платы
  // Нумерация выполнена согласно схеме
  func_bits_1301_1401_1501_1601(pcDest, status, "A23", "A24", "A25", NULL);
  // Конечная часть текста
  strcpy((char *)(pcDest + strlen((const char *)pcDest)), acFinalText);

  return 0;
}

/**	----------------------------------------------------------------------------
	* @brief Доп.функционал для 1501. Формирует строку в зависимости от номера 
    аварийного драйвера Канала 2 */
s32_t
func_help_1501(u8_t *pcDest, u32_t status, const u8_t *pcSrc) {
/*----------------------------------------------------------------------------*/
  static const char *acStartText = "Сработала защита драйвера(-ов) IGBT ";
  static const char *acFinalText = " Каналa 3 (потребители вагона) НСП. "     \
    "Необходимо проверить состояние модулей IGBT и заменить их в случае "     \
    "неисправности; проверить плату драйвера IGBT и заменить её в случае "    \
    "нарушения функционирования; проверить внутренние и внешние подключения " \
    "на отсутствие короткого замыкания";
  
  if ((!pcDest)|!(status & 0xff)) return -1;
  // Начальная часть текста
  strcpy((char *)pcDest, acStartText);
  // сверка бит, идентификация аварийной платы
  // Нумерация выполнена согласно схеме
  func_bits_1301_1401_1501_1601(pcDest, status, "A28", "A29", "A26", "A27");
  // Конечная часть текста
  strcpy((char *)(pcDest + strlen((const char *)pcDest)), acFinalText);

  return 0;
}

/**	----------------------------------------------------------------------------
	* @brief Доп.функционал для 1501. Формирует строку в зависимости от номера 
    аварийного драйвера Канала 2 */
s32_t
func_help_1601(u8_t *pcDest, u32_t status, const u8_t *pcSrc) {
/*----------------------------------------------------------------------------*/
  static const char *acStartText = "Сработала защита драйвера(-ов) IGBT ";
  static const char *acFinalText = " Каналa 4 (заряд АБ) НСП. "               \
    "Необходимо проверить состояние модулей IGBT и заменить их в случае "     \
    "неисправности; проверить плату драйвера IGBT и заменить её в случае "    \
    "нарушения функционирования; проверить внутренние и внешние подключения " \
    "на отсутствие короткого замыкания";
  
  if ((!pcDest)|!(status & 0xff)) return -1;
  // Начальная часть текста
  strcpy((char *)pcDest, acStartText);
  // сверка бит, идентификация аварийной платы
	// Нумерация выполнена согласно схеме
  func_bits_1301_1401_1501_1601(pcDest, status, "A19", "A17", "A18", NULL);
  // Конечная часть текста
  strcpy((char *)(pcDest + strlen((const char *)pcDest)), acFinalText);

  return 0;
}

/**	----------------------------------------------------------------------------
	* @brief Идентификация аварийной платы
    Бит аварии:   |... b7  b6  b5  b4  b3  b2  b1  b0 |
    Тип аварии:   |    POW POW POW POW FLT FLT FLT FLT|
    Блок силовой: |    #4  #3  #2  #1  #4  #3  #2  #1 |
    Например, если активен бит b0, то случилась авария по FLT в плате №1 
      (плата A20 Канала 1 / плата A23 Канала 2 и т.д.).
    Если активен бит b4, то случилась авария по питанию POW в плате №1 и т.д.
      */
/**	----------------------------------------------------------------------------
	* @brief Определение аварийной платы по биту */
static void
  func_bits_1301_1401_1501_1601(u8_t *pcDest, u32_t status, const u8_t *pcName1,
  const u8_t *pcName2, const u8_t *pcName3, const u8_t *pcName4) {
/*----------------------------------------------------------------------------*/
  // показывает нужно ли в каждом последующемем операторе if ставить запятую
  bool prev_exist = false;
  
  // побитовая проверка, сигналы FLT или POW платы ПДВ2К №1 (pcName1) Канала X
  if ((status & (1<<0)) | (status & (1<<4))) {
    strcpy((char *)(pcDest + strlen((const char *)pcDest)), (char *)pcName1);
    prev_exist |= true;
  }
  // побитовая проверка, сигналы FLT или POW платы ПДВ2К №2 (pcName2) Канала X
  if ((status & (1<<1)) | (status & (1<<5))) {
    if (prev_exist) {
      strcpy((char *)(pcDest + strlen((const char *)pcDest)), ", ");
    }
    strcpy((char *)(pcDest + strlen((const char *)pcDest)), (char *)pcName2);
    prev_exist |= true;
  }
  // побитовая проверка, сигналы FLT или POW платы ПДВ2К №3 (pcName3) Канала X
  if ((status & (1<<2)) | (status & (1<<6))) {
    if (prev_exist) {
      strcpy((char *)(pcDest + strlen((const char *)pcDest)), ", ");
    }
    strcpy((char *)(pcDest + strlen((const char *)pcDest)), (char *)pcName3);
    prev_exist |= true;
  }
  if (!pcName4) return;
  // побитовая проверка, сигналы FLT или POW платы ПДВ2К №4 (pcName4) Канала X
  if ((status & (1<<3)) | (status & (1<<7))) {
    if (prev_exist) {
      strcpy((char *)(pcDest + strlen((const char *)pcDest)), ", ");
    }
    strcpy((char *)(pcDest + strlen((const char *)pcDest)), (char *)pcName4);
  }
}

/**	----------------------------------------------------------------------------
	* @brief Доп.функционал для 1202. */
s32_t
func_help_1304_1404(u8_t *pcDest, u32_t status, const u8_t *pcSrc) {
/*----------------------------------------------------------------------------*/
  static const u8_t *acStart1 = "Превышение номинального тока выхода в ";
  static const u8_t *acStart2 = " раза в течение ";
  static const u8_t *acEnd = " c. Необходимо сократить нагрузку путем отключения" \
    " отдельных потребителей; проверить исправность цепей нагрузки";
  
  if ((!pcSrc)|(!pcDest)) return -1;

  strcpy((char *)pcDest, (const char *)pcSrc);
  strcpy((char *)pcDest + strlen((const char *)pcDest), (const char *)acStart1);
  // побитовая проверка. Первое число
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
  // побитовая проверка. Второе число
  if (status & (1<<0)) {
    sprintf((char *)pcDest + strlen((const char *)pcDest), "%.1f", 10.0f);
  }
  else if (status & (1<<1)) {
    sprintf((char *)pcDest + strlen((const char *)pcDest), "%.1f", 2.0f);
  }
  else if (status & (1<<2)) {
    sprintf((char *)pcDest + strlen((const char *)pcDest), "%.1f", 1.5f);
  }
  else if (status & (1<<3)) {
    sprintf((char *)pcDest + strlen((const char *)pcDest), "%.1f", 1.0f);
  }
  strcpy((char *)pcDest + strlen((const char *)pcDest), (const char *)acEnd);

  return 0;
}

/**	----------------------------------------------------------------------------
	* @brief Доп.функционал для 1504. */
s32_t
func_help_1504(u8_t *pcDest, u32_t status, const u8_t *pcSrc) {
/*----------------------------------------------------------------------------*/
  static const u8_t *acStart1 = "Превышение номинального тока выхода в ";
  static const u8_t *acStart2 = " раза в течение ";
  static const u8_t *acEnd = " c. Необходимо сократить нагрузку путем отключения" \
    " отдельных потребителей; проверить исправность цепей нагрузки";
  
  if ((!pcSrc)|(!pcDest)) return -1;

  strcpy((char *)pcDest, (const char *)pcSrc);
  // Первое число
  strcpy((char *)pcDest + strlen((const char *)pcDest), (const char *)acStart1);
  if (status & (1<<0))
    sprintf((char *)pcDest + strlen((const char *)pcDest), "%.2f", 2.0f);
  else if (status & (1<<1))
    sprintf((char *)pcDest + strlen((const char *)pcDest), "%.2f", 2.5f);
  else if (status & (1<<2))
    sprintf((char *)pcDest + strlen((const char *)pcDest), "%.2f", 3.0f);
  else
    sprintf((char *)pcDest + strlen((const char *)pcDest), "%.2f", 0.0f);

  // Второе число
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
	* @brief Доп.функционал для 1305_1405. */
s32_t
func_help_1305_1405(u8_t *pcDest, u32_t status, const u8_t *pcSrc) {
/*----------------------------------------------------------------------------*/
  static const u8_t *acStart = "Сработала защита от несимметрии выходного тока" \
    "(обрыв) фазы ";
  bool comma = false;
   
  if ((!pcSrc)|(!pcDest)) return -1;

  strcpy((char *)pcDest, (const char *)acStart);
  // побитовая проверка
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

#elif (defined SRV)

// Функции копирования payload параметров в буфер

/**	----------------------------------------------------------------------------
	* @brief */
err_item_t *
  upvs_err__next(upvs_err_t *self) {
/*----------------------------------------------------------------------------*/  
  if (self->ulIdx >= UPVS_ERR_LIST_LENGHT) self->ulIdx = 0;
  else self->ulIdx += 1;
  return &(self->axItems[self->ulIdx]);
}

/**	----------------------------------------------------------------------------
	* @brief */
u32_t
  upvs_err__get_idx(upvs_err_t *self) {
/*----------------------------------------------------------------------------*/  
  return self->ulIdx;
}

/**	----------------------------------------------------------------------------
	* @brief ???
	* @retval error: Статус выполнения функции. */
s32_t
  upvs_err__get_item_idx(upvs_err_t *self, s32_t code) {
/*----------------------------------------------------------------------------*/
  if (!self) return -1;
  
  for (s32_t i = 0; i < UPVS_ERR_LIST_LENGHT; i++) {
		if ( (self->axItems[i].ulCode == code) ) {
      return i;
    }
	}
	return -1;
}

/**	----------------------------------------------------------------------------
	* @brief ???
	* @retval error: Статус выполнения функции. */
bool
  upvs_err__is_help_equal(upvs_err_t *self, s32_t idx, const u8_t *pBuf) {
/*----------------------------------------------------------------------------*/
	if ( !strcmp((const char *)pBuf, (const char *)self->axItems[idx].acHelp) ) {
		return true;
	}
	return false;
}

#endif // SRV


// Функции, ограниченные областью видимости данного файла

#if (defined CLT)

/**	----------------------------------------------------------------------------
	* @brief ??? */
static s32_t
  insert_item(upvs_err_t *self, u32_t code, s32_t value) {
/*----------------------------------------------------------------------------*/
  s32_t idx = -1, i = 0;
  if (!self) return -1;
  
  // Ищем совпадение по 'code' и 'value'.
  // если запись с такими значениями отсутсвует, то получем idx=-1
  // если запись присутствуют, то получаем idx отличный от '-1' и выходим
  for (i = 0; i < UPVS_ERR_LIST_LENGHT; i++) {
		if ( (self->axItems[i].ulCode == code) & (self->axItems[i].slValue == value) )
    {
      idx = i;
    }
	}
  if (idx != -1) return 0;
  
  // Ищем свободную позицию в БД
  if ((idx = upvs_err__get_free_pos(self)) == -1) {
    DBG_PRINT( NET_DEBUG, ("Can't get free pos for %04d, in '%s' /UPVS2/upvs_err.c:%d\r\n", 
      code, __FUNCTION__, __LINE__) );
    return -1;
  }
  
  // Заполняем значения поля по индексу idx:
  // Сохраняем аргументы 'code' и 'value', помечаем аварию как 'Активная' и 
  // 'Новая'. Код в upvs_clt_sess.c пробегаем по всем полям БД и следит за 
  // появлением флага bNew в элементах - если такой выявляется, то брокеру 
  // шлётся Error (1 раз), после чего флаг очищается, но поля ulCode и slValue 
  // не трогаются и запись в целом остается "живой" до момента сброса аварии
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

  // если ячейки в журнале нет, то выходим с ошибкой
  //if ( (idx = errdb_get_cell_id(px, code, value)) < 0 ) return -1;
  for (i = 0; i < UPVS_ERR_LIST_LENGHT; i++) {
		if ( self->axItems[i].ulCode == code ) {
      idx = i;
    }
	}
  if (idx == -1) return -1;
  
  // помечаем НЕактивный статус. Непосредственно удаление выполянет
  // клиент в очередном цикле do_session(..), а не здесь
  self->axItems[idx].bActive = false;
  self->axItems[idx].bNew = true;
  
  return 0;
}

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

#elif (defined SRV)
  
#endif // CLT or SRV

