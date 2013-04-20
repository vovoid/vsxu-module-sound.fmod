channelgroups_cpp:	main.cpp
	g++ -O3 -o channelgroups main.cpp ../../api/lib/libfmodex64.so -pthread

channelgroups_c:	main.c
	g++ -O3 -o channelgroups main.c ../../api/lib/libfmodex64.so -pthread

run:
	./channelgroups

clean:
	rm -f channelgroups
