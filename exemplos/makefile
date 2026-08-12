CC       = gcc
STD      = -std=c99
WARNINGS = -Wall -Wextra -Werror -pedantic -Wconversion -Wstrict-prototypes
SANITIZE = -fsanitize=address,undefined
OPT      = -O2
LDLIBS   = -lm

SRCS := $(wildcard *.c)
BINS := $(SRCS:.c=)

.PHONY: $(SRCS) clean
$(SRCS):
	$(CC) $(STD) $(WARNINGS) $(SANITIZE) $(OPT) $@ -o $(@:.c=) $(LDLIBS)

clean:
	rm -f $(BINS)
