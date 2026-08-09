# SPDX-License-Identifier: CC0-1.0
#
# SPDX-FileContributor: Antonio Niño Díaz, 2023-2026

.SUFFIXES:
.SECONDARY:
.SECONDEXPANSION:
.DEFAULT_GOAL := all

BLOCKSDS ?= /opt/blocksds/core

# User config
# ===========

NAME          := persona-3-dual
GAME_TITLE    := Persona 3 Dual
GAME_SUBTITLE := Memento Mori.
GAME_AUTHOR   := Atlus, The P3D Project
GAME_ICON     := icon.bmp

# A compile_commands.json file is created if this is set to 1
COMPDB := 1

# Source code paths
# -----------------

SOURCEDIRS  := source
INCLUDEDIRS := source \
               libs/aegis_engine/include \
               libs/aegis_engine/libs/etl/include \
               libs/aegis_engine/libs/fpm/include
GFXDIRS     :=
BINDIRS     :=
AUDIODIRS   := assets/sfx
NITROFSDIR  :=

# Libraries
# ---------

LIBS    := -lmm9 -lnds9
LIBDIRS := $(BLOCKSDS)/libs/maxmod

#---------------------------------------------------------------------------------
# Python tool configuration
#---------------------------------------------------------------------------------
TOOLS_DIR       := $(CURDIR)/tools
VENV_PYTHON     := $(HOME)/.venv/bin/python3
ASSETS_DIR      := $(CURDIR)/assets

ASSETS_DIALOGUE     := $(ASSETS_DIR)/dialogue
ASSETS_MUSIC        := $(ASSETS_DIR)/music
ASSETS_VIDEO        := $(ASSETS_DIR)/video
ASSETS_ENVIRONMENTS := $(ASSETS_DIR)/environments
ASSETS_MODELS       := $(ASSETS_DIR)/models
ASSETS_MAPS         := $(ASSETS_DIR)/maps

DATA_MUSIC := $(CURDIR)/data/music
DATA_VIDEO := $(CURDIR)/data/video

#---------------------------------------------------------------------------------
# Collect source files
#---------------------------------------------------------------------------------
DLG_FILES        := $(wildcard $(ASSETS_DIALOGUE)/*.dlg)
MP3_FILES        := $(shell find $(ASSETS_MUSIC) -type f -name '*.mp3' 2>/dev/null)
MP4_FILES        := $(wildcard $(ASSETS_VIDEO)/*.mp4)
ENV_OBJ_FILES    := $(wildcard $(ASSETS_ENVIRONMENTS)/*/*.obj)
JMAP_FILES       := $(wildcard $(ASSETS_MAPS)/*.jmap)

MODEL_JSON_FILES := $(wildcard $(ASSETS_MODELS)/*/*.json)

# Recursively find all PNG files in graphics and models.
FAT_PNG_FILES   := $(shell find $(CURDIR)/assets/graphics $(CURDIR)/assets/environments $(CURDIR)/assets/models -type f -name '*.png' 2>/dev/null)
FONT_PNG_FILES   := $(shell find $(CURDIR)/assets/fonts -type f -name '*.png' 2>/dev/null)
FONT_FNT_FILES   := $(shell find $(CURDIR)/assets/fonts -type f -name '*.fnt' 2>/dev/null)

#---------------------------------------------------------------------------------
# Derive output paths
#---------------------------------------------------------------------------------
DIALOGUE_OUT    := $(DLG_FILES:$(ASSETS_DIALOGUE)/%.dlg=$(CURDIR)/source/dialogue/%_dialogue.cpp)
MUSIC_OUT       := $(patsubst $(ASSETS_MUSIC)/%.mp3,$(DATA_MUSIC)/%.pcm,$(MP3_FILES))
VIDEO_OUT       := $(MP4_FILES:$(ASSETS_VIDEO)/%.mp4=$(DATA_VIDEO)/%.vid)
JMAP_OUT        := $(JMAP_FILES:$(ASSETS_MAPS)/%.jmap=$(CURDIR)/source/maps/%.h)

MODEL_OUT       := $(foreach file,$(MODEL_JSON_FILES),$(CURDIR)/source/models/$(notdir $(file:.json=.h)))

# Environments are now built entirely into .bin files by obj2environment.py
# and integrated into source/data/environmentDb.cpp, no .h headers needed
# Map obj files to sentinel files in data/environments/<name>/.sentinel
ENVIRONMENT_OUT := $(foreach file,$(ENV_OBJ_FILES),$(CURDIR)/data/environments/$(notdir $(patsubst %/,%,$(dir $(file))))/.sentinel)

