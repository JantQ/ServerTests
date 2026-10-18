CXX := g++
CXXFLAGS := -std=c++23 -Wall -g

CLIENT_DIR := client
SERVER_DIR := server
COMMON_DIR := common
CLIENT_BIN := $(CLIENT_DIR)/client
SERVER_BIN := $(SERVER_DIR)/server

.PHONY: all clean

all: $(CLIENT_BIN) $(SERVER_BIN)

$(CLIENT_BIN): $(CLIENT_DIR)/src/main.cc
	$(CXX) $(CXXFLAGS) -I$(CLIENT_DIR)/include/ -I$(COMMON_DIR)/include -o $@ $(CLIENT_DIR)/src/main.cc

$(SERVER_BIN): $(SERVER_DIR)/src/main.cc
	$(CXX) $(CXXFLAGS) -I$(SERVER_DIR)/include/ -I$(COMMON_DIR)/include -o $@ $(SERVER_DIR)/src/main.cc

clean:
	rm -f $(CLIENT_BIN) $(SERVER_BIN)