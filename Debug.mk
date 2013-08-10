CFLAGS   += -O0 -g
CXXFLAGS += -O0 -g

$(TARGET): $(OBJECTS)
	@echo $(OBJECTS)
	$(CC) $(LFLAGS) -o $@ $(OBJECTS)

include $(DEPS)
