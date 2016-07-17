prog: coordcube.o cubiecube.o optiqtm.o pruning.o symmetry.o cube_input.o
	gcc coordcube.o cubiecube.o optiqtm.o pruning.o symmetry.o cube_input.o -o optiqtm

coordcube.o : coordcube.c
	gcc -O3 -Wall -c coordcube.c

cubiecube.o : cubiecube.c
	gcc -O3 -Wall -c cubiecube.c

optiqtm.o : optiqtm.c
	gcc -O3 -Wall -c optiqtm.c

pruning.o : pruning.c
	gcc -O3 -Wall -c pruning.c

symmetry.o : symmetry.c
	gcc -O3 -Wall -c symmetry.c

cube_input.o : cube_input.c
	gcc -O3 -Wall -c cube_input.c

