#---------------------------------------------------------------------------------
.SUFFIXES:
#---------------------------------------------------------------------------------

.SECONDARY:
.SECONDEXPANSION:

ifeq ($(strip $(DEVKITARM)),)
$(error "Please set DEVKITARM in your environment. export DEVKITARM=<path to>devkitARM")
endif

include $(DEVKITARM)/ds_rules

# Default flags for release optimization
OPT := -O3
LTO_FLAG := -flto=auto

ifeq ($(DEBUG), 1)
    OPT := -O0 -g
    LTO_FLAG :=
endif

#---------------------------------------------------------------------------------
# TARGET is the name of the output
# BUILD is the directory where object files & intermediate files will be placed
# SOURCES is a list of directories containing source code
# INCLUDES is a list of directories containing extra header files
#---------------------------------------------------------------------------------
TARGET      :=  persona-3-dual
BUILD       :=  build
SOURCES 	:= source source/views source/controllers source/core source/data source/dialogue source/models source/environments source/components source/battleActions source/battleActions/enemies source/battleActions/party source/battleActions/skills source/battleActions/actions source/helpers source/battleActions/armours source/battleActions/personas source/battleActions/shoes source/battleActions/weapons source/components/ui source/components/menu
INCLUDES    :=  include source
SFX         :=  assets/sfx

GAME_TITLE     := Persona 3 Dual
GAME_SUBTITLE1 := Memento Mori.
GAME_SUBTITLE2 := Atlus/Sega, P3D Team
export GAME_ICON := $(CURDIR)/../icon.bmp

#---------------------------------------------------------------------------------
# Python tool configuration
#---------------------------------------------------------------------------------
TOOLS_DIR       := $(CURDIR)/tools
ifeq ($(OS),Windows_NT)
    VENV_PYTHON := $(HOME)/.venv/Scripts/python.exe
else
    VENV_PYTHON := $(HOME)/.venv/bin/python3
endif

ASSETS_DIALOGUE := $(CURDIR)/assets/dialogue
ASSETS_MUSIC    := $(CURDIR)/assets/music
ASSETS_VIDEO    := $(CURDIR)/assets/video
ASSETS_ENVIRONMENTS := $(CURDIR)/assets/environments
ASSETS_MODELS   := $(CURDIR)/assets/models
ASSETS_MAPS     := $(CURDIR)/assets/maps

DATA_MUSIC      := $(CURDIR)/data/music
DATA_VIDEO      := $(CURDIR)/data/video

#---------------------------------------------------------------------------------
# MMUTIL OS select
#---------------------------------------------------------------------------------

ifeq ($(OS),Windows_NT)
    MMUTIL := $(DEVKITPRO)/tools/bin/mmutil.exe
else
    MMUTIL := $(DEVKITPRO)/tools/bin/mmutil
endif

export MMUTIL
#---------------------------------------------------------------------------------
# Collect source files
#---------------------------------------------------------------------------------
DLG_FILES       := $(wildcard $(ASSETS_DIALOGUE)/*.dlg)
MP3_FILES       := $(shell find $(ASSETS_MUSIC) -type f -name '*.mp3' 2>/dev/null)
MP4_FILES       := $(wildcard $(ASSETS_VIDEO)/*.mp4)
ENV_OBJ_FILES   := $(wildcard $(ASSETS_ENVIRONMENTS)/*/*.obj)
JMAP_FILES      := $(wildcard $(ASSETS_MAPS)/*.jmap)

MODEL_JSON_FILES := $(wildcard $(ASSETS_MODELS)/*/*.json)

# Recursively find all PNG files in graphics, environments, and models.
FAT_PNG_FILES   := $(shell find $(CURDIR)/assets/graphics $(CURDIR)/assets/environments $(CURDIR)/assets/models -type f -name '*.png' 2>/dev/null)

#---------------------------------------------------------------------------------
# Derive output paths & dynamically add environment output dirs to SOURCES
#---------------------------------------------------------------------------------
DIALOGUE_OUT := $(DLG_FILES:$(ASSETS_DIALOGUE)/%.dlg=$(CURDIR)/source/dialogue/%_dialogue.cpp)
MUSIC_OUT    := $(patsubst $(ASSETS_MUSIC)/%.mp3,$(DATA_MUSIC)/%.pcm,$(MP3_FILES))
VIDEO_OUT    := $(MP4_FILES:$(ASSETS_VIDEO)/%.mp4=$(DATA_VIDEO)/%.vid)
JMAP_OUT     := $(JMAP_FILES:$(ASSETS_MAPS)/%.jmap=$(CURDIR)/source/maps/%.h)

MODEL_OUT    := $(foreach file,$(MODEL_JSON_FILES),$(CURDIR)/source/models/$(notdir $(file:.json=.h)))

# Keep track of environment directories so Make knows where to find the generated .s files later
ENV_OUT_DIRS    := $(foreach file,$(ENV_OBJ_FILES),source/environments/$(notdir $(patsubst %/,%,$(dir $(file)))))
ENVIRONMENT_OUT := $(foreach file,$(ENV_OBJ_FILES),$(CURDIR)/source/environments/$(notdir $(file:.obj=.h)))

