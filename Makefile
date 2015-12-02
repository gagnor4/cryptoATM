
CXX = g++
DEBUG = -g
INCLUDE = 
CFLAGS = -Wall $(DEBUG) $(INCLUDE) -c
LFLAGS = -Wall $(DEBUG) $(INCLUDE)
LIBFLAGS = -lcryptopp

DIRS = bin lib bank proxy atm
EXECS = bin/bank bin/proxy bin/atm bin/gen-key

LIB = lib/util.o lib/crypto.o
BANK = bank/bank.o bank/main.o
PROXY = proxy/proxy.o proxy/main.o
ATM = atm/atm.o atm/main.o

all: $(EXECS)

bin/bank: $(LIB) $(BANK) atm/atm.o
	$(CXX) $(LFLAGS) $(LIB) atm/atm.o $(BANK) $(LIBFLAGS) -o bin/bank

bin/proxy: $(LIB) $(PROXY)
	$(CXX) $(LFLAGS) $(LIB) $(PROXY) $(LIBFLAGS) -o bin/proxy

bin/atm: $(LIB) $(ATM)
	$(CXX) $(LFLAGS) $(LIB) $(ATM) $(LIBFLAGS) -o bin/atm

bin/gen-key: $(LIB)
	$(CXX) $(LFLAGS) $(LIB) lib/gen-key.cpp $(LIBFLAGS) -o bin/gen-key

bank: bin/bank

proxy: bin/proxy

atm: bin/atm

gen-key: bin/gen-key

%.o: %.cpp
	$(CXX) $(CFLAGS) $< -o $@

clean:
	rm -f *.o $(foreach dir,$(DIRS),$(wildcard $(dir)/*.o)) $(wildcard bin/*)
