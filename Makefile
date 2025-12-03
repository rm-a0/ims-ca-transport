CXX = g++
CXXFLAGS = -std=c++17 -Wall -Iinc
SRCDIR = src
INCDIR = inc
BUILDDIR = build

SRCS = $(wildcard $(SRCDIR)/*.cpp)
HDRS = $(wildcard $(INCDIR)/*.hpp)
OBJS = $(patsubst $(SRCDIR)/%.cpp,$(BUILDDIR)/%.o,$(SRCS))

TARGET = main

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(OBJS) -o $(TARGET)

$(BUILDDIR)/%.o: $(SRCDIR)/%.cpp | $(BUILDDIR)
	$(CXX) $(CXXFLAGS) -I$(INCDIR) -c $< -o $@

$(BUILDDIR):
	mkdir -p $(BUILDDIR)

run: $(TARGET)
	./$(TARGET)

clean:
	rm -rf $(BUILDDIR) $(TARGET)

runvizmp4: $(TARGET)
	rm -rf viz/ output.gif output.mp4
	./$(TARGET) -v
	ffmpeg -i viz/frame_%05d.ppm -r 5 output.mp4 -y

runvizgif: $(TARGET)
	rm -rf viz/ output.gif output.mp4
	./$(TARGET) -v
	ffmpeg -i viz/frame_%05d.ppm -r 5 output.gif -y

cleanviz:
	rm -rf viz/ output.gif

.PHONY: all run clean