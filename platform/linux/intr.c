#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <pthread.h>

#include "platform.h"

#include "util.h"

struct irq_entry {
	struct irq_entry *next;
	unsigned int irq;
	int (*handler)(unsigned int irq, void *dev);
	int flags;
	char name[16];
	void *dev;
};

static struct irq_entry *irqs;

static sigset_t sigmask;

static pthread_t tid;
static pthread_barrier_t barrier;

int
intr_request_irq(unsigned int irq, int (*handler)(unsigned int irq, void *dev), int flags, const char *name, void *dev)
{
	struct irq_entry *entry;

	/* check if sharing irq number is allowed */
	debugf("irq=%u, flags=%d, name=%s", irq, flags, name);
	for (entry = irqs; entry; entry = entry->next) {
		if (entry->irq == irq) {
			if (entry->flags ^ INTR_IRQ_SHARED || flags ^ INTR_IRQ_SHARED) {
				errorf("conflicts with alredy registered IRQs");
				return -1;
			}
		}
	}

	entry = memory_alloc(sizeof(*entry));
	if (!entry) {
		errorf("memory_alloc() failure");
		return -1;
	}
	entry->irq = irq;
	entry->handler = handler;
	entry->flags = flags;
	strncopy(entry->name, name, sizeof(entry->name)-1);
	entry->dev = dev;

	/* append irq to head of the irq list */
	entry->next = irqs;
	irqs = entry;

	sigaddtest(&sigmask, irq);
	debugf("registered: irq=%u, name=%s", irq, name);

	return 0;
}
