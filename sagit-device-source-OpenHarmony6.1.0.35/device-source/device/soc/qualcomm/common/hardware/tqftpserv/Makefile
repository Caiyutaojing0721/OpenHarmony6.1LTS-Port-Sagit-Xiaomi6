OHOS_ROOT = /home/AlgoIdeas/OpenHarmony
CC = $(OHOS_ROOT)/prebuilts/clang/ohos/linux-x86_64/llvm/bin/clang --sysroot=$(OHOS_ROOT)/out/oneplus_fajita/obj/third_party/musl
CXX = $(OHOS_ROOT)/prebuilts/clang/ohos/linux-x86_64/llvm/bin/clang++ --sysroot=$(OHOS_ROOT)/out/oneplus_fajita/obj/third_party/musl
AR = $(OHOS_ROOT)/prebuilts/clang/ohos/linux-x86_64/llvm/bin/llvm-ar
LD = $(OHOS_ROOT)/prebuilts/clang/ohos/linux-x86_64/llvm/bin/llvm-link
ARFLAG = -rcs

CFLAGS := --target=aarch64-linux-ohosmusl \
	--sysroot=$(OHOS_ROOT)/out/oneplus_fajita/obj/third_party/musl

TQFTPSERV := tqftpserv

CFLAGS += -Wall -g -O2
CFLAGS += -I/homec/minjie.yu/openharmony/sdm845/modem/qrtr-1.0/lib
LDFLAGS += -L/homec/minjie.yu/openharmony/sdm845/modem/qrtr-1.0 -lqrtr
prefix ?= /usr/local

bindir := $(prefix)/bin
servicedir := $(prefix)/lib/systemd/system

SRCS := tqftpserv.c translate.c

OBJS := $(SRCS:.c=.o)

$(TQFTPSERV): $(OBJS)
	$(CC) -o $@ $^ $(LDFLAGS) $(CFLAGS)

tqftpserv.service: tqftpserv.service.in
	@sed 's+TQFTPSERV_PATH+$(bindir)+g' $< > $@

install: $(TQFTPSERV) tqftpserv.service
	@install -D -m 755 $(TQFTPSERV) $(DESTDIR)$(bindir)/$(TQFTPSERV)
	@install -D -m 644 tqftpserv.service $(DESTDIR)$(servicedir)/tqftpserv.service

clean:
	rm -f $(TQFTPSERV) $(OBJS) tqftpserv.service
