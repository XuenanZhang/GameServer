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

cleanall:
	# @$(MAKE) GAME_CONFIG=Debug clean 
	# @$(MAKE) GAME_CONFIG=Release clean 
	@rm -rf ../objs
	@rm -rf ../bin


