tester: bin/json.o bin/parser.o test/test.cpp
	g++ -o tester bin/json.o bin/parser.o -g test/test.cpp -I include

bin/json.o: src/json.cpp include/json.h
	g++ -std=c++11 -g -c src/json.cpp -I include -o bin/json.o

bin/parser.o: src/parser.cpp include/json.h
	g++ -std=c++11 -g -c src/parser.cpp -I include -o bin/parser.o

tests: tester
	@echo;
	@./tester
