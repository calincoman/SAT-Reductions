CC = g++
CFLAGS = -Wall -Wextra -std=c++17 -g
LDFLAGS = -lm

build: retele reclame registre

run_retele:
	./retele

run_reclame:
	./reclame

run_registre:
	./registre

retele: retele.cpp 
	$(CC) $(CFLAGS) $(LDFLAGS) $< -o $@ 

reclame: reclame.cpp 
	$(CC) $(CFLAGS) $(LDFLAGS) $< -o $@ 

registre: registre.cpp 
	$(CC) $(CFLAGS) $(LDFLAGS) $< -o $@ 

clean:
	rm -f retele reclame registre

.PHONY: build clean
