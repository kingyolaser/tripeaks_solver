
tripeaks: tripeaks.cpp
	g++ -g -Wall -o $@ tripeaks.cpp -lcppunit

run: tripeaks
	./$<

test: tripeaks
	./$< --test

clean:
	rm -f tripeaks.exe
