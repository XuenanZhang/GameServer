ifndef GAME_ROOT
export GAME_ROOT := $(subst /src,,$(CURDIR))
endif


all clean:
	@$(MAKE) -C lib $@
	@$(MAKE) -C server $@

debug:
	@$(MAKE) GAME_CONFIG=Debug all
	@$(MAKE) GAME_CONFIG=Debug all

release:
	@$(MAKE) GAME_CONFIG=Release all
	@$(MAKE) GAME_CONFIG=Release all


