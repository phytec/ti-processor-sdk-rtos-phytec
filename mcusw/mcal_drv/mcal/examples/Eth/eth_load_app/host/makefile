UTILS_DIR := ../../utils

all: EthHostLoadApp

EthHostLoadApp: HostLoadApp.c $(UTILS_DIR)/EthUtils.c $(UTILS_DIR)/EthUtils_Host.c
	gcc $^ -I$(UTILS_DIR) -o $@

.PHONY: clean
clean:
	rm -f *.o EthHostLoadApp
