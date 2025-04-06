TARGET=main

$(TARGET): main.o shaders.o galaxy.o universe.o
	c++ -o $(TARGET) main.o shaders.o galaxy.o universe.o -lglfw -lGLEW -lGLU -lGL -lm

main.o: shaders.h galaxy.h universe.h

shaders.o: shaders.h

galaxy.o: galaxy.h

universe.o: universe.h galaxy.h