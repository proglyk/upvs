#ifndef UPVS_MQTT_CLT_H
#define UPVS_MQTT_CLT_H

#include "userint.h"
#include "MQTTClient.h"
#include "upvs_conf.h"
#include "upvs_clt.h"

// Упр. структура
typedef struct mqtt_clt_st mqtt_clt_t;

// Объявления функций

// Функции создания, инициализации и удаления экземпляра упр. структуры
void *upvs_mqtt_clt__create(void);
int   upvs_mqtt_clt__init(mqtt_clt_t *, s32_t);
void  upvs_mqtt_clt__del(mqtt_clt_t *);
// Передача по сети
s32_t	upvs_mqtt_clt__send(mqtt_clt_t *, const u8_t *, const u8_t *);
// Вспомогательные
bool  upvs_mqtt_clt__is_conntd(mqtt_clt_t *);
#define clt_inst(P) upvs_mqtt_clt__clt_inst(P)
upvs_clt_t *upvs_mqtt_clt__clt_inst(mqtt_clt_t *self);

#endif //UPVS_MQTT_CLT_H