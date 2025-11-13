main.x: *.cpp *.hpp Makefile
	g++ -std=c++17 main.cpp linkedlist.cpp LCRS.cpp STLL.cpp AST.cpp -o main.x -g

clean:
	rm main.x