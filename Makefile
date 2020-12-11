EXEC = euler
APP_OBJS = 	json/cJSON.o \
		graph.o \
		node.o \
		main.o

LIBS = -lpthread
all: $(EXEC)
CC=gcc -g 
CXX=g++ -g

euler: $(APP_OBJS)
	g++ -g $(LDFLAGS) -o $@ $(APP_OBJS) $(LDLIBS$(LDLIBS_$@)) $(LIBS) $(CFLAGS)

clean:
	rm -f *.o json/*.o  $(EXEC) *.gdb *.elf *.a