#---------------------------------------------------------------------------------
# options for code generation
#---------------------------------------------------------------------------------
ARCH    :=  -march=armv5te -mtune=arm946e-s -mthumb

CFLAGS  := $(OPT) $(ARCH) $(INCLUDE) -DARM9 -Wall $(LTO_FLAG) -ffunction-sections -fdata-sections
CXXFLAGS    := $(CFLAGS) -fno-rtti -fno-exceptions

ASFLAGS :=  -g $(ARCH)

LDFLAGS =   -specs=ds_arm9.specs $(ARCH) $(LTO_FLAG) -Wl,--gc-sections -Wl,-Map,$(notdir $*.map)

#---------------------------------------------------------------------------------
# any extra libraries we wish to link with the project
#---------------------------------------------------------------------------------
LIBS    := -lmm9 -lfat -lnds9

#---------------------------------------------------------------------------------
# list of directories containing libraries, this must be the top level containing
# include and lib
#---------------------------------------------------------------------------------
LIBDIRS :=  $(LIBNDS) $(PORTLIBS)

#---------------------------------------------------------------------------------
ifneq ($(BUILD),$(notdir $(CURDIR)))
#---------------------------------------------------------------------------------
export TOPDIR   :=  $(CURDIR)
export OUTPUT   :=  $(CURDIR)/$(TARGET)

export VPATH    :=  $(foreach dir,$(SOURCES),$(CURDIR)/$(dir)) \
                    $(foreach dir,$(DATA),$(CURDIR)/$(dir)) \
                    $(foreach dir,$(ENV_OUT_DIRS),$(CURDIR)/$(dir))

export DEPSDIR  :=  $(CURDIR)/$(BUILD)

CFILES      :=  $(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.c)))
CPPFILES    :=  $(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.cpp)))
CPPFILES    +=  $(notdir $(DLG_FILES:$(ASSETS_DIALOGUE)/%.dlg=%_dialogue.cpp))
CPPFILES    :=  $(sort $(CPPFILES))
SFILES      :=  $(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.s)))
BINFILES    :=  soundbank.bin

