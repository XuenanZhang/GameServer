BIN = loginserver

MY_LIBS = log net



ifndef GAME_ROOT
export GAME_ROOT := $(subst /src/server/$(BIN),,$(CURDIR))
endif

include $(GAME_ROOT)/src/build.mk
