
tripeaks: tripeaks.cpp
	g++ -Wall -o $@ tripeaks.cpp

tripeaks_test: tripeaks.cpp
	g++ -g -Wall -DTEST -o $@ tripeaks.cpp -lcppunit


run: tripeaks
	./$<

bench: tripeaks
	time ./$< "A  6  Q" "73 q3 8a" "902K25274" "62k96jjajq" 9470901k658845530j834qk7

test: tripeaks_test
	./$< --test

clean:
	rm -f tripeaks.exe
	rm -f tripeaks_test.exe
