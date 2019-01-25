OBJS = main.cpp sdlwrapper.cpp SFont.c sprite.cpp

all: $(OBJS)
	g++ $(OBJS) -fpermissive -w -lSDL -o rBlocks