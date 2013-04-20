useplugins_cpp:	main.cpp
	g++ -O3 -o useplugins main.cpp ../../api/lib/libfmodexp64.so -pthread

useplugins_c:	main.c
	g++ -O3 -o useplugins main.c ../../api/lib/libfmodexp64.so -pthread

run:
	./useplugins

clean:
	rm -f useplugins
