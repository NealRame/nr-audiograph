export TARGET        = audiograph
export CC            = g++
export COMMON_FLAGS  = -Wall -Werror
export COMMON_FLAGS += -I$(CURDIR)/modules/nr-audio-lib/sources
export COMMON_FLAGS += -I$(CURDIR)/modules/nr-graph-lib/sources
export CFLAGS        = $(COMMON_FLAGS)
export CXXFLAGS      = $(COMMON_FLAGS) -std=c++11
export LDFLAGS       = -lnraudio
export LDFLAGS      += -lnrgraph
export LDFLAGS      += -l:libboost_system.a -l:libboost_filesystem.a -l:libboost_regex.a -lcairo -lmp3lame
export MODULES_PATH  = $(CURDIR)/modules
export SOURCES      := $(wildcard $(CURDIR)/sources/*.cpp)
export OBJECTS      := $(patsubst $(CURDIR)/sources/%.cpp,%.o,$(SOURCES))
export VPATH        := $(CURDIR)/sources
export DEPS         := $(CURDIR)/Makefile.depends

.PHONY: all clean Debug depends realclean Release tags

all: Debug Release

Debug Release:
	mkdir -p $@
	$(MAKE) --no-print-directory -C $@ -f ../$@.mk $(TARGET)

tags:
	$(CC) -M $(INCLUDE_DIRECTORIES) $(SOURCES) \
		| sed -e 's/[\\ ]/\n/g' \
		| sed -e '/^$$/d' -e '/\.o:[ \t]*$$/d' \
		| ctags -L - --c++-kinds=+p --fields=+iaS --extra=+q

depends: $(SOURCES)
	$(CC) $(CXXFLAGS) -MM $(SOURCES) > $(DEPS)

realclean: clean
	rm -fr log
	rm -fr tags
	rm -fr Debug
	rm -fr Release

clean:
	rm -fr *~
	rm -fr $(DEPS)
