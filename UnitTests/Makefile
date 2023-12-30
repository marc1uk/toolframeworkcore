includes= -I ../include
libs= -L ../lib -lStore

.SECONDARY: $(%.o)

all: $(patsubst %.cpp, %.exe, $(wildcard *.cpp))

%.exe: %.o
	g++  $< -o $@ $(includes) $(libs)

%.o: %.cpp
	g++ -c $< -o $@ $(includes)

clean: 
	rm -f *.exe

cleanfull:
	rm -f *.exe
	rm -f *.o
