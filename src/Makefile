ifdef BREAKDOWN
BREAK = -DBREAKDOWN
endif

ifdef NLONG
INTN = -DNLONG
endif

ifdef ELONG
INTE = -DELONG
endif

ifdef GCC
CC = clang++
else
CC = g++
endif

INCLUDE_PATH = -I../parlaylib/include/

CPPFLAGS = -std=c++17 -Wall -Wextra -Werror $(INTN) $(INTE)

ifdef CILKPLUS
CFLAGS = -DPARLAY_CILKPLUS -DCILK -fcilkplus
else ifdef OPENCILK
CFLAGS = -DPARLAY_OPENCILK -DCILK -fopencilk
else ifdef SERIAL
CFLAGS = -DPARLAY_SEQUENTIAL
else
CFLAGS = -pthread
endif

ifdef DEBUG
CPPFLAGS += -DDEBUG -O0 -g
else
CPPFLAGS += -O3 -mcx16 -march=native
endif

ifdef STDALLOC
CPPFLAGS += -DPARLAY_USE_STD_ALLOC
endif

COMMON = graph.hpp parseCommandLine.hpp utilities.h get_time.hpp hash_bag.h resizable_table.h

all: Connectivity scc scc_serial scc_round LeList

Connectivity: Connectivity.h Connectivity.cpp ldd.hpp union_find_rules.h $(COMMON)
	$(CC) $(CPPFLAGS) $(CFLAGS) $(INCLUDE_PATH) Connectivity.cpp -o Connectivity

scc: scc.hpp scc.cpp bfs.hpp reach.hpp $(COMMON)
	$(CC) $(CPPFLAGS) $(CFLAGS) $(INCLUDE_PATH) scc.cpp -o scc

scc_round: scc.hpp scc.cpp bfs.hpp reach.hpp $(COMMON)
	$(CC) $(CPPFLAGS) $(CFLAGS) -DROUND $(INCLUDE_PATH) scc.cpp -o scc_round

scc_serial: scc.hpp scc.cpp bfs.hpp reach.hpp $(COMMON)
	$(CC) $(CPPFLAGS) -DPARLAY_SEQUENTIAL $(INCLUDE_PATH) scc.cpp -o scc_serial

LeList: LeList.hpp LeList.cpp resizable_table.h $(COMMON)
	$(CC) $(CPPFLAGS) $(CFLAGS) $(INCLUDE_PATH) LeList.cpp -o LeList

clean:
	rm Connectivity scc scc_round scc_serial LeList 
