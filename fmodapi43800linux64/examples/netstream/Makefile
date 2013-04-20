netstream_cpp:	main.cpp
	g++ -O3 -o netstream main.cpp ../../api/lib/libfmodex64.so -pthread

netstream_c:	main.c
	g++ -O3 -o netstream main.c ../../api/lib/libfmodex64.so -pthread

run:
	./netstream

clean:
	rm -f netstream
