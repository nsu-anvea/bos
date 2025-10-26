CC = gcc
LDFLAGS = -lpthread

SRC_DIR = src
BLD_DIR = build

define compile
$(CC) $< -o $(BLD_DIR)/$(1)
endef


all: 1 2 3 4

1 2 3 4: | $(BLD_DIR)

1: $(SRC_DIR)/1-interception-of-sigint.c
	$(call compile,1-ios)

2: $(SRC_DIR)/2-masks-and-signal-blocking.c
	$(call compile,2-masb)

3: $(SRC_DIR)/3-sigwait.c
	$(call compile,3-s)

4: $(SRC_DIR)/4-which-thread-processes-signal.c
	$(call compile,4-wtps) $(LDFLAGS)


run_1: $(BLD_DIR)/1-ios
	./$<

run_2: $(BLD_DIR)/2-masb
	./$<

run_3: $(BLD_DIR)/3-s
	./$<

run_4: $(BLD_DIR)/4-wtps
	./$<

RUNNERS = run_1 run_2 run_3 run_4


$(BLD_DIR):
	mkdir -p $@

clean:
	rm -rf $(BLD_DIR)

.PHONY: all clean RUNNERS