.PHONY: assets dialogue music video environments jmaps models graphics font_bitmap sdcard help

assets: dialogue music video environments jmaps models graphics font_bitmap

# Dialogue
#---------------------------------------------------------------------------------
$(CURDIR)/source/dialogue/%_dialogue.cpp: $(ASSETS_DIALOGUE)/%.dlg $$(wildcard $(ASSETS_DIALOGUE)/$$*.build.json)
	@echo "  DLG   $(notdir $<)"
	@mkdir -p $(CURDIR)/source/dialogue
	@cd $(CURDIR)/source/dialogue && $(VENV_PYTHON) $(TOOLS_DIR)/build_asset.py "$<" "$*"
dialogue: $(DIALOGUE_OUT)

# Music
#---------------------------------------------------------------------------------
$(DATA_MUSIC)/%.pcm: $(ASSETS_MUSIC)/%.mp3
	@echo "  PCM   $(notdir $<)"
	@mkdir -p $(dir $@)
	@ffmpeg -i "$<" -f s16le -ar 32000 -ac 2 "$@" -y -loglevel error
music: $(MUSIC_OUT)

# Video
#---------------------------------------------------------------------------------
$(DATA_VIDEO)/%.vid: $(ASSETS_VIDEO)/%.mp4 $$(wildcard $(ASSETS_VIDEO)/$$*.build.json)
	@echo "  VID   $(notdir $<)"
	@mkdir -p $(dir $@)
	@$(VENV_PYTHON) $(TOOLS_DIR)/build_asset.py "$<" "$(basename $@)"
video: $(VIDEO_OUT)

