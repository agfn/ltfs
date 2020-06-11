OBJS = main.o cmd.o fsmgr.o utils.o
FS_OBJS = $(addprefix ../fs/, bitmap.o block.o device.o entry.o fd.o fs.o path.o) 


fsmanager: $(OBJS) $(FS_OBJS)
	$(MAKE) -C ../fs clean
	$(MAKE) -C ../fs
	g++ -g $^ -o $@ -lcrypto -lssl

%.o: %.cpp
	g++ -c -g -o $@ $^

clean:
	- rm fsmanager
	- rm $(OBJS)