ifndef GAME_ROOT
export GAME_ROOT := $(subst /src/lib,,$(CURDIR))
endif

all clean :
	@$(MAKE) -C common $@
	@$(MAKE) -C thread $@
	@$(MAKE) -C log $@
	@# @$(MAKE) -C network $@


