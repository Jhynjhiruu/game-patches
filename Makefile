TARGET := oot

.PHONY: $(TARGET)

SRC_DIR := src
BUILD_DIR := build

INCLUDE_DIRS := include
INCLUDE_FLAGS := $(foreach i,$(INCLUDE_DIRS),-I$i)

CFLAGS := $(INCLUDE_FLAGS) -G0 -O2 -march=vr4300 -mtune=vr4300 -mabi=32

PREFIX := mips64-ultra-elf-

CC := $(PREFIX)gcc
AS := $(PREFIX)as
LD := $(PREFIX)ld
OBJCOPY := $(PREFIX)objcopy

$(shell mkdir -p $(BUILD_DIR))

DEPS := $(TARGET).o
REAL_DEPS := $(foreach i,$(DEPS),$(BUILD_DIR)/$i)

TEMP_TARGET := $(BUILD_DIR)/$(TARGET)

$(TARGET): $(TEMP_TARGET)
	$(OBJCOPY) -O binary -j .hdr -j .data $< $(shell $(OBJCOPY) -O binary $< -j .crc >(xxd -ps -l 8 -u)).patch

$(TEMP_TARGET): $(REAL_DEPS) | patch.ld
	$(LD) -o $@ $^ -T $|

$(BUILD_DIR)/%.o: $(BUILD_DIR)/%.i
	$(AS) -o $@ $<

$(BUILD_DIR)/%.i: $(BUILD_DIR)/%.S
	cp $< $@
	sed -i 's/\bjal\b/bal/g' $@
	sed -i -i 's/\bj\b/b/g' $@

$(BUILD_DIR)/%.S: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -o $@ -S $<

.PHONY: clean
clean:
	rm -rf $(BUILD_DIR)
	rm -rf *.patch
