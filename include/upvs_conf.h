#ifndef _UPVS_CONF_H_
#define _UPVS_CONF_H_

#define UPVS_PRM_LIST_LENGHT            72U

#define UPVS_PARAM_VALUE_SIZE           64U
#define UPVS_TOPICPATH_SIZE             64U
#define UPVS_VALUE_SIZE                 64U
#define UPVS_CLT_ERROR_SIZE             2048U

// Размер буферов. для upvs_srv2.h
#define DECODER_BUFSIZE_RX              (2048UL)
#define DECODER_BUFSIZE_TX              ( 256UL)

#define UPVS_ERR_LIST_LENGHT            (10UL)

// Размеры промежуточного буфера аварий. для upvs_err.h
// размеры массивов соответствуют таковым в таблицах 2..7 Протокола ПК-ПИ396
#define DESC_LENGHT  ( 128UL) // или  64 симв. UTF-8
#define INFL_LENGHT  ( 256UL) // или 128 симв. UTF-8
#define HELP_LENGHT  (1024UL) // или 512 (4x128) симв. UTF-8

#define MQTT_BUFSIZE                    2048

#define UPVS_SOCK_RCVTIME               10000U

#define UPVS_CLT_CNT_INIT               7U
#define UPVS_CLT_CNT_MAX                69U

#define UPVS_SRV_CNT_INIT               0U
#define UPVS_SRV_CNT_MAX                5U

#endif //_UPVS_CONF_H_