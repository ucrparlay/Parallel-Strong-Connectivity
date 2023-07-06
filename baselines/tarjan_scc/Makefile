ifdef BREAKDOWN
BREAK = -DBREAKDOWN
endif

ifdef NLONG
INTN = -DNLONG
endif

ifdef ELONG
INTE = -DELONG
endif

ifdef N2LONG
INTN2 = -DN2LONG
endif

ifdef CLANG
CC = clang++
else
CC = g++
endif

CPPFLAGS = -std=c++17 -Wall -Wextra -Werror $(INTN) $(INTE) $(INTN2) $(BREAK)

ifdef CILKPLUS
CPPFLAGS += -DPARLAY_CILKPLUS -DCILK -fcilkplus
else ifdef OPENCILK
CPPFLAGS += -DPARLAY_OPENCILK -DCILK -fopencilk
else ifdef SERIAL
CPPFLAGS += -DPARLAY_SEQUENTIAL
else
CPPFLAGS += -pthread
endif

ifdef DEBUG
CPPFLAGS += -DDEBUG -O0 -g
else
CPPFLAGS += -O3 -mcx16 -march=native
endif

ifdef STDALLOC
CPPFLAGS += -DPARLAY_USE_STD_ALLOC
endif

# COMMON = graph.hpp parseCommandLine.hpp utilities.h parallel.h
INCLUDE_PATH = -I../../parlaylib/include/ -I../../src/

all: tarjan_scc
tarjan_scc: tarjan_scc.hpp tarjan_scc.cpp 
	$(CC) $(CPPFLAGS) $(INCLUDE_PATH) tarjan_scc.cpp -o tarjan_scc
clean:
	rm -f tarjan_scc