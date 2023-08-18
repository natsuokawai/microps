#include <stdint.h>
#include <stddef.h>

#include "util.h"
#include "net.h"
#include "ip.h"

struct ip_hdr {
	uint8_t vhl; /* version (4bit) + header length (4bit) */
	uint8_t tos;
	uint16_t total;
	uint16_t id;
	uint16_t offset; /* flag (3bit) + fragment offset (13bit) */
	uint8_t ttl;
	uint8_t protocol;
	uint8_t sum;
	ip_addr_t src;
	ip_addr_t dst;
	uint8_t options[];
};

const ip_addr_t IP_ADDR_ANY = 0x00000000; /* 0.0.0.0 */
const ip_addr_t IP_ADDR_BROADCAST = 0xffffffff /* 255.255.255.255 */

/* Printable text TO Network binary */
int
ip_addr_pton(const char *p, ip_addr_t *n)
{
	char *sp, *ep;
	int idx;
	long ret;

	sp = (char *)p;
	for (idx = 0; idx < 4; idx++) {
		ret = strtol(sp, &ep, 10);
		if (ret < 0 || ret > 255) {
			return -1;
		}
		if (ep == sp) {
			return -1;
		}
		if ((idx == 3 && *ep != '\0') || (idx != 3 && *ep != '.')) {
			return -1;
		}
		((uint8_t *)n)[idx] = ret;
		sp = ep + 1;
	}
	return 0;
}

/* Network binary TO Printable text */
char *
ip_addr_ntop(ip_addr_t n, char *p, size_t size)
{
	uint8_t *u8;

	u8 = (uint8_t *)&n;
	snprintf(p, size, "%d.%d.%d.%d", u8[0], u8[1], u8[2], u8[3]);
	return p;
}

static void
ip_input(const uint8_t *data, size_t len, struct net_device *dev)
{
	debugf("dev=%s, len=%zu", dev->name, len);
	debugdump(data, len);
}

int
ip_init(void)
{
	if (net_protocol_register(NET_PROTOCOL_TYPE_IP, ip_input) == -1) {
		errorf("net_protocol_register() failure");
		return -1;
	}
	return 0;
}
