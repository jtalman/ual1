DIRS = ./UAL ./ZLIB ./PAC ./TEAPOT ./ETEAPOT ./ETEAPOT_MltTurn ./ETEAPOT2 ./E4_interim ./E_m ./ACCSIM ./SPINK ./TIBETAN ./SIMBAD ./AIM #./ICE ./AIM 

#ifdef BETACOOL
#DIRS += ./BETACOOL
#endif

compile:  
	@for dir in $(DIRS); do \
		(cd $$dir; if [ -f ./Makefile ]; then $(MAKE) compile; fi;); \
	done

clean: 
	@for dir in $(DIRS); do \
		(cd $$dir; if [ -f ./Makefile ]; then $(MAKE) clean; fi;); \
	done
