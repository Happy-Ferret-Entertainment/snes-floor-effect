ifeq ($(strip $(PVSNESLIB_HOME)),)
$(error "Please create an environment variable PVSNESLIB_HOME with path to its folder and restart application. (you can do it on windows with <setx PVSNESLIB_HOME "/c/snesdev">)")
endif

# include ./snes_rules
include ${PVSNESLIB_HOME}/devkitsnes/snes_rules

.PHONY: all

#---------------------------------------------------------------------------------
# ROMNAME is used in snes_rules file
export ROMNAME := flooreffect

TARGET := FLOOREFFECT
GTITLE := -ht"$(TARGET)"

all: level sprites $(ROMNAME).sfc
	@echo MAKE NTSC VERSION
	$(SNTOOLS) -hi! $(GTITLE) $(ROMNAME).sfc
	cp $(ROMNAME).sfc $(ROMNAME)_ntsc.sfc
	cp $(ROMNAME).sym $(ROMNAME)_ntsc.sym
	@echo MAKE PAL VERSION
	$(SNTOOLS) -hi! -hS! $(GTITLE) -hc03 $(ROMNAME).sfc
	mv $(ROMNAME).sfc $(ROMNAME)_pal.sfc
	mv $(ROMNAME).sym $(ROMNAME)_pal.sym

cleanRom:
	@echo clean rom
	@rm -f $(ROMNAME).sfc $(ROMNAME).sym $(ROMNAME)_ntsc.sfc $(ROMNAME)_pal.sfc

cleanGfxFloor:
	@echo clean floor graphics data
	@rm -f res/home/*.pic res/home/*.map res/home/*.pal
	@rm -f res/sprites/*.pic res/sprites/*.pal

clean: cleanBuildRes cleanRom cleanGfx cleanGfxFloor cleanAudio

# Terrain
terrain.pic: res/home/terrain.bmp
	@echo convert bitmap ... $(notdir $@)
	$(GFXCONV) -pr -pc16 -n -gs8 -pe0 -fbmp -m $<

# OAM Sprites
oam1.pic: res/sprites/oam1.bmp
	@echo convert bitmap ... $(notdir $@)
	$(GFXCONV) -pr -pc16 -n -gs8 -pe0 -fbmp $<

level: terrain.pic

sprites: oam1.pic
