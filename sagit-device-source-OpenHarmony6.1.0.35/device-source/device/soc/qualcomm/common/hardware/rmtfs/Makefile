OHOS_ROOT = /home/AlgoIdeas/OpenHarmony
CC = $(OHOS_ROOT)/prebuilts/clang/ohos/linux-x86_64/llvm/bin/clang --sysroot=$(OHOS_ROOT)/out/oneplus_fajita/obj/third_party/musl
CXX = $(OHOS_ROOT)/prebuilts/clang/ohos/linux-x86_64/llvm/bin/clang++ --sysroot=$(OHOS_ROOT)/out/oneplus_fajita/obj/third_party/musl
AR = $(OHOS_ROOT)/prebuilts/clang/ohos/linux-x86_64/llvm/bin/llvm-ar
LD = $(OHOS_ROOT)/prebuilts/clang/ohos/linux-x86_64/llvm/bin/llvm-link
ARFLAG = -rcs

CFLAGS := --target=aarch64-linux-ohosmusl \
	--sysroot=$(OHOS_ROOT)/out/oneplus_fajita/obj/third_party/musl

OUT := rmtfs

CFLAGS += -Wall -g -O2 -I$(OHOS_ROOT)/third_party/eudev/src/libudev
CFLAGS += -I/homec/minjie.yu/openharmony/sdm845/modem/qrtr-1.0/lib
LDFLAGS += -L/homec/minjie.yu/openharmony/sdm845/modem/qrtr-1.0 -lqrtr
LDFLAGS += -L$(OHOS_ROOT)/out/oneplus_fajita/packages/phone/system/lib64 -ludev.z
LDFLAGS += -lpthread
prefix = /usr/local
bindir := $(prefix)/bin
servicedir := $(prefix)/lib/systemd/system

SRCS := qmi_rmtfs.c rmtfs.c rproc.c sharedmem.c storage.c util.c
OBJS := $(SRCS:.c=.o)

$(OUT): $(OBJS)
	$(CC) -o $@ $^ $(LDFLAGS) $(CFLAGS) 

%.c: %.qmi
	qmic -k < $<

rmtfs.service: rmtfs.service.in
	@sed 's+RMTFS_PATH+$(bindir)+g' $< > $@

install: $(OUT) rmtfs.service
	@install -D -m 755 $(OUT) $(DESTDIR)$(prefix)/bin/$(OUT)
	@install -D -m 644 rmtfs.service $(DESTDIR)$(servicedir)/rmtfs.service

clean:
	rm -f $(OUT) $(OBJS) rmtfs.service

