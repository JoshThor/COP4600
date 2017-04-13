SRCS   = charWrite.c charRead.c
OBJS   = $(SRCS:.c=.o)

obj-m += $(OBJS)

EXTRA_CFLAGS = -O2


all:
	$(MAKE) -C /lib/modules/`uname -r`/build M=$(PWD) modules

clean:
	$(MAKE) -C /lib/modules/`uname -r`/build M=$(PWD) clean
	$(RM) Module.markers modules.order