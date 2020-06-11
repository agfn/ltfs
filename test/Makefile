
TARGETS += entry create orwc delete info

OBJS = utils.o
FS_OBJS = $(addprefix ../fs/, bitmap.o block.o device.o entry.o fd.o fs.o path.o) 

all: $(TARGETS)

run: create orwc
	sudo ./create
	sudo ./orwc
	sudo ./delete

create: create.o $(FS_OBJS)
	gcc -g $^ -o $@

orwc: orwc.o $(FS_OBJS)
	gcc -g $^ -o $@

delete: delete.o $(FS_OBJS)
	gcc -g $^ -o $@

info: info.o $(FS_OBJS) $(OBJS)
	gcc -g $^ -o $@

info.cpp.elf: info.cpp $(FS_OBJS) $(OBJS)
	g++ -g $^ -o $@

entry: entry.o 
	gcc -g $^ -o $@

clean:
	- rm $(TARGETS)
	- rm $(FS_OBJS)
	- rm $(OBJS)
	- rm $(addsuffix .o, $(TARGETS))

.PHONY: clean
