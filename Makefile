#TODO: add headers dependencies

CC := g++
CFLAGS = -std=c++11 -Wall -pthread -O2
LDFLAGS = -pthread

BASE_DIR = .
OBJ_DIR = $(BASE_DIR)/obj
DIST_DIR = $(BASE_DIR)/dist

SERVER_SRC_DIR = $(BASE_DIR)/server
SERVER_OBJ_DIR = $(OBJ_DIR)/$(SERVER_SRC_DIR)
SERVER_DIST_DIR = $(DIST_DIR)/$(SERVER_SRC_DIR)
TEST_SRC_DIR = $(BASE_DIR)/tests
TEST_OBJ_DIR = $(OBJ_DIR)/$(TEST_SRC_DIR)
TEST_DIST_DIR = $(DIST_DIR)/$(TEST_SRC_DIR)

SERVER_SRC = \
  $(SERVER_SRC_DIR)/dispatcher.cpp \
  $(SERVER_SRC_DIR)/led.cpp \
  $(SERVER_SRC_DIR)/main.cpp \
  $(SERVER_SRC_DIR)/server.cpp

TEST_SRC = \
  $(TEST_SRC_DIR)/test_led.cpp \
  $(TEST_SRC_DIR)/test_dispatcher.cpp

SERVER_OBJ = $(patsubst %.cpp, $(OBJ_DIR)/%.o, $(SERVER_SRC))
SERVER_BIN = $(SERVER_DIST_DIR)/led_server
TEST_OBJ = $(patsubst %.cpp, $(OBJ_DIR)/%.o, $(TEST_SRC))
TEST_BIN = $(patsubst $(TEST_OBJ_DIR)/%.o, $(TEST_DIST_DIR)/%, $(TEST_OBJ))

.SECONDEXPANSION:

.PHONY: all clean check todo
all: server tests

DIRECTORY_MARKER_NAME := .directory.marker
DIRECTORY = $$(@D)/$(DIRECTORY_MARKER_NAME)

%/$(DIRECTORY_MARKER_NAME):
	@mkdir -p $(dir $@)
	@touch $@

.PHONY: server
server: $(SERVER_BIN)

$(SERVER_BIN): $(SERVER_OBJ) | $(DIRECTORY)
	$(CC) $(LDFLAGS) $^ -o $@

.PHONY: tests
tests: $(TEST_BIN)

$(TEST_DIST_DIR)/test_dispatcher: $(SERVER_OBJ_DIR)/led.o
$(TEST_DIST_DIR)/test_%: \
  $(SERVER_OBJ_DIR)/%.o \
  $(TEST_OBJ_DIR)/test_%.o \
| $(DIRECTORY)
	$(CC) $(LDFLAGS) $^ -o $@

check: tests
	@ret=0;\
	for i in $(TEST_BIN); do \
	    echo -n "$$(basename $$i): ";     \
	    if $$i; then         \
	      echo OK;           \
	    else                 \
	      echo FAIL;         \
	      ret=1;             \
	    fi;                  \
	done;                    \
	exit $$ret

todo:
	@git grep --color -wn '[T]ODO'

$(SERVER_OBJ): CFLAGS += -I$(SERVER_SRC_DIR)
$(TEST_OBJ): CFLAGS += -I$(SERVER_SRC_DIR)

$(OBJ_DIR)/%.o: %.cpp | $(DIRECTORY)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJ_DIR) $(DIST_DIR)
