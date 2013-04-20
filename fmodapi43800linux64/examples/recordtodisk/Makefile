recordtodisk_cpp:	main.cpp
	g++ -O3 -o recordtodisk main.cpp ../../api/lib/libfmodex64.so -pthread

recordtodisk_c:	main.c
	g++ -O3 -o recordtodisk main.c ../../api/lib/libfmodex64.so -pthread

run:
	./recordtodisk

clean:
	rm -f recordtodisk
