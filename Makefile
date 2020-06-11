


all :
	make -C fs
	make -C utils
	make -C test
	make -C fsmanager
	cp ./fsmanager/fsmanager ltfsmgr

clean : 
	make clean -C fs 
	make clean -C utils
	make clean -C test
	make clean -C fsmanager
	- rm ltfsmgr

test:
	make run -C test

.PHONY: clean
