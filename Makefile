TARGET := shaderdemo
SOURCE := demo.c

.PHONY: all clean run

$(TARGET): $(SOURCE)
	$(CC) -Wall -O3 $(SOURCE) -lraylib -lm -o $(TARGET)

all: $(TARGET)

clean:
	rm -f $(TARGET)

run: $(TARGET)
	./$(TARGET)
