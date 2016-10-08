
tripeaks: tripeaks.cpp
	gcc -g -Wall -o $@ tripeaks.cpp

run: tripeaks
	./$<

clean:
	rm -f tripeaks.exe
