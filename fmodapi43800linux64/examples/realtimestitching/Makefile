realtimestitching_cpp:	main.cpp
	g++ -O3 -o realtimestitching main.cpp ../../api/lib/libfmodex64.so -pthread

realtimestitching_c:	main.c
	g++ -O3 -o realtimestitching main.c ../../api/lib/libfmodex64.so -pthread

run:
	./realtimestitching

clean:
	rm -f realtimestitching
