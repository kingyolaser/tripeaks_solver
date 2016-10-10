
tripeaks: tripeaks.cpp
	g++ -Wall -o $@ tripeaks.cpp

tripeaks_test: tripeaks.cpp
	g++ -g -Wall -DTEST -o $@ tripeaks.cpp -lcppunit


run: tripeaks
	./$<

test: tripeaks_test
	./$< --test

clean:
	rm -f tripeaks.exe
	rm -f tripeaks_test.exe
