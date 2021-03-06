CC = gcc
LD = gcc
INCLUDE = includes
INCLUDES = -I$(INCLUDE)

# -O0 désactive les optimisations à la compilation
# C'est utile pour débugger, par contre en "production"
# on active au moins les optimisations de niveau 2 (-O2).
CFLAGS = -std=c99 -Wall -Wextra -g3 -O3 -Werror -Wshadow -Wconversion -Wdouble-promotion -Wformat=2 -Wformat-truncation=2 -Wundef -fno-common -fstack-usage -fstack-protector-all -Wfloat-equal -Wpointer-arith -Wcast-align -Wstrict-prototypes -Wstrict-overflow=5 -Wwrite-strings -Waggregate-return -ffunction-sections -fdata-sections -fopenmp

# Par défaut, on compile tous les fichiers source (.c) qui se trouvent dans le
# répertoire src/
SRC_FILES = $(wildcard src/*.c)

# Par défaut, la compilation de src/toto.c génère le fichier objet obj/toto.o
OBJ_FILES = $(patsubst src/%.c,obj/%.o,$(SRC_FILES))

all: $(OBJ_FILES)
	+$(MAKE) -C src/tests

obj/%.o: src/%.c
	$(CC) -c $(CFLAGS) $(INCLUDES) $< -o $@
	$(info )
	$(info Call gcc to generate $@ from $<)

clean-tests:
	rm -rf obj/test_* bin/*

clean:
	rm -rf bin/* obj/*

.PHONY: all clean clean-test
