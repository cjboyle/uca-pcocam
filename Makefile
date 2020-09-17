SISODIR5 = /opt/SiliconSoftware/Runtime5.4.4.1
PCOSDKDIR = /opt/PCO

LIB_NAME = ucapcoclhs


CC = gcc
CCFLAGS = -std=c99 -Wall -fPIC
LDFLAGS = -shared


INC_DIRS =
LIB_DIRS =
LIB_NAMES =

SRCS = uca-pcoclhs-camera.c
HDRS = uca-pcoclhs-camera.h


# UFO-KIT UCA
INC_DIRS += /usr/include/uca
LIB_NAMES += uca


# SISO Runtime SDK
INC_DIRS += $(SISODIR5)/include

LIB_DIRS += $(SISODIR5)/lib
LIB_DIRS += $(SISODIR5)/lib64

LIB_NAMES += clsersis fglib5 haprt


# PCO.Linux SDK
INC_DIRS += $(PCOSDKDIR)/pco_common/pco_include
INC_DIRS += $(PCOSDKDIR)/pco_common/pco_classes
INC_DIRS += $(PCOSDKDIR)/pco_clhs/pco_clhs_common
INC_DIRS += $(PCOSDKDIR)/pco_clhs/pco_classes

LIB_DIRS += $(PCOSDKDIR)/pco_clhs/bindyn
LIB_DIRS += $(PCOSDKDIR)/pco_common/pco_lib

LIB_NAMES += pcoclhs pcocam_clhs pcofile pcolog reorderfunc


# GLib headers
INC_DIRS += /usr/include/glib /usr/include/glib-2.0


# Other
OUTFILE = lib$(LIB_NAME).so
BUILD_DIR = ./build
INST_DIR = /usr/lib64/uca

OBJS := $(SRCS:%=$(BUILD_DIR)/%.o)
DEPS := $(OBJS:.o=.d)

INC_FLAGS = $(addprefix -I,$(INC_DIRS))
LIB_FLAGS = $(addprefix -L,$(LIB_DIRS)) $(addprefix -l,$(LIB_NAMES))


### Build Targets

.PHONY: all
all: $(BUILD_DIR)/$(OUTFILE)

$(BUILD_DIR)/$(OUTFILE): $(OBJS)
	mkdir -p $(BUILD_DIR)
	$(CC) $(LDFLAGS) $(OBJS) -o $@ $(LIB_FLAGS)

$(BUILD_DIR)/%.o: %.c $(HDRS)
	mkdir -p $(BUILD_DIR)
	$(CC) $(CCFLAGS) $(INC_FLAGS) -c $< -o $@


.PHONY: install
install: $(INST_DIR)/$(OUTFILE)

$(INST_DIR)/$(OUTFILE): $(BUILD_DIR)/$(OUTFILE)
	install -m 755 $(BUILD_DIR)/$(OUTFILE) $(INST_DIR)


.PHONY: clean
clean:
	rm -rf $(BUILD_DIR)


.PHONY: uninstall
uninstall:
	rm -f $(INST_DIR)/$(OUTFILE)


-include $(DEPS)