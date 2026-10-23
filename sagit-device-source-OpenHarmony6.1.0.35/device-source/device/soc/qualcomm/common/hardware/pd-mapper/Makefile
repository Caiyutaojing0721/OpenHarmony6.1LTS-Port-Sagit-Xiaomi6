OHOS_ROOT = /home/AlgoIdeas/OpenHarmony
CC = $(OHOS_ROOT)/prebuilts/clang/ohos/linux-x86_64/llvm/bin/clang --sysroot=$(OHOS_ROOT)/out/oneplus_fajita/obj/third_party/musl
CXX = $(OHOS_ROOT)/prebuilts/clang/ohos/linux-x86_64/llvm/bin/clang++ --sysroot=$(OHOS_ROOT)/out/oneplus_fajita/obj/third_party/musl
AR = $(OHOS_ROOT)/prebuilts/clang/ohos/linux-x86_64/llvm/bin/llvm-ar
LD = $(OHOS_ROOT)/prebuilts/clang/ohos/linux-x86_64/llvm/bin/llvm-link
ARFLAG = -rcs

CFLAGS := --target=aarch64-linux-ohosmusl \
	--sysroot=$(OHOS_ROOT)/out/oneplus_fajita/obj/third_party/musl


PD_MAPPER := pd-mapper

CFLAGS += -Wall -g -O2
CFLAGS += -I/homec/minjie.yu/openharmony/sdm845/modem/qrtr-1.0/lib
LDFLAGS += -L/homec/minjie.yu/openharmony/sdm845/modem/qrtr-1.0 -lqrtr
prefix ?= /usr/local

bindir := $(prefix)/bin
servicedir := $(prefix)/lib/systemd/system

SRCS := pd-mapper.c \
        assoc.c \
        json.c \
	servreg_loc.c

OBJS := $(SRCS:.c=.o)

$(PD_MAPPER): $(OBJS)
	$(CC) -o $@ $^ $(LDFLAGS) $(CFLAGS)

pd-mapper.service: pd-mapper.service.in
	@sed 's+PD_MAPPER_PATH+$(bindir)+g' $< > $@

install: $(PD_MAPPER) pd-mapper.service
	@install -D -m 755 $(PD_MAPPER) $(DESTDIR)$(bindir)/$(PD_MAPPER)
	@install -D -m 644 pd-mapper.service $(DESTDIR)$(servicedir)/pd-mapper.service

clean:
	rm -f $(PD_MAPPER) $(OBJS) pd-mapper.service
