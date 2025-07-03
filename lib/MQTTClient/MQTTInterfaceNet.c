//#include "mqtt_if.h"
#include "MQTTInterface.h"
#include "userint.h"
#include "lwip/sockets.h"

// int (*mqttread) (Network*, unsigned char*, int, int);
// int (*mqttwrite) (Network*, unsigned char*, int, int);
// void (*disconnect) (Network*);

static int MQTTInterface__read(Network *, u8_t *, int, int);
static int MQTTInterface__write(Network *, u8_t *, int, int);
static void MQTTInterface__disconnect(Network *pnet);


Network xNetwork = {
  .socket = -1,
  .mqttread = MQTTInterface__read,
  .mqttwrite = MQTTInterface__write,
  .disconnect = MQTTInterface__disconnect 
};

/**	----------------------------------------------------------------------------
	* @brief ??? */
static int
  MQTTInterface__read(Network *pnet, u8_t *buffer, int len, int timeout_ms) {
/*----------------------------------------------------------------------------*/
  int available;

	/* !!! LWIP_SO_RCVBUF must be enabled !!! */
  //check receive buffer
	if(lwip_ioctl(pnet->socket, FIONREAD, &available) < 0) return -1;

	if(available > 0)	{
		return recv(pnet->socket, buffer, len, 0);
	}

	return 0;
}

/**	----------------------------------------------------------------------------
	* @brief ??? */
static int
  MQTTInterface__write(Network *pnet, u8_t *buffer, int len, int timeout_ms) {
/*----------------------------------------------------------------------------*/
  return send(pnet->socket, buffer, len, 0);
}

/**	----------------------------------------------------------------------------
	* @brief ??? */
static void
  MQTTInterface__disconnect(Network *pnet) {
/*----------------------------------------------------------------------------*/
  lwip_close(pnet->socket);
	pnet->socket = 0;
}
