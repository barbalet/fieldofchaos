CC ?= cc
CFLAGS ?= -std=c11 -Wall -Wextra -pedantic -O2
CPPFLAGS ?= -Iengine/include
TARGET = fieldofchaos
CHAR_TARGET = character_create
ENGINE_SRC = engine/fieldofchaos_engine.c
ENGINE_OBJ = engine/fieldofchaos_engine.o
TEST_TARGET = tests/test_engine

all: $(TARGET) $(CHAR_TARGET)

$(TARGET): fieldofchaos.c $(ENGINE_OBJ)
	$(CC) $(CPPFLAGS) $(CFLAGS) -o $(TARGET) fieldofchaos.c $(ENGINE_OBJ)

$(CHAR_TARGET): character_create.c $(ENGINE_OBJ)
	$(CC) $(CPPFLAGS) $(CFLAGS) -o $(CHAR_TARGET) character_create.c $(ENGINE_OBJ)

$(ENGINE_OBJ): $(ENGINE_SRC) engine/include/fieldofchaos_engine.h
	$(CC) $(CPPFLAGS) $(CFLAGS) -c -o $(ENGINE_OBJ) $(ENGINE_SRC)

$(TEST_TARGET): tests/test_engine.c $(ENGINE_OBJ)
	$(CC) $(CPPFLAGS) $(CFLAGS) -o $(TEST_TARGET) tests/test_engine.c $(ENGINE_OBJ)

test: all $(TEST_TARGET)
	./$(TEST_TARGET)

clean:
	rm -f $(TARGET) $(CHAR_TARGET) $(ENGINE_OBJ) $(TEST_TARGET)

.PHONY: all clean test