#---------------------------------------------------------------------------------
# ENVIRONMENTS: Sentinel file to track build completion
# No .h files are produced—everything is in .bin + environmentDb.cpp
#---------------------------------------------------------------------------------
$(CURDIR)/data/environments/%/.sentinel: $(ASSETS_ENVIRONMENTS)/%/$$*.obj \
		$$(wildcard $(ASSETS_ENVIRONMENTS)/%/*.png) \
		$$(wildcard $(ASSETS_ENVIRONMENTS)/%/*.mtl) \
		$$(wildcard $(ASSETS_ENVIRONMENTS)/%/$$*.build.json) \
		$$(wildcard $(ASSETS_ENVIRONMENTS)/$$*.build.json)
	@echo "  ENV   $*"
	@mkdir -p $(dir $@) $(CURDIR)/data/environments/$*
	@$(VENV_PYTHON) $(TOOLS_DIR)/build_asset.py "$<" "$(CURDIR)/data/environments/$*"
	@touch $@
environments: $(ENVIRONMENT_OUT)

#---------------------------------------------------------------------------------
# MODELS: Appended /$* to force output into a specific subdirectory
#---------------------------------------------------------------------------------
$(CURDIR)/source/models/%.h: $(ASSETS_MODELS)/%/$$*.json \
		$$(wildcard $(ASSETS_MODELS)/%/$$*.build.json) \
		$$(wildcard $(ASSETS_MODELS)/$$*.build.json)
	@echo "  MODEL $*"
	@mkdir -p $(dir $@) $(CURDIR)/data/models/$*
	@$(VENV_PYTHON) $(TOOLS_DIR)/build_asset.py "$<" "$(CURDIR)/data/models/$*/$*.bin"
	@mv $(CURDIR)/data/models/$*/$*.h $@
	@touch $@
models: $(MODEL_OUT)

# Jmaps
#---------------------------------------------------------------------------------
$(CURDIR)/source/maps/%.h: $(ASSETS_MAPS)/%.jmap
	@echo "  JMAP  $(notdir $<)"
	@mkdir -p $(dir $@)
	@$(VENV_PYTHON) $(TOOLS_DIR)/build_asset.py "$<" "$@"
jmaps: $(JMAP_OUT)

#---------------------------------------------------------------------------------
# ALL GRAPHICS (Dynamic explicit rules using GNU Make Macros)
#---------------------------------------------------------------------------------
FAT_GRAPHICS_OUT := $(foreach file,$(FAT_PNG_FILES),$(patsubst $(ASSETS_DIR)/%.png,$(CURDIR)/data/%/$(notdir $(file:.png=.img.bin)),$(file)))

# Define a macro that acts as a blueprint for our build rule
define GRIT_RULE

$(patsubst $(ASSETS_DIR)/%.png,$(CURDIR)/data/%/$(notdir $(1:.png=.img.bin)),$(1)): $(1) $$(wildcard $$(1:.png=.grit))
	@echo "  GRIT  $$(notdir $$<)"
	@mkdir -p $$(dir $$@)
	$(V)$(BLOCKSDS)/tools/grit/grit "$$<" -ftb -fh! -o "$$(patsubst %.img.bin,%,$$@)"
endef

# Evaluate the macro for every single PNG found, dynamically scripting the rules into the Make environment
$(foreach file,$(FAT_PNG_FILES),$(eval $(call GRIT_RULE,$(file))))

graphics: $(FAT_GRAPHICS_OUT)

# FONTS
#---------------------------------------------------------------------------------

FONT_BM_OUT := $(foreach file,$(FONT_PNG_FILES),$(patsubst $(CURDIR)/assets/fonts/%.png,$(CURDIR)/data/fonts/%.img.bin,$(file)))
FONT_FNT_OUT := $(foreach file,$(FONT_FNT_FILES),$(patsubst $(CURDIR)/assets/fonts/%.fnt,$(CURDIR)/data/fonts/%.fnt,$(file)))

# Define a macro that acts as a blueprint for our build rule
define GRIT_RULE
$(patsubst $(CURDIR)/assets/fonts/%.png,$(CURDIR)/data/fonts/%.img.bin,$(1)): $(1) $$(wildcard $$(1:.png=.grit))
	@echo "  GRIT  $$(notdir $$<)"
	@mkdir -p $$(dir $$@)
	$(V)$(BLOCKSDS)/tools/grit/grit "$$<" -ftb -gb -gB8 -fh! -o "$$(patsubst %.img.bin,%,$$@)"
endef

define COPY_FONT_RULE
$(patsubst $(CURDIR)/assets/fonts/%.fnt,$(CURDIR)/data/fonts/%.fnt,$(1)): $(1)
	@echo "  COPY  $$(notdir $$<)"
	@mkdir -p $$(dir $$@)
	@cp "$$<" "$$@"
endef

# Evaluate the macro for every single PNG found, dynamically scripting the rules into the Make environment
$(foreach file,$(FONT_PNG_FILES),$(eval $(call GRIT_RULE,$(file))))
$(foreach file,$(FONT_FNT_FILES),$(eval $(call COPY_FONT_RULE,$(file))))

font_bitmap: $(FONT_BM_OUT) $(FONT_FNT_OUT)

#---------------------------------------------------------------------------------
# Second pass: after creating all the assets
# We use the official BlocksDS template, which already has everything configured
#---------------------------------------------------------------------------------
ifndef ASSETS_DONE

.PHONY: all dump clean

all dump: assets
	@$(MAKE) --no-print-directory ASSETS_DONE=1 $@

clean:
	@$(MAKE) --no-print-directory ASSETS_DONE=1 $@
else

# Debug flags
ifeq ($(DEBUG),1)
CFLAGS   := -O0 -ggdb
CXXFLAGS := -O0 -ggdb -Wno-psabi -fno-rtti -fno-exceptions
endif

# Default makefile includes arm7_maxmod as ARM7 elf
# It compiles with some default flags
include $(BLOCKSDS)/sys/default_makefiles/rom_arm9/Makefile

#---------------------------------------------------------------------------------
# Generate a FAT32 SD Card image
#---------------------------------------------------------------------------------
all: sdcard.img

DATA_FILES := $(shell find $(CURDIR)/data -type f)

sdcard.img: $(NAME).nds $(DATA_FILES)
ifeq ($(SKIPSD), 1)
	@echo "Skipping sdcard.img generation..."
else
	@echo "Generating sdcard.img (2GB)..."
	@$(VENV_PYTHON) -c "with open('sdcard.img', 'wb') as f: f.truncate(512 * 1024 * 1024 * 4)"
	@mformat -i sdcard.img -v P3D_SD -F ::
	@mcopy -i sdcard.img $(NAME).nds ::/
	@mcopy -s -i sdcard.img $(CURDIR)/data ::/
	@echo "Successfully built sdcard.img"
endif

.PHONY: clean-assets
clean: clean-assets

clean-assets:
	@echo "  CLEAN   assets"
	$(V)$(RM) $(MUSIC_OUT) $(VIDEO_OUT) $(JMAP_OUT) $(MODEL_OUT) $(DIALOGUE_OUT) \
	          $(CURDIR)/source/dialogue/*_dialogue.h
	$(V)$(RM) $(CURDIR)/data/models/* $(CURDIR)/data/graphics/* $(CURDIR)/data/fonts/* $(CURDIR)/data/environments/*
	$(V)$(RM) sdcard.img sdcard.img.idx

endif
