CC     := gcc
CFLAGS := -std=c11 -Wall -Wextra -g -O0 -Iinclude -MMD -MP

# Engine sources (everything except tests)
SRC := src/platform/platform_linux.c
OBJ := $(SRC:src/%.c=build/%.o)

TESTS := bin/test_plat

.PHONY: all clean
.SECONDARY: # keep intermediate .o files instead of letting make delete them
all: $(TESTS)

# Link step: test object + engine objects -> executable
bin/test_%: build/tests/test_%.o $(OBJ)
	@mkdir -p $(@D)
	$(CC) $^ -o $@

# Compile step: one .c -> one .o
build/%.o: src/%.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $< -o $@

build/tests/%.o: tests/%.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf build bin

# Auto-generated header dependencies (-MMD writes these .d files)
-include $(OBJ:.o=.d) $(TESTS:bin/%=build/tests/%.d)
