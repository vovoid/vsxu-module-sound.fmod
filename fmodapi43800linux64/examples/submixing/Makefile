submixing_cpp:	main.cpp
	g++ -O3 -o submixing main.cpp ../../api/lib/libfmodex64.so -pthread

submixing_c:	main.c
	g++ -O3 -o submixing main.c ../../api/lib/libfmodex64.so -pthread

run:
	./submixing

clean:
	rm -f submixing
