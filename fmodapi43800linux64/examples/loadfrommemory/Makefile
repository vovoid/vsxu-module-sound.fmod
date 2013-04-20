loadfrommemory_cpp:	main.cpp
	g++ -O3 -o loadfrommemory main.cpp ../../api/lib/libfmodex64.so -pthread

loadfrommemory_c:	main.c
	g++ -O3 -o loadfrommemory main.c ../../api/lib/libfmodex64.so -pthread

run:
	./loadfrommemory

clean:
	rm -f loadfrommemory
