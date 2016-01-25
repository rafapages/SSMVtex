OBJECTS = camera.o main.o mesh2d.o mesh3d.o \
		multitexturer.o triangle.o color.o \
		image.o chart.o unwrapper.o packer.o


all: multitex

LIBS = -lfreeimageplus -lopencv_core -lopencv_highgui -lopencv_imgproc -lopencv_objdetect -lopencv_photo
CXXFLAGS = -Wall -pedantic -std=c++98


multitex: $(OBJECTS)
	g++ $(CXXFLAGS) -o multitex $(OBJECTS) $(LIBS)

clean:
	rm -f *.o
	rm -f multitex