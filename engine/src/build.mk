ifndef GAME_CONFIG
GAME_CONFIG := Debug
endif

VPATH += $(shell find $(CURDIR) -type d)
LIBDIR = $(GAME_ROOT)/src/libs

#执行文件路径
ifdef BIN
MAKE_LIBS = 1
OUTPUT_DIR = $(GAME_ROOT)/bin
OUTPUT_FILE = $(OUTPUT_DIR)/$(BIN)
OUTPUT_OBJDIR = $(GAME_ROOT)/objs/$(GAME_CONFIG)/$(BIN)
endif # BIN

#静态库路径
ifdef LIB
OUTPUT_DIR = $(LIBDIR)
OUTPUT_FILE = $(OUTPUT_DIR)/lib$(LIB).a
OUTPUT_OBJDIR = $(GAME_ROOT)/objs/$(GAME_CONFIG)/$(LIB)
endif # LIB

#共享库
ifdef SO

endif

#编译配置
INCLUDES := $(GAME_ROOT)/src
INCLUDES += $(shell find $(CURDIR) -type d)
INCLUDES += $(shell find $(GAME_ROOT)/src/lib -type d)

#链接配置
LDFLAGS := -L$(LIBDIR)
ifndef NO_BASE_LIBS
MY_LIBS += common 
endif
LDLIBS := $(addprefix -l, $(MY_LIBS))

#########################
# 额外参数
#########################
#范例
ifdef test
CPPFLAGS += -DTEST
INCLUDES += $(GAME_ROOT)/src/lib/test
LDLIBS += -ltest
endif

ifndef MULIT_THREAD
LDLIBS += -lpthread
endif

#########################
# flags 配置
#########################
ifndef CC
CC = gcc
endif

ifndef CXX
CXX = g++
endif

# -Wall 生成所有警告信息
CXXFLAGS += -Wall 
# -Wno-deprecated 使用过时的特性时不要给出警告。
# CXXFLAGS += -Wno-deprecated
# -Werror时所有的警告都变成错误,使出现警告时也停止编译.需要和指定警告的参数一起使用.
# CXXFLAGS += -Werror

CXXFLAGS += -MMD -DGAME_CONFIG=\"${GAME_CONFIG}\"

ifeq ($(GAME_CONFIG), Release)
CXXFLAGS += -O3
CPPFLAGS += -D_RELEASE
endif

ifeq ($(GAME_CONFIG), Debug)
CXXFLAGS += -g
CPPFLAGS += -D_DEBUG
endif

# ifneq (,$(findstring Debug,$(GAME_CONFIG)))
	# CXXFLAGS += -g
	# CPPFLAGS += -D_DEBUG
# endif

LDFLAGS += $(MY_LDFLAGS)
INCLUDES += $(MY_INCLUDE)
INCLUDES := $(addprefix -I, $(INCLUDES))


#########################
# 构建变量
#########################
# COMPILE.c = $(CC) $(CFLAGS) $(CPPFLAGS) $(KBE_INCLUDES) $(TARGET_ARCH) -c
COMPILE.cpp = $(CXX) $(CXXFLAGS) $(CPPFLAGS) $(INCLUDES) -c

LINK.cpp = $(CXX) $(CXXFLAGS) $(LDFLAGS)

#########################
# 处理目标文件
#########################
.PHONY : all done clean info always

all: info $(OUTPUT_DIR) $(OUTPUT_OBJDIR) $(OUTPUT_FILE) done

info:
	@echo VPATH = $(VPATH)
	@echo CURDIR = $(CURDIR)
	@echo OUTPUT_FILE = $(OUTPUT_FILE)
	@echo SRC_CPP = $(SRC_CPP)
	@echo SRC_OBJS = $(SRC_OBJS)
	@echo OUTPUT_DIR = $(OUTPUT_DIR)
	@echo OUTPUT_OBJDIR = $(OUTPUT_OBJDIR)
	@echo OUTPUT_OBJS = $(OUTPUT_OBJS)
	@echo OUTPUT_DEPS = $(OUTPUT_DEPS)
	@echo MY_LIBS = $(MY_LIBS)
	@echo INCLUDES = $(INCLUDES)

done:
	@echo "------------------------------------------------------------------------"
	@echo "done ===> $(OUTPUT_FILE)"
	@echo "------------------------------------------------------------------------"
# ifeq (0, $(MAKELEVEL))
	# @echo -n 
# endif

clean:
	@rm -rf $(OUTPUT_OBJDIR) $(OUTPUT_FILE)

# DIRS := $(shell find . -type d ! -path "*.svn" ! -path "*.git")
SRC_CPP := $(shell find $(CURDIR) -name "*.cpp")
SRC_C := $(shell find $(CURDIR) -name "*.c")
# SRC_ALL := $(OUTPUT_C) $(OUTPUT_CPP) 
SRC_OBJS := $(SRC_CPP:%.cpp=%.o) $(SRC_C:%.c=%.o)

OUTPUT_OBJS := $(addprefix $(OUTPUT_OBJDIR)/, $(notdir $(SRC_OBJS))) 
OUTPUT_DEPS := $(OUTPUT_OBJS:%.o=%.d)

ifdef MAKE_LIBS
MY_LIBNAMES = $(foreach L, $(MY_LIBS), $(LIBDIR)/lib$(L).a)

# $(MY_LIBNAMES) : always
	# $(MAKE) -C $(GAME_ROOT)/src/lib/$(subst XXXXX,_lib,$(subst lib,,$(subst _lib,XXXXX,$(*F)))) GAME_CONFIG=$(GAME_CONFIG)
endif

ifneq ($(SRC_CPP),)
-include $(OUTPUT_DEPS)

endif


$(OUTPUT_DIR): 
	@mkdir -p $(OUTPUT_DIR)

$(OUTPUT_OBJDIR): 
	@mkdir -p $(OUTPUT_OBJDIR)

# /home/coder/GameServer/engine/objs/Debug/loginserver/main.o:/home/coder/GameServer/engine/src/server/loginserver/tt/pp.h
$(OUTPUT_OBJDIR)/%.o: %.cpp
	@echo $+
	$(COMPILE.cpp) $< $(OUTPUT_OPTION)
	
ifdef BIN
$(OUTPUT_FILE):: $(OUTPUT_OBJS) $(MY_LIBNAMES)
	@echo $^
	$(LINK.cpp) -o $@ $(OUTPUT_OBJS) $(LDLIBS) 
endif

ifdef LIB
$(OUTPUT_FILE):: $(OUTPUT_OBJS)
	@$(AR) rsuv $@ $(OUTPUT_OBJS)
endif

ifdef SO
$(OUTPUT_FILE):: $(OUTPUT_OBJS) $(MY_LIBNAMES)
	$(LINK.cc) -shared -o $@ $(OUTPUT_OBJS) $(LDLIBS)
endif





