all: main.o Database.o Relation.o LogicalExpression.o
	g++ -Wall -o ./run main.o Database.o Relation.o LogicalExpression.o

main.o: main.cpp
	g++ -c main.cpp

Database.o: Database.cpp Database.h
	g++ -c Database.cpp

Relation.o: Relation.cpp Relation.h
	g++ -c Relation.cpp

LogicalExpression.o: LogicalExpression.cpp LogicalExpression.h
	g++ -c LogicalExpression.cpp

clean:
	rm *.o
