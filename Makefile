Q2_CC ?= clang
Q2_LD ?= ld.lld
PYTHON ?= python3
Q2_FLAGS = --target=mipsel-linux-gnu -march=mips32r2 -mabi=32 -mfp64 -mnan=2008 -mabs=2008 \
	-mno-abicalls -fno-pic -G0 -ffreestanding -fno-builtin -fno-stack-protector \
	-fno-unwind-tables -fno-asynchronous-unwind-tables
Q2_CFLAGS ?= -Oz -Wall -Wextra
CORE = $(wildcard vendor/dinorunner/src/*.c)
SOURCES = src/app.c $(CORE)
INCLUDES = -Isrc -Ivendor/dinorunner/include -Ibuild
HEADERS = $(wildcard src/*.h vendor/dinorunner/include/*.h vendor/dinorunner/src/*.h)
OBJECTS = $(addprefix build/q2/,$(SOURCES:.c=.o) src/platform_q2.o)
CARD = dist/.dino-runner

all: dist/q2-dino-runner.zip

build/sprites.inc: assets/100-offline-sprite.png tools/pack-sprites.py
	mkdir -p $(@D)
	$(PYTHON) tools/pack-sprites.py $< > $@.tmp
	mv $@.tmp $@

build/q2/%.o: %.c $(HEADERS)
	mkdir -p $(@D)
	$(Q2_CC) $(Q2_FLAGS) $(Q2_CFLAGS) -std=c99 $(INCLUDES) -c $< -o $@

build/q2/src/app.o: build/sprites.inc

$(CARD)/q2-dino-runner: $(OBJECTS)
	mkdir -p $(@D)
	$(Q2_LD) -m elf32ltsmip -static -e __start -s -z noexecstack -o $@ $^

$(CARD)/q2app: q2app Makefile
	mkdir -p $(@D)
	tr -d '\r' < $< > $@

$(CARD)/LICENSE.txt: LICENSE assets/LICENSE-Chromium.txt
	mkdir -p $(@D)
	cat $^ > $@

dist/q2-dino-runner.zip: $(CARD)/q2-dino-runner $(CARD)/q2app $(CARD)/LICENSE.txt
	cd dist && rm -f q2-dino-runner.zip && zip -q q2-dino-runner.zip $(patsubst dist/%,%,$^)

build/check: tests/check.c $(SOURCES) $(HEADERS) build/sprites.inc
	$(CC) $(CFLAGS) -O2 -Wall -Wextra -std=c99 $(INCLUDES) -o $@ tests/check.c $(SOURCES)

check: build/check
	./build/check

clean:
	rm -rf build dist

.PHONY: all check clean
