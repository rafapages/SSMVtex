# ------------------------------------------------
# Makefile
#
# Date  : 2015-01-24
# Author: Rafa Pagés + Daniel Berjón, but basically thanks to the following link:
#  		  http://stackoverflow.com/questions/7004702/how-can-i-create-a-makefile-for-c-projects-with-src-obj-and-bin-subdirectories
# ------------------------------------------------

TARGET   = multitex

CXX      = g++
CFLAGS   = -std=c++11 -Wall -I.

LINKER   = g++ -o
LFLAGS   = -Wall -I. -lm
LIBS = -lfreeimageplus -lopencv_core -lopencv_highgui -lopencv_imgproc -lopencv_objdetect -lopencv_photo

SRCDIR   = src
OBJDIR   = obj
BINDIR   = bin

SOURCES  := $(wildcard $(SRCDIR)/*.cpp)
INCLUDES := $(wildcard $(SRCDIR)/*.h)
OBJECTS  := $(SOURCES:$(SRCDIR)/%.cpp=$(OBJDIR)/%.o)
rm       = rm -f


$(BINDIR)/$(TARGET): $(OBJECTS)
	@mkdir -p $(BINDIR)
	$(LINKER) $@ $(LFLAGS) $(OBJECTS) $(LIBS)
	@echo "Linking complete!"

$(OBJECTS): $(OBJDIR)/%.o : $(SRCDIR)/%.cpp
	@mkdir -p $(OBJDIR)
	$(CXX) $(CFLAGS) -c $< -o $@
	@echo "Compiled "$<" successfully!"

clean:
	$(rm) $(OBJECTS)
	$(rm) $(BINDIR)/$(TARGET)
	@echo "Cleanup complete!"