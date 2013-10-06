ifndef TARGET
TARGET=satxo-sensors
endif


CONTIKI_PROJECT = echo-server
all: $(CONTIKI_PROJECT)

CFLAGS += -DWITH_UIP=1 -UIP_CONF_IPV6=0


CONTIKI = ../..
include $(CONTIKI)/Makefile.include
