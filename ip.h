#ifndef IP_H
#define IP_H

#include <stddef.h>
#include <stdint.h>

typedef uint32_t ip_addr_t;

extern int
ip_init(void);

extern int
ip_addr_pton(const char *p, ip_addr_t *n);
extern char *
ip_addr_ntop(ip_addr_t *n, const char *p, size_t size);

#endif
