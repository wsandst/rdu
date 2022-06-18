SRC_DIR := src
SRC := $(wildcard $(SRC_DIR)/*.c)
BIN_DIR := build

CC := gcc

# Debug build
DEBUG_DIR := ${BIN_DIR}/debug
DEBUG_OBJ_DIR := ${DEBUG_DIR}/obj
DEBUG_EXE := $(BIN_DIR)/debug/mdu
DEBUG_OBJ := $(SRC:$(SRC_DIR)/%.c=$(DEBUG_OBJ_DIR)/%.o)
DEBUG_CFLAGS := -g

# Release build
RELEASE_DIR := ${BIN_DIR}/release
RELEASE_OBJ_DIR := ${RELEASE_DIR}/obj
RELEASE_EXE := $(BIN_DIR)/release/mdu
RELEASE_OBJ := $(SRC:$(SRC_DIR)/%.c=$(RELEASE_OBJ_DIR)/%.o)
RELEASE_CFLAGS := -O3 -march=native

CFLAGS  := -pthread -std=gnu11 -Wall -Wextra -Wpedantic -Wmissing-declarations -Wmissing-prototypes -Wold-style-definition
LDLIBS := -pthread -lm -Isrc

.PHONY: all debug release clean test time

# Compile program
all: debug

# Debug build

debug: $(DEBUG_EXE)

$(DEBUG_EXE): $(DEBUG_OBJ) | $(BIN_DIR) $(DEBUG_DIR) $(DEBUG_OBJ_DIR)
	$(CC) $^ $(LDLIBS) -o $@

$(DEBUG_OBJ_DIR)/mdu.o: $(SRC_DIR)/mdu.c $(SRC_DIR)/args.h $(SRC_DIR)/disk_usage.h $(SRC_DIR)/queue.h | $(DEBUG_OBJ_DIR)
	$(CC) ${CFLAGS} $(DEBUG_CFLAGS) -c $< -o $@

$(DEBUG_OBJ_DIR)/args.o: $(SRC_DIR)/args.c $(SRC_DIR)/args.h $(SRC_DIR)/helpers.h | $(DEBUG_OBJ_DIR)
	$(CC) ${CFLAGS} $(DEBUG_CFLAGS) -c $< -o $@

$(DEBUG_OBJ_DIR)/helpers.o: $(SRC_DIR)/helpers.c $(SRC_DIR)/helpers.h | $(DEBUG_OBJ_DIR)
	$(CC) ${CFLAGS} $(DEBUG_CFLAGS) -c $< -o $@

$(DEBUG_OBJ_DIR)/disk_usage.o: $(SRC_DIR)/disk_usage.c $(SRC_DIR)/disk_usage.h $(SRC_DIR)/helpers.h $(SRC_DIR)/args.h $(SRC_DIR)/stack.h | $(DEBUG_OBJ_DIR)
	$(CC) ${CFLAGS} $(DEBUG_CFLAGS) -c $< -o $@

$(DEBUG_OBJ_DIR)/stack.o: $(SRC_DIR)/stack.c $(SRC_DIR)/stack.h $(SRC_DIR)/helpers.h | $(DEBUG_OBJ_DIR)
	$(CC) ${CFLAGS} $(DEBUG_CFLAGS) -c $< -o $@

$(DEBUG_OBJ_DIR)/queue.o: $(SRC_DIR)/queue.c $(SRC_DIR)/queue.h $(SRC_DIR)/helpers.h | $(DEBUG_OBJ_DIR)
	$(CC) ${CFLAGS} $(DEBUG_CFLAGS) -c $< -o $@

# Release build

release: ${RELEASE_EXE}

$(RELEASE_EXE): $(RELEASE_OBJ) | $(BIN_DIR) $(RELEASE_DIR) $(RELEASE_OBJ_DIR)
	$(CC) $^ $(LDLIBS) -o $@

$(RELEASE_OBJ_DIR)/mdu.o: $(SRC_DIR)/mdu.c $(SRC_DIR)/args.h $(SRC_DIR)/disk_usage.h $(SRC_DIR)/stack.h | $(RELEASE_OBJ_DIR)
	$(CC) ${CFLAGS} $(RELEASE_CFLAGS) -c $< -o $@

$(RELEASE_OBJ_DIR)/args.o: $(SRC_DIR)/args.c $(SRC_DIR)/args.h $(SRC_DIR)/helpers.h | $(RELEASE_OBJ_DIR)
	$(CC) ${CFLAGS} $(RELEASE_CFLAGS) -c $< -o $@

$(RELEASE_OBJ_DIR)/helpers.o: $(SRC_DIR)/helpers.c $(SRC_DIR)/helpers.h | $(RELEASE_OBJ_DIR)
	$(CC) ${CFLAGS} $(RELEASE_CFLAGS) -c $< -o $@

$(RELEASE_OBJ_DIR)/disk_usage.o: $(SRC_DIR)/disk_usage.c $(SRC_DIR)/disk_usage.h $(SRC_DIR)/helpers.h $(SRC_DIR)/args.h $(SRC_DIR)/stack.h | $(RELEASE_OBJ_DIR)
	$(CC) ${CFLAGS} $(RELEASE_CFLAGS) -c $< -o $@

$(RELEASE_OBJ_DIR)/stack.o: $(SRC_DIR)/stack.c $(SRC_DIR)/stack.h $(SRC_DIR)/helpers.h | $(RELEASE_OBJ_DIR)
	$(CC) ${CFLAGS} $(RELEASE_CFLAGS) -c $< -o $@

$(RELEASE_OBJ_DIR)/queue.o: $(SRC_DIR)/queue.c $(SRC_DIR)/queue.h $(SRC_DIR)/helpers.h | $(RELEASE_OBJ_DIR)
	$(CC) ${CFLAGS} $(RELEASE_CFLAGS) -c $< -o $@

$(BIN_DIR) $(DEBUG_DIR) $(DEBUG_OBJ_DIR) $(RELEASE_DIR) $(RELEASE_OBJ_DIR):
	mkdir -p $@

clean:
	@$(RM) -rv $(BIN_DIR)

test: $(DEBUG_EXE)
	bash test/test.sh "build/debug/mdu" "src" "." "$$HOME/repos/umu"

time: $(RELEASE_EXE)
	bash test/pkg_time.sh 64

time-complete: $(RELEASE_EXE)
	bash test/pkg_time_graph.sh 128 20

-include $(OBJ:.o=.d)