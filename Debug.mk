CFLAGS   += -O0 -g
CXXFLAGS += -O0 -g
LDFLAGS  += -L$(CURDIR)

$(TARGET): libnrgraph.a libnraudio.a $(OBJECTS)
	$(CC) $(OBJECTS) $(LDFLAGS) -o $@

libnraudio.a:
	$(MAKE) -C $(MODULES_PATH)/nr-audio-lib depends Debug
	@cp $(MODULES_PATH)/nr-audio-lib/Debug/libnraudio.a .

libnrgraph.a:
	$(MAKE) -C $(MODULES_PATH)/nr-graph-lib depends Debug
	@cp $(MODULES_PATH)/nr-graph-lib/Debug/libnrgraph.a .

include $(DEPS)
