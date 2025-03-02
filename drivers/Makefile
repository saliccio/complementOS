include ../common.mk

DRIVERS=$(patsubst $(BASE_PATH)/%/src,%,$(wildcard $(BASE_PATH)/*/src))

all: $(DRIVERS)
	$(foreach module,$(DRIVERS),$(MAKE) -C $(module) CC=$(CC) NASM=$(NASM) ARCH=$(ARCH) BIN=$(BIN) CINCLUDE="$(CINCLUDE)" CFLAGS="$(CFLAGS)";)