export SFXFILES :=  $(foreach dir,$(notdir $(wildcard $(SFX)/*.*)),$(CURDIR)/$(SFX)/$(dir))

ifeq ($(strip $(CPPFILES)),)
    export LD := $(CC)
else
    export LD := $(CXX)
endif

export OFILES   :=  $(addsuffix .o,$(BINFILES)) \
                    $(CPPFILES:.cpp=.o) $(CFILES:.c=.o) $(SFILES:.s=.o)

export INCLUDE  :=  $(foreach dir,$(INCLUDES),-I$(CURDIR)/$(dir)) \
                    $(foreach dir,$(LIBDIRS),-I$(dir)/include) \
                    $(foreach dir,$(LIBDIRS),-I$(dir)/include) \
                    -I$(CURDIR)/$(BUILD)

export LIBPATHS :=  $(foreach dir,$(LIBDIRS),-L$(dir)/lib)

.PHONY: $(BUILD) clean assets dialogue music video environments jmaps models graphics sdcard help

#---------------------------------------------------------------------------------
$(BUILD):
	@$(MAKE) --no-print-directory assets
	@[ -d $@ ] || mkdir -p $@
	@$(MAKE) --no-print-directory -C $(BUILD) -f $(CURDIR)/Makefile
	@$(MAKE) --no-print-directory sdcard.img

help:
	@echo "  make            Build everything"
	@echo "  make assets     Run all asset converters"

assets: dirs dialogue music video environments jmaps models graphics

dirs:
	@mkdir -p $(CURDIR)/source/dialogue $(CURDIR)/source/maps $(CURDIR)/source/models $(CURDIR)/source/environments $(DATA_MUSIC) $(DATA_VIDEO) $(CURDIR)/data/models $(CURDIR)/data/environments $(CURDIR)/data/graphics

sdcard: sdcard.img
#---------------------------------------------------------------------------------
$(CURDIR)/source/dialogue/%_dialogue.cpp: $(ASSETS_DIALOGUE)/%.dlg $$(wildcard $(ASSETS_DIALOGUE)/$$*.build.json)
	@echo "  DLG   $(notdir $<)"
	@mkdir -p $(dir $@)
	@cd $(CURDIR)/source/dialogue && \
		$(VENV_PYTHON) $(TOOLS_DIR)/build_asset.py "$<" "$*"
dialogue: $(DIALOGUE_OUT)

#---------------------------------------------------------------------------------
$(DATA_MUSIC)/%.pcm: $(ASSETS_MUSIC)/%.mp3
	@echo "  PCM   $(notdir $<)"
	@mkdir -p $(dir $@)
	@ffmpeg -i "$<" -f s16le -ar 32000 -ac 2 "$@" -y -loglevel error
music: $(MUSIC_OUT)

#---------------------------------------------------------------------------------
$(DATA_VIDEO)/%.vid: $(ASSETS_VIDEO)/%.mp4 $$(wildcard $(ASSETS_VIDEO)/$$*.build.json)
	@echo "  VID   $(notdir $<)"
	@mkdir -p $(dir $@)
	@$(VENV_PYTHON) $(TOOLS_DIR)/build_asset.py "$<" "$(basename $@)"
video: $(VIDEO_OUT)

#---------------------------------------------------------------------------------
# ENVIRONMENTS: Appended /$* to force output into a specific subdirectory
#---------------------------------------------------------------------------------
$(CURDIR)/source/environments/%.h: $(ASSETS_ENVIRONMENTS)/%/$$*.obj \
		$$(wildcard $(ASSETS_ENVIRONMENTS)/%/*.png) \
		$$(wildcard $(ASSETS_ENVIRONMENTS)/%/*.mtl) \
		$$(wildcard $(ASSETS_ENVIRONMENTS)/%/$$*.build.json) \
		$$(wildcard $(ASSETS_ENVIRONMENTS)/$$*.build.json)
	@echo "  ENV   $*"
	@mkdir -p $(dir $@) $(CURDIR)/data/environments/$*
	@$(VENV_PYTHON) $(TOOLS_DIR)/build_asset.py "$<" "$(CURDIR)/data/environments/$*"
	@mv $(CURDIR)/data/environments/$*/$*.h $@
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

#---------------------------------------------------------------------------------
$(CURDIR)/source/maps/%.h: $(ASSETS_MAPS)/%.jmap
	@echo "  JMAP  $(notdir $<)"
	@mkdir -p $(dir $@)
	@$(VENV_PYTHON) $(TOOLS_DIR)/build_asset.py "$<" "$@"

jmaps: $(JMAP_OUT)

#---------------------------------------------------------------------------------
# ALL GRAPHICS (Dynamic explicit rules using GNU Make Macros)
#---------------------------------------------------------------------------------
# Generate the exact target paths.
FAT_GRAPHICS_OUT := $(foreach file,$(FAT_PNG_FILES),$(patsubst $(CURDIR)/assets/%.png,$(CURDIR)/data/%/$(notdir $(file:.png=.img.bin)),$(file)))

# Define a macro that acts as a blueprint for our build rule
define GRIT_RULE
$(patsubst $(CURDIR)/assets/%.png,$(CURDIR)/data/%/$(notdir $(1:.png=.img.bin)),$(1)): $(1) $$(wildcard $$(1:.png=.grit))
	@echo "  GRIT  $$(notdir $$<)"
	@mkdir -p $$(dir $$@)
	@grit "$$<" -ftb -fh! -o "$$(patsubst %.img.bin,%,$$@)"
endef

# Evaluate the macro for every single PNG found, dynamically scripting the rules into the Make environment
$(foreach file,$(FAT_PNG_FILES),$(eval $(call GRIT_RULE,$(file))))

graphics: $(FAT_GRAPHICS_OUT)

#---------------------------------------------------------------------------------
clean:
	@echo clean ...
	@rm -fr $(BUILD) $(TARGET).elf $(TARGET).nds $(TARGET).ds.gba
	@rm -f $(MUSIC_OUT) $(VIDEO_OUT) $(JMAP_OUT) $(MODEL_OUT) $(DIALOGUE_OUT) $(CURDIR)/source/dialogue/*_dialogue.h
	@rm -rf $(CURDIR)/source/environments/* $(CURDIR)/data/models/* $(CURDIR)/data/environments/* $(CURDIR)/data/graphics/*
	@rm -f sdcard.img sdcard.img.idx

#---------------------------------------------------------------------------------
else

DEPENDS :=  $(OFILES:.o=.d)

$(OUTPUT).nds   :   $(OUTPUT).elf
$(OUTPUT).elf   :   $(OFILES)

soundbank.bin soundbank.h : $(SFXFILES)
	@mmutil $^ -d -osoundbank.bin -hsoundbank.h

%.bin.o :   %.bin
	@echo $(notdir $<)
	@$(bin2o)

%.mp3.o :   %.mp3
	@echo $(notdir $<)
	@$(bin2o)

-include $(DEPENDS)
endif

#---------------------------------------------------------------------------------
# Generate a FAT32 SD Card image
#---------------------------------------------------------------------------------
ifneq ($(BUILD),$(notdir $(CURDIR)))
DATA_FILES := $(shell find $(CURDIR)/data -type f)
endif
sdcard.img: $(OUTPUT).nds $(DATA_FILES)
	@echo "Generating sdcard.img (2GB)..."
	@$(VENV_PYTHON) -c "with open('sdcard.img', 'wb') as f: f.truncate(512 * 1024 * 1024 * 4)"
	@mformat -i sdcard.img -v P3D_SD -F ::
	@mcopy -i sdcard.img $(OUTPUT).nds ::/
	@mcopy -s -i sdcard.img $(CURDIR)/data ::/
	@echo "Successfully built sdcard.img"
