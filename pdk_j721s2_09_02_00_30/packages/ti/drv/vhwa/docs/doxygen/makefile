PDK_INSTALL_PATH ?= $(abspath ../../../../../../packages)

include $(PDK_INSTALL_PATH)/ti/build/Rules.make

export DOXYGEN ?= doxygen

#Add module name here as and when a module design is enabled
DESIGN_MODULE_LIST = vhwa

include cfg/soc/$(SOC)/make.cfg

export
#No edit is required beyond this point
DESIGN_LIST = $(addprefix design_, $(DESIGN_MODULE_LIST))
DESIGN_LIST_CLEAN = $(addsuffix _clean, $(DESIGN_LIST))

.PHONY : all clean help
.PHONY : apiguide apiguide_clean
.PHONY : design design_clean $(DESIGN_LIST) $(DESIGN_LIST_CLEAN)

all: apiguide design

clean : apiguide_clean design_clean

apiguide: apiguide_clean
	$(ECHO) API Guide for $(SOC)...
	$(MAKE) -C apiguide
	$(ECHO) API Guide $(SOC)... Done!!!
	$(ECHO) ""

apiguide_clean:
	$(MAKE) -C apiguide clean

design: $(DESIGN_LIST)

design_clean: $(DESIGN_LIST_CLEAN)

$(DESIGN_LIST):
	$(ECHO) $@...
	$(MAKE) -C design/$(subst design_,,$@)
	$(ECHO) $@ Done!!!
	$(ECHO) ""

$(DESIGN_LIST_CLEAN):
	$(MAKE) -C design/$(subst _clean,,$(subst design_,,$@)) clean

help:
	@echo
	@echo make -s SOC=[SOC] [targets]
	@echo
	@echo "supported SOC: j721e j721s2 j784s4"
	@echo
	@echo "supported targets:"
	@echo "all            : Builds all (apiguide, design) for provided SOC"
	@echo "apiguide       : Builds vhwa apiguide for provided SOC"
	@echo "design         : Builds all module design documents for provided SOC"
	@echo "design_[module]: Builds specific module design doc. See below for supported modules"
	@echo "[$(DESIGN_LIST)]"
	@echo
	@echo "clean          : Cleans all"
	@echo "apiguide_clean : Cleans apiguide"
	@echo "design_clean   : Cleans all module design"
	@echo
	@echo "help           : Prints this help message"
	@echo

# Nothing beyond this point
