
tripeaks: tripeaks.cpp
	g++ -Wall -O3 -o $@ tripeaks.cpp

tripeaks_test: tripeaks.cpp
	g++ -g -Wall -DTEST -o $@ tripeaks.cpp -lcppunit


run: tripeaks
	./$<

bench: tripeaks
	time ./$< "A  6  Q" "73 q3 8a" "902K25274" "62k96jjajq" 9470901k658845530j834qk7

bench2: tripeaks
	time ./$< "0  6  6" "0Q 85 03" "Q9A794A93" "22540376ja" 2k534jq4752j91kqj88768kk

test: tripeaks_test
	./$< --test

clean:
	rm -f tripeaks.exe
	rm -f tripeaks_test.exe
