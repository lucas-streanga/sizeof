SRC=sizeof.cpp
EXE = sizeof

CFLAGS = -O2 -std=c++17

INSTALL_PATH = /usr/local/bin

all : $(EXE)

$(EXE) : $(SRC)
	g++ $(CFLAGS) $(SRC) -o $(EXE) -lstdc++fs

debug: CFLAGS = -g -Wall -DDEBUG=1 -std=c++17
debug: all

install : $(EXE)
	@echo "Installing executable to path" $(INSTALL_PATH)
	cp $(EXE) $(INSTALL_PATH)/$(EXE)
