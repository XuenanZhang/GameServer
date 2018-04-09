ifndef GAME_ROOT
export GAME_ROOT := $(subst /src/lib,,$(CURDIR))
endif

all clean release:
	@echo "lib"
	@$(MAKE) -C common $@
	# @$(MAKE) -C thread $@
	# @$(MAKE) -C network $@


