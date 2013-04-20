multiplesoundcard_cpp:	main.cpp
	g++ -O3 -o multiplesoundcard main.cpp ../../api/lib/libfmodex64.so -pthread

multiplesoundcard_c:	main.c
	g++ -O3 -o multiplesoundcard main.c ../../api/lib/libfmodex64.so -pthread

run:
	./multiplesoundcard

clean:
	rm -f multiplesoundcard
