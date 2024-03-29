TARGETS = me4 usb clhs
BUILD_DIR = ./build


ALL_TARGETS = $(TARGETS)
INSTALL_TARGETS = $(TARGETS:%=install-%)
CLEAN_TARGETS = $(TARGETS:%=clean-%)
ENUMS_TARGETS = $(TARGETS:%=enums-%)
UNINSTALL_TARGETS = $(TARGETS:%=uninstall-%)
CHECK_TARGETS = $(TARGETS:%=check-%)

MAKE_FLAGS += --no-print-directory


.PHONY: all
all: $(ALL_TARGETS)

.PHONY: $(ALL_TARGETS)
$(ALL_TARGETS):
	@echo
	@echo "===== $@ ======"
	@$(MAKE) $(MAKE_FLAGS) -f Makefile.$@ all


.PHONY: enums
enums: $(ENUMS_TARGETS)

.PHONY: $(ENUMS_TARGETS)
$(ENUMS_TARGETS):
	@echo
	@echo "===== $@ ======"
	@$(MAKE) $(MAKE_FLAGS) -f Makefile.$(@:enums-%=%) enums


.PHONY: install
install: $(INSTALL_TARGETS)

.PHONY: $(INSTALL_TARGETS)
$(INSTALL_TARGETS):
	@echo
	@echo "===== $@ ======"
	@$(MAKE) $(MAKE_FLAGS) -f Makefile.$(@:install-%=%) install


.PHONY: clean
clean:
	@rm -rf $(BUILD_DIR)

.PHONY: $(CLEAN_TARGETS)
$(CLEAN_TARGETS):
	@$(MAKE) $(MAKE_FLAGS) -f Makefile.$(@:clean-%=%) clean


.PHONY: uninstall
uninstall: $(UNINSTALL_TARGETS)

.PHONY: $(UNINSTALL_TARGETS)
$(UNINSTALL_TARGETS):
	@$(MAKE) $(MAKE_FLAGS) -f Makefile.$(@:uninstall-%=%) uninstall


.PHONY: check-cmds
check-cmds: $(CHECK_TARGETS)

.PHONY: $(CHECK_TARGETS)
$(CHECK_TARGETS):
	@echo
	@echo "===== $@ ======"
	@$(MAKE) $(MAKE_FLAGS) -f Makefile.$(@:check-%=%) check-cmds
	@echo
