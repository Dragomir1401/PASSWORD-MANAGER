build:
	g++ -Ofast -static -std=c++17 -Wall -Wextra main.cpp -o main

run:
	./main

clean:
	rm -rf main
