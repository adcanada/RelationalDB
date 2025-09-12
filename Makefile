all: main.o Database.o Relation.o LogicalExpression.o
	g++ -g -Wall -o ./run main.o Database.o Relation.o LogicalExpression.o

main.o: main.cpp
	g++ -g -c main.cpp

Database.o: Database.cpp Database.h
	g++ -g -c Database.cpp

Relation.o: Relation.cpp Relation.h
	g++ -g -c Relation.cpp

LogicalExpression.o: LogicalExpression.cpp LogicalExpression.h
	g++ -g -c LogicalExpression.cpp

clean:
	rm run *.o
