CC=gcc
TARGET=zeroone
INSTALLED_TARGET=$(HOME)/bin/$(TARGET)

all: $(TARGET)


install: $(TARGET)
	cp $(TARGET) $(INSTALLED_TARGET)

$(TARGET): $(TARGET).o
	$(CC) -o $(TARGET) $(TARGET).o -lX11

clean:
	rm -rf $(TARGET) *.o
