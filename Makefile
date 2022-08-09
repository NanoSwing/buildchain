CC := clang
CFLAGS := -std=c17 -MP -MD -g -Wextra -pedantic
LFLAGS :=
IFLAGS :=
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
.PHONY: clean
clean:
	rm -f $(OBJ) $(DEP) bin/buildchain
