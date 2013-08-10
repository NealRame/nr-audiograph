CFLAGS   += -O3
CXXFLAGS += -O3

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(LFLAGS) -o $@ $(OBJECTS)

include $(DEPS)
