OBJECTS = camera.o main.o mesh2d.o mesh3d.o \
		multitexturer.o triangle3d.o \
		color.o image.o chart.o unwrapper.o \
		packer.o


all: multitex

LIBS = -lfreeimageplus -lopencv_core -lopencv_highgui -lopencv_imgproc -lopencv_objdetect

multitex: $(OBJECTS)
	g++ -o multitex $(OBJECTS) $(LIBS)

clean:
	rm -f *.o
	rm -f multitex