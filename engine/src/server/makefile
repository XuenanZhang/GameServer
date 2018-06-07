ifndef GAME_ROOT
export GAME_ROOT := $(subst /src/server,,$(CURDIR))
endif

all clean:
	@$(MAKE) -C loginserver  $@
	@$(MAKE) -C testServer  $@
	@$(MAKE) -C testClient  $@


