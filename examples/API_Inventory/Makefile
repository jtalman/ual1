MFILES = working.map*
TFILES = working.twiss*
DIRS = ./getMapWithOpticsCalculator ./getTwissWithShell

compile:  
	for dir in $(DIRS); do \
		(cd $$dir; if [ -f ./Makefile ]; then make compile; fi;); \
	done

clean: 
	rm -f $(MFILES)
	rm -f $(TFILES)
	rm -f echo.sxf
	for dir in $(DIRS); do \
		(cd $$dir; if [ -f ./Makefile ]; then make clean; fi;); \
	done
