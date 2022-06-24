CC := gcc
CFLAGS  := -pthread -std=gnu11 -Wall -Wextra -Wpedantic -Wmissing-declarations -Wmissing-prototypes -Wold-style-definition
LDLIBS := -pthread -lm -Isrc

EXE := rdu
SRC_DIR := src
BIN_DIR := build
TEST_DIR := test/unit
TEST_OBJ_DIR := build/test

SRC := $(wildcard $(SRC_DIR)/*.c)
SRC += $(wildcard $(SRC_DIR)/util/*.c)
OBJ := $(SRC:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)
SRC_NO_MAIN := $(filter-out $(SRC_DIR)/rdu.c, $(SRC))
TEST_SRC := $(wildcard $(TEST_DIR)/*.c)
TEST_SRC += $(wildcard $(TEST_DIR)/*.h)

DEBUG_DIR = $(BIN_DIR)/debug
DEBUG_EXE = $(DEBUG_DIR)/$(EXE)
DEBUG_OBJ := $(SRC:$(SRC_DIR)/%.c=$(DEBUG_DIR)/obj/%.o)
DEBUG_OBJ_DIR := $(DEBUG_DIR)/obj $(DEBUG_DIR)/obj/util
DEBUG_CFLAGS = -g -O0 -DDEBUG

RELEASE_DIR = $(BIN_DIR)/release
RELEASE_EXE = $(RELEASE_DIR)/$(EXE)
RELEASE_OBJ := $(SRC:$(SRC_DIR)/%.c=$(RELEASE_DIR)/obj/%.o)
RELEASE_OBJ_DIR := $(RELEASE_DIR)/obj $(RELEASE_DIR)/obj/util
RELEASE_CFLAGS := -O3 -march=native

TEST_DIR = $(BIN_DIR)/test
TEST_EXE := $(TEST_DIR)/$(EXE)-test
TEST_OBJ := $(TEST_SRC:$(TEST_DIR)/%.c=$(TEST_DIR)/obj/%.o)
TEST_OBJ += $(SRC_NO_MAIN:$(SRC_DIR)/%.c=$(TEST_DIR)/obj/%.o)
TEST_OBJ_DIR := $(TEST_DIR)/obj $(TEST_DIR)/obj/util
RDU_PERF_TEST_DIR ?= ~

.PHONY: all debug release clean test time

# Compile program
all: debug

# Debug build

debug: $(DEBUG_EXE)

$(DEBUG_EXE): $(DEBUG_OBJ) | $(BIN_DIR) $(DEBUG_DIR) $(DEBUG_OBJ_DIR)
	$(CC) $^ $(LDLIBS) -o $@

$(DEBUG_DIR)/obj/%.o: $(SRC_DIR)/%.c | $(DEBUG_DIR) $(DEBUG_OBJ_DIR)
	$(CC) $(CFLAGS) $(DEBUG_CFLAGS) -c $< -o $@

# Release build

release: ${RELEASE_EXE}

$(RELEASE_EXE): $(RELEASE_OBJ) | $(BIN_DIR) $(RELEASE_DIR) $(RELEASE_OBJ_DIR)
	$(CC) $^ $(LDLIBS) -o $@

$(RELEASE_DIR)/obj/%.o: $(SRC_DIR)/%.c | $(RELEASE_DIR) $(RELEASE_OBJ_DIR)
	$(CC) $(CFLAGS) $(RELEASE_CFLAGS) -c $< -o $@

# Unit test build

unit-test-exe: $(TEST_EXE)
	
$(TEST_EXE): $(TEST_OBJ) $(DEBUG) | $(BIN_DIR) $(TEST_DIR) $(TEST_OBJ_DIR)
	$(CC) $^ $(LDLIBS) -o $@

$(TEST_DIR)/obj/%.o: $(SRC_DIR)/%.c | $(TEST_DIR) $(TEST_OBJ_DIR)
	$(CC) $(CFLAGS) $(DEBUG_CFLAGS) -c $< -o $@

$(BIN_DIR) $(DEBUG_DIR) $(DEBUG_OBJ_DIR) $(RELEASE_DIR) $(RELEASE_OBJ_DIR) $(TEST_DIR) $(TEST_OBJ_DIR):
	mkdir -p $@

clean:
	@$(RM) -rv $(BIN_DIR)

# Test commands

# Run all tests
test: unit-test validate-test

# Run unit tests
unit-test: unit-test-exe
	@echo [TEST] Running unit tests...
	./build/test/rdu-test
	@echo "[TEST] \e[0;32mPassed unit tests!\e[0m"
	@echo Running memory leak test...
	valgrind --leak-check=full --error-exitcode=1 --log-fd=2 ./build/test/rdu-test 1>/dev/null
	@echo "[TEST] \e[0;32mPassed memory leak test!\e[0m"

# Run validity test against du
validate-test: debug
	bash test/test.sh "build/debug/rdu" "src" "." ".."


# Run performance test
perf-test: release
	bash test/time.sh --count 10 build/release/rdu -j 32 $(RDU_PERF_TEST_DIR)

# Run complete performance test
perf-test-complete: release
	bash test/complete_time.sh $(RDU_PERF_TEST_DIR) 128 10

-include $(OBJ:.o=.d)