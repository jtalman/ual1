include $(UAL)/gui/QT/src/Makefile.config

MY_DIR  := $(UAL)/gui/QT/
DIRS     = ./QT

compile : 
	(if [ !  -d $(MY_DIR)/include/UAL ]; \
		then mkdir  $(MY_DIR)/include/UAL; fi;)
	@for dir in $(DIRS); do \
		(cd $$dir; if [ -f ./Makefile ]; then $(MAKE) compile; fi;); \
	done

clean :
	@for dir in $(DIRS); do \
		(cd $$dir; if [ -f ./Makefile ]; then $(MAKE) clean; fi;); \
	done 
	rm -rf $(MY_DIR)/include/UAL
