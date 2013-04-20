effects_cpp:	main.cpp
	g++ -O3 -o effects main.cpp ../../api/lib/libfmodex64.so -pthread

effects_c:	main.c
	g++ -O3 -o effects main.c ../../api/lib/libfmodex64.so -pthread

run:
	./effects

clean:
	rm -f effects
