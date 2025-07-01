#ifndef _UPVS_CONF_H_
#define _UPVS_CONF_H_

#define UPVS_PARAM_LIST_LEN             72U

#define UPVS_PARAM_VALUE_SIZE           (64UL)
#define UPVS_TOPICPATH_SIZE             (64UL)

// Размер буферов. для upvs_srv2.h
#define DECODER_BUFSIZE_RX              (2048UL)
#define DECODER_BUFSIZE_TX              ( 256UL)

#define ERR_CAPACITY                    (10UL)

// Размеры промежуточного буфера аварий. для upvs_err.h
// размеры массивов соответствуют таковым в таблицах 2..7 Протокола ПК-ПИ396
#define DESC_LENGHT  ( 128UL) // или  64 симв. UTF-8
#define INFL_LENGHT  ( 256UL) // или 128 симв. UTF-8
#define HELP_LENGHT  (1024UL) // или 512 (4x128) симв. UTF-8

#endif //_UPVS_CONF_H_