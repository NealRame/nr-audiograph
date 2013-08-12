CFLAGS   += -O3
CXXFLAGS += -O3
LDFLAGS  += -L$(CURDIR)

$(TARGET): libnrgraph.a libnraudio.a $(OBJECTS)
	$(CC) $(OBJECTS) $(LDFLAGS) -o $@
	strip $(TARGET)

libnraudio.a:
	$(MAKE) -C $(MODULES_PATH)/nr-audio-lib depends Release
	@cp $(MODULES_PATH)/nr-audio-lib/Debug/libnraudio.a .

libnrgraph.a:
	$(MAKE) -C $(MODULES_PATH)/nr-graph-lib depends Release
	@cp $(MODULES_PATH)/nr-graph-lib/Debug/libnrgraph.a .

include $(DEPS)
