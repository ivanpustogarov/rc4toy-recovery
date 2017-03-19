# you can change it by running 'make WORD_SIZE=4'
WORD_SIZE=4
#FLAGS=-Wall -DALPHA="($(WORD_SIZE))"
FLAGS=-Wall
# set this to -DDEBUG to enable debug printing
#VERBOSE=-DDEBUG
VERBOSE=
all:	
	@echo "Compiling for word size $(WORD_SIZE)"
	@echo "Change word size by invoking 'make WORD_SIZE=4'"
ifeq ($(VERBOSE), -DDEBUG)
	@echo "Debug printing is ENABLED"
else
	@echo "Debug printing is DISABLED"
	@echo "Enable debug printing by invoking 'make VERBOSE=-DDEBUG'"
endif
	gcc -c $(FLAGS) util.c -o util.o
	gcc -c $(FLAGS) -DALPHA="($(WORD_SIZE))" rc4prga.c -o rc4prga.o
	gcc -c $(FLAGS) -DALPHA="($(WORD_SIZE))" rc4test.c -o rc4test.o
	gcc -c $(FLAGS) -DALPHA="($(WORD_SIZE))" $(VERBOSE) state-recovery.c -o state-recovery.o
	gcc rc4test.o rc4prga.o util.o -o rc4test
	gcc state-recovery.o rc4prga.o util.o -o state-recovery

clean:
	rm -f rc4prga util.o rc4test.o rc4test state-recovery state-recovery.o rc4prga.o
