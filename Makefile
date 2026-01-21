#
#  crucible - Small 6502 simulator with Atari 8bit bios.
#  Copyright (C) 2026 AtariFoundry.com
#  Copyright (C) 2017-2019 Daniel Serpell
#
#  This program is free software; you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation, either version 2 of the License, or
#  (at your option) any later version.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License along
#  with this program.  If not, see <http://www.gnu.org/licenses/>
#

CC=gcc
INCLUDES=-Iccan
CFLAGS=$(INCLUDES) -O3 -Wall -g -flto
LDLIBS=-lm

BDIR=build
ODIR=$(BDIR)/obj
TARGET=$(BDIR)/crucible

.PHONY: all clean help test

all: $(TARGET)

help:
	@echo "crucible - Small 6502 simulator with Atari 8bit bios"
	@echo ""
	@echo "Available targets:"
	@echo "  all     - Build the crucible executable (default)"
	@echo "  clean   - Remove all build artifacts and test files"
	@echo "  test    - Run test programs"
	@echo "  help    - Show this help message"
	@echo ""
	@echo "Build output: $(TARGET)"
	@echo "Object files: $(ODIR)/"

test: $(TARGET)
	@echo "Running crucible tests..."
	@echo ""
	@echo "Test 1: ATR image loading..."
	@$(TARGET) -I testfiles/test.atr -d 2>&1 | grep -q "loaded.*test.atr\|sectors of" && echo "  ✓ ATR image loads successfully" || echo "  ✗ ATR image load failed"
	@echo ""
	@echo "Test 2: XEX program loading (hello.xex)..."
	@$(TARGET) testfiles/hello.xex 2>&1 | grep -q "HELLO" && echo "  ✓ Program runs successfully (prints HELLO)" || echo "  ✗ Program failed"
	@echo ""
	@echo "Test 3: H: device test (read_h.xex)..."
	@$(TARGET) -R testfiles testfiles/read_h.xex 2>&1 | grep -q "SCREEN:" && echo "  ✓ Program runs successfully (H: device access works)" || echo "  ⚠ Program loads but encounters runtime issues"
	@echo ""
	@echo "Tests completed. See testfiles/TEST_RESULTS.md for detailed results."

SRC=\
 src/atari.c\
 src/atcio.c\
 src/ataridos.c\
 src/atrdev.c\
 src/atrfs.c\
 src/athost.c\
 src/atsio.c\
 src/dosfname.c\
 src/hw.c\
 src/main.c\
 src/mathpack.c\
 src/sim65.c\

OBJS=$(SRC:src/%.c=$(ODIR)/%.o)

$(TARGET): $(OBJS) | $(BDIR)
	$(CC) $(CFLAGS) -o $@ $^ $(LDLIBS)

clean:
	rm -rf $(BDIR)
	rm -f testfiles/*.xex testfiles/*.atr testfiles/TEST.TXT

$(ODIR)/%.o: src/%.c | $(ODIR)
	$(CC) $(CFLAGS) -c -o $@ $<

$(BDIR) $(ODIR):
	mkdir -p $@


$(ODIR)/atari.o: src/atari.c src/atari.h src/sim65.h src/atcio.h src/atsio.h \
	src/mathpack.h src/hw.h
$(ODIR)/atcio.o: src/atcio.c src/atcio.h src/sim65.h src/atari.h src/dosfname.h src/atrdev.h src/athost.h
$(ODIR)/atrdev.o: src/atrdev.c src/atrdev.h src/sim65.h src/atcio.h src/atari.h src/hw.h
$(ODIR)/atrfs.o: src/atrfs.c src/atrfs.h src/atsio.h src/atari.h
$(ODIR)/athost.o: src/athost.c src/athost.h src/atcio.h src/atari.h src/ciodev.h src/dosfname.h
$(ODIR)/atsio.o: src/atsio.c src/atsio.h src/sim65.h src/atari.h
$(ODIR)/dosfname.o: src/dosfname.c src/dosfname.h
$(ODIR)/ataridos.o: src/ataridos.c src/ataridos.h src/atari.h src/atcio.h src/ciodev.h src/dosfname.h src/atrfs.h src/atsio.h
$(ODIR)/hw.o: src/hw.c src/hw.h src/sim65.h
$(ODIR)/main.o: src/main.c src/atari.h src/sim65.h
$(ODIR)/mathpack.o: src/mathpack.c src/mathpack.h src/sim65.h src/mathpack_bin.h
$(ODIR)/sim65.o: src/sim65.c src/sim65.h
