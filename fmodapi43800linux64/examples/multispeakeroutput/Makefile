multispeakeroutput_cpp:	main.cpp
	g++ -O3 -o multispeakeroutput main.cpp ../../api/lib/libfmodex64.so -pthread

multispeakeroutput_c:	main.c
	g++ -O3 -o multispeakeroutput main.c ../../api/lib/libfmodex64.so -pthread

run:
	./multispeakeroutput

clean:
	rm -f multispeakeroutput
