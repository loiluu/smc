all: main_qifs
main_qifs: utils.o contains.o match_regex.o list.o collect_leakage_kal.o collect_leakage.o main.o 
	gcc -o main_qifs main.o list.o utils.o contains.o match_regex.o -lML64i3 -lm -lpthread -lrt -lstdc++  
	gcc -o collect_leakage collect_leakage.o utils.o list.o -lML64i3 -lm -lpthread -lrt -lstdc++
	gcc -o collect_leakage_kal collect_leakage_kal.o -lML64i3 -lm -lpthread -lrt -lstdc++
	javac -d "automaton" automaton/src/dk/brics/automaton/*.java

list.o: list.c
	gcc -c list.c

utils.o: utils.c
	gcc -c utils.c

contains.o: contains.c
	gcc -c contains.c

match_regex.o: match_regex.c
	gcc -c match_regex.c

collect_leakage_kal.o: collect_leakage_kal.c
	gcc -c collect_leakage_kal.c

collect_leakage.o: collect_leakage.c
	gcc -c collect_leakage.c

main.o: main.c
	gcc -c main.c

clean:
	\rm *.o
	\rm main_qifs
	\rm collect_leakage
	\rm collect_leakage_kal
	\rm -rf automaton/dk/
	\rm *.pyc
