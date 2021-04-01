SISODIR5 ?= /opt/SiliconSoftware/Runtime5.7.0
PCOSDKDIR = /opt/PCO

LIB_NAME = ucapcome4
PROJ_NAME = uca-pcome4



CC = gcc
CCFLAGS = -isystem -std=c99 -O2 -Wall -Wno-write-strings -Werror=implicit-fallthrough -fPIC -DSISODIR5=$(SISODIR5)
CXX = g++
CXXFLAGS = -isystem -std=c++0x -O2 -Wall -Wno-write-strings -Werror=implicit-fallthrough -fPIC
LDFLAGS = -shared


SRCS = $(PROJ_NAME)-camera.c $(PROJ_NAME)-enums.c pcome4.cpp
HDRS = $(PROJ_NAME)-camera.h $(PROJ_NAME)-enums.h pcome4.h pco.h


# UFO-KIT UCA
INC_DIRS += /usr/include/uca
LIB_NAMES += uca


# SISO Runtime SDK
INC_DIRS += $(SISODIR5)/include
LIB_DIRS += $(SISODIR5)/lib
LIB_DIRS += $(SISODIR5)/lib64
LIB_NAMES += fglib5 clsersis


# PCO.Linux SDK
PCO_LIB_DIR = ./pco/lib/me4
PCO_LIB_NAMES += pcolog pcofile reorderfunc pcocam_me4
#PCO_LIBS = -Wl,-Bstatic $(addprefix $(PCO_LIB_DIR)/lib,$(addsuffix .a,$(PCO_LIB_NAMES))) -Wl,-Bdynamic

ifndef PCO_LIBS
LIB_DIRS += /usr/local/lib
LIB_DIRS += $(PCOSDKDIR)/lib
LIB_DIRS += $(PCOSDKDIR)/pco_camera/pco_me4/bindyn
LIB_DIRS += $(PCOSDKDIR)/pco_me4_camera/pco_me4/bindyn
LIB_NAMES += $(PCO_LIB_NAMES)
endif


# GLib headers
INC_DIRS += /usr/include/glib /usr/include/glib-2.0 /usr/lib64/glib-2.0/include
LIB_NAMES += glib-2.0


# Other libs
LIB_NAMES += pthread rt dl


# Other
OUTFILE = lib$(LIB_NAME).so
BUILD_DIR = ./build
INST_DIR = /usr/lib64/uca

OBJS := $(SRCS:%=$(BUILD_DIR)/%.o)
DEPS := $(OBJS:.o=.d)

INC_FLAGS = $(addprefix -I,$(INC_DIRS))
LIB_FLAGS = $(addprefix -L,$(LIB_DIRS)) $(addprefix -l,$(LIB_NAMES)) $(PCO_LIBS)


### Build Targets

.PHONY: all
all: $(HDRS) $(SRCS) $(BUILD_DIR)/$(OUTFILE)

.PHONY: enums
enums:  $(HDRS) $(SRCS) .FORCE

%-enums.c: %-enums.c.in %-enums.h
	glib-mkenums --template=$< --output=$@ $(PROJ_NAME)-camera.h

%-enums.h: %-enums.h.in %-camera.h
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
	install -m 755 $(BUILD_DIR)/$(OUTFILE) $(INST_DIR)


.PHONY: clean
clean:
	@rm -rf $(BUILD_DIR)/*me4*
	@rm -rf $(BUILD_DIR)/*cl*


.PHONY: uninstall
uninstall:
	@rm -f $(INST_DIR)/$(OUTFILE)


.PHONY: check-cmds
check-cmds:
	@echo "gcc compile: $(CC) $(CCFLAGS) $(INC_FLAGS) -c foo.c -o $(BUILD_DIR)/foo.c.o"
	@echo
	@echo "g++ compile: $(CXX) $(CXXFLAGS) $(INC_FLAGS) -c bar.cpp -o $(BUILD_DIR)/bar.cpp.o"
	@echo
	@echo "linker: $(CXX) $(LDFLAGS) $(BUILD_DIR)/foo.c.o $(BUILD_DIR)/bar.cpp.o -o $(BUILD_DIR)/foobar.so $(LIB_FLAGS)"

.FORCE:

-include $(DEPS)