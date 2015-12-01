
CXX = g++
DEBUG = -g
CFLAGS = -Wall $(DEBUG) -c
LFLAGS = -Wall $(DEBUG)
LIBFLAGS = -lssl -lcrypto

DIRS = bin lib bank proxy atm
EXECS = bin/bank bin/proxy bin/atm

LIB = lib/util.o
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

bank: bin/bank

proxy: bin/proxy

atm: bin/atm

%.o: %.cpp
	$(CXX) $(CFLAGS) $< -o $@


clean:
	rm -f *.o $(foreach dir,$(DIRS),$(wildcard $(dir)/*.o)) $(wildcard bin/*)
