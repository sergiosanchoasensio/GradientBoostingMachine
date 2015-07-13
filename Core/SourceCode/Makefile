# How to use valgrind:
# valgrind --track-origins=yes --leak-check=full -v --show-reachable=yes ./nomExec 
#valgrind --track-origins=yes --leak-check=full -v --show-reachable=yes ./GBM --train=../DataSet/iris.arff --minimumnumberofinstancesperleaf=8 --maximumnumberofleaves=0 --maximumdepth=2 --minimumvariance=0.003

OBJS = Config.o DataSet.o BinaryTree.o GradientBoosting.o Main.o

#DEBUGING FLAGS
CFLAGS = -Wall -Wextra -std=c++11 -c -O0 -ggdb
OFLAGS =  -o
P_FLAGS = -g -c

#MEMORY PROFILING FLAGS
#CFLAGS = -Wall -Wextra -std=c++11 -c -O3 -pg -ggdb -fno-inline -mtune=native -march=native -msse -msse2 -mssse3 -msse4 -mfpmath=sse -finline-functions -mfpmath=sse -m64 -DLEVEL1_DCACHE_LINESIZE=`getconf LEVEL1_DCACHE_LINESIZE`
#OFLAGS = -g -pg -fno-inline -o
#P_FLAGS = -g -pg -c

#RUNNING TAGS
#CFLAGS = -Wall -Wextra -std=c++11 -c -O3 -mtune=native -march=native -msse -msse2 -mssse3 -msse4 -mfpmath=sse -finline-functions -mfpmath=sse -m64 -DLEVEL1_DCACHE_LINESIZE=`getconf LEVEL1_DCACHE_LINESIZE`
#OFLAGS = -DLEVEL1_DCACHE_LINESIZE=`getconf LEVEL1_DCACHE_LINESIZE` -o 
#P_FLAGS = -c 

all: executable

Config.o: Config.h Config.cpp
	g++ ${CFLAGS} Config.cpp

DataSet.o: Config.o DataSet.h DataSet.cpp NominalFeature.h
	g++ ${CFLAGS} DataSet.cpp

BinaryTree.o: Config.o BinaryTree.h BinaryTree.cpp DataSet.o
	g++ ${CFLAGS} BinaryTree.cpp

GradientBoosting.o: GradientBoosting.cpp GradientBoosting.cpp Config.o DataSet.o BinaryTree.o
	g++ ${CFLAGS} GradientBoosting.cpp

Main.o: Config.o DataSet.o BinaryTree.o GradientBoosting.o Main.cpp
	g++ ${CFLAGS} Main.cpp

executable: ${OBJS}
	g++ ${OBJS} ${OFLAGS} GBM -lm -pthread
	
clean:
	rm -f *.o GBM core* *~
