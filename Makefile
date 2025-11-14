.PHONY: clean

main.x: *.cpp *.hpp
	g++ -o main.x *.cpp -g -std=c++17

clean:
	rm main.x