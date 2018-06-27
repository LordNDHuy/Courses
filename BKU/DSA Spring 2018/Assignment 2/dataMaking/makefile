all: dataMaking

DEFS = -DUSE_SOME_DEF

dataMaking: sampleDataMaking.o
	g++ sampleDataMaking.o -std=c++11 -o dataMaking $(DEFS)
sampleDataMaking.o: sampleDataMaking.cpp
	g++ -c sampleDataMaking.cpp -std=c++11 $(DEFS)

clean:
	rm *.o dataMaking 
