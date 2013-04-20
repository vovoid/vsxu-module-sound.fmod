pitchdetection_cpp:	main.cpp
	g++ -O3 -o pitchdetection main.cpp ../../api/lib/libfmodex64.so -pthread

pitchdetection_c:	main.c
	g++ -O3 -o pitchdetection main.c ../../api/lib/libfmodex64.so -pthread

run:
	./pitchdetection

clean:
	rm -f pitchdetection
