CXX = g++
CXXFLAGS = -o3 -std=c++17 -Iinc
SRCDIR = src
INCDIR = inc
BUILDDIR = build
VIZDIR = viz
DATADIR = data
SCRIPTDIR = scripts
ZIPNAME = 08_xrepcim00_xvesela00.zip

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
	./$(TARGET) -s 3600

clean:
	rm -rf $(BUILDDIR) $(TARGET)

runvizmp4: $(TARGET)
	./$(TARGET) -v
	ffmpeg -i viz/frame_%05d.ppm -r 5 output.mp4 -y

runvizgif: $(TARGET)
	./$(TARGET) -v
	ffmpeg -i $(VIZDIR)/frame_%05d.ppm -r 5 output.gif -y

cleanviz:
	rm -rf $(VIZDIR) output.gif output.mp4

runplot:
	./$(TARGET) -p -s 3600
	./$(TARGET) -p -o -s 3600
	./$(SCRIPTDIR)/plot_graphs.py $(DATADIR)/baseline $(DATADIR)/modified $(DATADIR)/graphs

cleanplot:
	rm -rf $(DATADIR)

zip:
	zip -r $(ZIPNAME) $(SRCDIR) $(INCDIR) $(SCRIPTDIR) README.md Makefile documentation.pdf


.PHONY: all run clean