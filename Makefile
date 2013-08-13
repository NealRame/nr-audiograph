TARGET        = audiograph
CC            = g++
COMMON_FLAGS  = -Wall -Werror
COMMON_FLAGS += -I$(CURDIR)/modules/nr-audio-lib/sources
COMMON_FLAGS += -I$(CURDIR)/modules/nr-graph-lib/sources
CFLAGS        = $(COMMON_FLAGS)
CXXFLAGS      = $(COMMON_FLAGS) -std=c++11
LDFLAGS       = -lnraudio
LDFLAGS      += -lnrgraph
LDFLAGS      += -l:libboost_system.a
LDFLAGS      += -l:libboost_filesystem.a
LDFLAGS      += -l:libboost_program_options.a
LDFLAGS      += -l:libboost_regex.a
LDFLAGS      += -lcairo
LDFLAGS      += -lmp3lame
MODULES_PATH  = $(CURDIR)/modules
SOURCES      := $(wildcard $(CURDIR)/sources/*.cpp)
OBJECTS      := $(patsubst $(CURDIR)/sources/%.cpp,%.o,$(SOURCES))
VPATH        := $(CURDIR)/sources
DEPS         := $(CURDIR)/Makefile.depends

export TARGET CC COMMON_FLAGS CFLAGS CXXFLAGS LDFLAGS MODULES_PATH SOURCES
export OBJECTS VPATH DEPS

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
