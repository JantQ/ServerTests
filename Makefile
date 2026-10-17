CXX := g++
CXXFLAGS := -std=c++26 -Wall -g

CLIENT_DIR := client
SERVER_DIR := server
CLIENT_BIN := $(CLIENT_DIR)/client
SERVER_BIN := $(SERVER_DIR)/server

.PHONY: all clean

all: $(CLIENT_BIN) $(SERVER_BIN)

$(CLIENT_BIN): $(CLIENT_DIR)/src/main.cc $(CLIENT_DIR)/include/networking.hh
	$(CXX) $(CXXFLAGS) -I$(CLIENT_DIR)/include -o $@ $(CLIENT_DIR)/src/main.cc

$(SERVER_BIN): $(SERVER_DIR)/src/main.cc
	$(CXX) $(CXXFLAGS) -o $@ $(SERVER_DIR)/src/main.cc

clean:
	rm -f $(CLIENT_BIN) $(SERVER_BIN)
