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

CPPFLAGS = -std=c++17 -Wall -Wextra -Werror $(INTN) $(INTE)

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

COMMON = graph.hpp parseCommandLine.hpp utilities.h get_time.hpp hash_bag.h resizable_table.h

all: Connectivity scc LeList

Connectivity: Connectivity.h Connectivity.cpp ldd.hpp union_find_rules.h $(COMMON)
	$(CC) $(CPPFLAGS) Connectivity.cpp -o Connectivity

scc: scc.hpp scc.cpp bfs.hpp reach.hpp $(COMMON)
	$(CC) $(CPPFLAGS) scc.cpp -o scc

LeList: LeList.hpp LeList.cpp resizable_table.h $(COMMON)
	$(CC) $(CPPFLAGS) LeList.cpp -o LeList

clean:
	rm Connectivity scc LeList
