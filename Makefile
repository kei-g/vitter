CC = clang
CFLAGS = -D NDEBUG -Oz -Wall -Werror -flto=full -march=native
CONTROLFILES = control md5sums
DEBCONTENTS = debian-binary control.tar.xz data.tar.xz
DEBPKG = vitter_1.0.0_amd64.deb
LD = clang
LDFLAGS = -Wl,--lto-O3 -Wl,-s -dynamic -fuse-ld=lld
RM = rm -fr
TAR = tar
TARFLAGS = --group root --owner root -ch
TARGET_BINARY = usr/local/bin/vitter
TARGET_SOURCES = bitstream.c main.c vitter.c
TARGET_OBJECTS = $(TARGET_SOURCES:%.c=%.o)

all: $(DEBPKG) $(TARGET_BINARY)

clean:
	$(RM) $(TARGET_BINARY) $(TARGET_OBJECTS) *.vt usr/local/bin/*

control.tar.xz: $(CONTROLFILES)
	$(TAR) $(TARFLAGS) -f - $(CONTROLFILES) | xz -cez9 - > $@ && $(RM) md5sums

data.tar.xz: $(TARGET_BINARY)
	$(TAR) $(TARFLAGS) -f - $(shell for name in `ls`; do test -d $$name && echo $$name; done | xargs) | xz -cez9 - > $@

debian-binary:
	@echo 2.0 > $@

disasm: $(TARGET_BINARY)
	@llvm-objdump --disassemble-all $(TARGET_BINARY) | less

md5sums: $(TARGET_BINARY)
	find . -type f | grep -v -e '/\.' | while read name; do echo $$name | cut -d'/' -f2- | grep '/' > /dev/null && echo $$name; done | xargs md5sum > $@

nm: $(TARGET_BINARY)
	@nm -gP --all $^ | less

test: $(TARGET_BINARY)
	@$(TARGET_BINARY) --compress < vitter.c > vitter.vt
	@$(TARGET_BINARY) --decompress < vitter.vt | cmp - vitter.c
	@$(RM) vitter.vt

.c.o:
	$(CC) $(CFLAGS) -c $<

.PHONY: clean disasm nm test

$(DEBPKG): $(DEBCONTENTS)
	$(AR) cr $@ $(DEBCONTENTS)
	$(RM) $(DEBCONTENTS)

$(TARGET_BINARY): $(TARGET_OBJECTS)
	$(LD) $(LDFLAGS) -o $@ $(TARGET_OBJECTS)
	llvm-strip --strip-all $@
