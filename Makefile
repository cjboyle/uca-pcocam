SISODIR5 = /opt/SiliconSoftware/Runtime5.7.0
# PCOSDKDIR = /opt/PCO/pco_camera

LIB_NAME = ucapcoclhs
PROJ_NAME = uca-pcoclhs



CC = gcc
CCFLAGS = -std=c99 -Wall -fPIC
CXX = g++
CXXFLAGS = -std=c++0x -Wall -fPIC
LDFLAGS = -shared


SRCS = $(PROJ_NAME)-camera.c $(PROJ_NAME)-enums.c pcoclhs.cpp
HDRS = $(PROJ_NAME)-camera.h $(PROJ_NAME)-enums.h pcoclhs.h


# UFO-KIT UCA
INC_DIRS += /usr/include/uca
LIB_NAMES += uca


# SISO Runtime SDK
INC_DIRS += $(SISODIR5)/include

LIB_DIRS += $(SISODIR5)/lib
LIB_DIRS += $(SISODIR5)/lib64

LIB_NAMES += clsersis fglib5 haprt


# PCO.Linux SDK
# INC_DIRS += $(PCOSDKDIR)/pco_common/pco_include
# INC_DIRS += $(PCOSDKDIR)/pco_common/pco_classes
# INC_DIRS += $(PCOSDKDIR)/pco_clhs/pco_clhs_common
# INC_DIRS += $(PCOSDKDIR)/pco_clhs/pco_classes

# LIB_DIRS += $(PCOSDKDIR)/pco_clhs/bindyn
# LIB_DIRS += $(PCOSDKDIR)/pco_common/pco_lib
LIB_DIRS += /usr/local/lib

LIB_NAMES += pcoclhs pcocam_clhs pcocom_clhs pcofile pcolog reorderfunc


# GLib headers
INC_DIRS += /usr/include/glib /usr/include/glib-2.0 /usr/lib64/glib-2.0/include
LIB_NAMES += glib-2.0

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
all: $(HDRS) $(SRCS) $(BUILD_DIR)/$(OUTFILE)

.PHONY: enums
enums:  $(HDRS) $(SRCS)

%-enums.c: %-enums.c.in %-enums.h
	glib-mkenums --template=$< --output=$@ $(HDRS)

%-enums.h: %-enums.h.in
	glib-mkenums --template=$< --output=$@ $(PROJ_NAME)-camera.h


$(BUILD_DIR)/$(OUTFILE): $(OBJS)
	@mkdir -p $(BUILD_DIR)
	$(CXX) $(LDFLAGS) $(OBJS) -o $@ $(LIB_FLAGS)

$(BUILD_DIR)/%.c.o: %.c $(HDRS)
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CCFLAGS) $(INC_FLAGS) -c $< -o $@

$(BUILD_DIR)/%.cpp.o: %.cpp $(HDRS)
	@mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $(INC_FLAGS) -c $< -o $@


.PHONY: install
install: $(INST_DIR)/$(OUTFILE)

$(INST_DIR)/$(OUTFILE): $(BUILD_DIR)/$(OUTFILE)
	install -m 644 $(BUILD_DIR)/$(OUTFILE) $(INST_DIR)


.PHONY: clean
clean:
	@rm -rf $(BUILD_DIR)


.PHONY: uninstall
uninstall:
	@rm -f $(INST_DIR)/$(OUTFILE)


.PHONY: check-vars
check-vars:
	@echo -n "SISODIR5: $(SISODIR5)" && test -d $(SISODIR5) && echo " (found)" || echo " (not found)"
	@echo -n "PCOSDKDIR: $(PCOSDKDIR)" && test -d $(PCOSDKDIR) && echo " (found)" || echo " (not found)"
	@echo
	@echo "SRCS: $(SRCS)"
	@echo "HDRS: $(HDRS)"
	@echo "LIB_NAMES: $(LIB_NAMES)"
	@echo
	@echo "BUILD_DIR: $(BUILD_DIR)"
	@echo "INST_DIR: $(INST_DIR)"

.PHONY: check-cmds
check-cmds:
	@echo "gcc compile: $(CC) $(CCFLAGS) $(INC_FLAGS) -c foo.c -o $(BUILD_DIR)/foo.c.o"
	@echo
	@echo "g++ compile: $(CXX) $(CXXFLAGS) $(INC_FLAGS) -c bar.cpp -o $(BUILD_DIR)/bar.cpp.o"
	@echo
	@echo "linker: $(CXX) $(LDFLAGS) $(BUILD_DIR)/foo.c.o $(BUILD_DIR)/bar.cpp.o -o $(BUILD_DIR)/foobar.so $(LIB_FLAGS)"

-include $(DEPS)