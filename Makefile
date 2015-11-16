CC = gcc
LIBS = -fopenmp
FLAGS = -g -Wall
EXECS = circuit prime
DEBUG_EXECS = circuit_debug prime_debug

all: $(EXECS)

debug: $(DEBUG_EXECS)
   
circuit: circuit.c
	$(CC) $(FLAGS) -o $@ $? $(LIBS) -lm
	
prime: prime.c
	$(CC) $(FLAGS) -o $@ $? $(LIBS) -lm

circuit_debug: circuit.c
	$(CC) $(FLAGS) -DDEBUG -o $@ $? $(LIBS) -lm

prime_debug: prime.c
	$(CC) $(FLAGS) -DDEBUG -o $@ $? $(LIBS) -lm

clean:
	$(RM) $(EXECS) $(DEBUG_EXECS)
