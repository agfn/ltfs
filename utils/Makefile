TARGETS += formatter
OBJS += 

all: $(TARGETS)


formatter: $(OBJS)
	gcc -g formatter.c $^ -o formatter

clean:
	- rm $(TARGETS)
	- rm $(OBJS)