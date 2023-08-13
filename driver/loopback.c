#include <stdio.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "platform.h"

#include "util.h"
#include "net.h"

#define LOOPBACK_MTU UINT16_MAX /* maximum size of IP datagram */
#define LOOPBACK_QUEUE_LIMIT 16
#define LOOPBACK_IRQ (INR_IRQ_BASE+1)

#define PRIV(x) ((struct loopback *)x->priv)

struct loopback {
	int irq;
	mutex_t mutex;
	struct queue_haead queue;
};

struct loopbak_queue_entry {
	uint16_t type;
	size_t len;
	uint8_t data[];
}

static int
loopback_isr(unsigned int irq, void *id)
{
}

struct net_device *
loopback_init(void)
{
	struct net_device *dev;
	struct loopback *lo;

	dev = net_device_alloc();
	if (!dev) {
		errorf("net_device_alloc() failure");
		return NULL;
	}
	dev->type = NET_DEVICE_TYPE_LOOPBACK;
	dev->mtu = LOOPBACK_MTU;
	dev->hlen = 0; /* no header */
	dev->alen = 0; /* no address */
	dev->flags = NET_DEVICE_FLAG_LOOPBACK;

	lo = memory_alloc(sizeof(*lo));
	if (!lo) {
		errorf("memory_alloc() failure");
		return NULL;
	}
	lo->irq = LOOPBACK_IRQ;
	mutex_init(&lo->mutex);
	queue_init(&lo->queue);
	dev->priv = lo;

	dev->ops = loopback_isr; 
	if (net_device_register(dev) == -1) {
		errorf("net_device_register() failure");
		return NULL;
	}

	debugf("initialized, dev=%s", dev->name);
	return dev;
}
