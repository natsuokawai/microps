#include <stdio.h>
#include <stddef.h>
#include <stdint.h>

#include "platform.h"

#include "util.h"
#include "net.h"

static struct net_device *devices;

struct net_device *
net_device_alloc(void)
{
	struct net_device *dev;

	dev = memory_alloc(sizeof(*dev));
	if (!dev) {
		errorf("memory_alloc() failure");
		return NULL;
	}
	return dev;
}

/* NOTE: must not be called aftere net_run() */
int
net_device_register(struct net_device *dev)
{
	static unsigned int index = 0;

	dev->index = index++;
	snprintf(dev->name, sizeof(dev->name), "net%d", dev->index);

	/* append device to head of the device list */
	dev->next = devices;
	devices = dev;
	infof("registered, dev=%s, type=0x%04x", dev->name, dev->type);

	return 0;
}

/* specify "static" to allow access to functions only within this file */
static int
net_device_open(struct net_device *dev)
{
	if (NET_DEVICE_FLAG_UP(dev)) {
		errorf("already opened, dev=%s", dev->name);
		return -1;
	}
	if (dev->ops->open) {
		if (dev->ops->open(dev) == -1) {
			errorf("open failure, dev=%s", dev->name);
		}
	}
	dev->flags |= NET_DEVICE_FLAG_UP;
	infof("dev=%s, state=%s", dev->name, NET_DEVICE_STATE(dev));
	return 0;
}

static int
net_device_close(struct net_device *dev)
{
	if (!NET_DEVICE_FLAG_UP(dev)) {
		errorf("not opened, dev=%s", dev->name);
		return -1;
	}
	if (dev->ops->close) {
		if (dev->ops->close(dev) == -1) {
			error("close failure, dev=%s", dev->name);
			return -1;
		}
	}
	def->flags &= -NET_DEVICE_FLAG_UP;
	infof("dev=%s, state=%s", dev->name, NET_DEVICE_STATE(dev));
	return 0;
}

int
net_device_output(struct net_device *dev, uint16_t type, const uint8_t *data, size_t len, const void *dst)
{
	if (!NET_DEVICE_IS_UP(dev)) {
		errorf("not opened, dev=%s", dev->name);
		return -1;
	}
	if (len > dev->mtu) {
		errorf("output too long, dev=%s, mtu=%u, len=%zu", dev->name, dev->mtu, len);
		return -1;
	}
	debugf("dev=%s, type=0x%04x, len=%zu", dev->name, type, len);
	debugdump(data, len);
	if (dev->ops->transmit(dev, type, data, len, dst) == -1) {
		errorf("device transmit failure, dev=%s, len=%zu", dev->name, len);
		return -1
	}
	return 0;
}
