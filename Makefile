CC := clang
CFLAGS := -std=c17 -MP -MD -g -Wall -Wextra -pedantic
LFLAGS :=
IFLAGS := -Isrc
SRC := $(foreach dir,src,$(wildcard $(dir)/*.c) $(wildcard $(dir)/**/*.c) $(wildcard $(dir)/**/**/*.c))
OBJ := $(addprefix obj/,$(notdir $(SRC:%.c=%.o)))
VPATH := $(dir $(SRC))
DEP := $(OBJ:%.o=%.d)
-include $(DEP)
.DEFAULT_GOAL := build
build: $(OBJ)
	@mkdir -p $(dir bin/buildchain)
	$(CC) $(CFLAGS) $(OBJ) -o bin/buildchain $(LFLAGS)
obj/%.o: %.c
	@mkdir -p obj
	$(CC) $(CFLAGS) -c $< -o $@ $(IFLAGS)
.PHONY: clean test_%
clean:
	rm -f $(OBJ) $(DEP) $(wildcard tests/test_*) bin/buildchain
test_%: tests/%.c build
	$(CC) -g -Wall -Wextra -pedantic $(IFLAGS) $< -o tests/$@ $(LFLAGS)
TESTS := $(addprefix test_,$(basename $(notdir $(wildcard tests/*.c))))
test_all: $(TESTS)
	$(foreach test,$(TESTS),./tests/$(test) && echo "" &&) cd .
