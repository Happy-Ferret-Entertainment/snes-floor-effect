#include <snes.h>
#include "level.h"
#include "utils.h"

// ROM

extern char terrainPic, terrainPic_end;
extern char terrainPalette, terrainPalette_end;
extern char terrainMap, terrainMap_end;

extern char oam1Palette, oam1Palette_end;
extern char oam1Pic, oam1Pic_end;

#define oamAddressOrigin 0x6000

/*!\brief Initialize the sprite engine.
    \param oamSize the OAM Size.
*/
void initSpriteEngine(u8 oamSize) {
    oamInit();
    oamInitGfxAttr(oamAddressOrigin, oamSize);
}

/*!\brief Initialize the Level screen.
*/
void initLevel() {
    bgSetMapPtr(BG0, 0, SC_64x32);

    bgInitTileSet(BG0,
        &terrainPic,
        &terrainPalette,
        PAL0,
        (&terrainPic_end - &terrainPic),
        16*2*4,
        BG_16COLORS,
        0x2000);

    WaitForVBlank();

    dmaCopyVram(&terrainMap, 0, 64*32*2);

    setMode(BG_MODE1, BG3_MODE1_PRIORITY_HIGH);
    bgSetEnable(BG0);
    bgSetDisable(BG1);
    bgSetDisable(BG2);
    bgSetDisable(BG3);

    // Workaround for SNES
    bgSetScroll(BG0, 128, -1);

    initSpriteEngine(OBJ_SIZE8_L16);
    
    // Warning: sprites using palettes 0-3 will reject color math
    // Please use palettes 4-7 for color math
    
    oamSet(0, 128, 0 + 160, 2, 0, 0, 0, PAL4);
    oamSetEx(0, OBJ_LARGE, OBJ_SHOW);
    
    oamSet(4, 128, 8 + 160, 2, 0, 0, 2, PAL4);
    oamSetEx(4, OBJ_LARGE, OBJ_SHOW);

    dmaCopyCGram(&oam1Palette, 128 + 64, 16*2);
    dmaCopyVram(&oam1Pic, oamAddressOrigin, 1024);
    WaitForVBlank();

    setFadeEffect(FADE_IN);
	WaitForVBlank();
}

/*!\brief Update the Level screen.
*/
void updateLevel() {
}

/*!\brief Clear the Level screen.
*/
void clearLevel() {
    // Fade out so light to black
	setFadeEffect(FADE_OUT);
	WaitForVBlank();

    oamClear(0, 0);
    dmaClearVram();

    bgSetDisable(BG0);
    bgSetDisable(BG1);
    bgSetDisable(BG2);
    bgSetDisable(BG3);
}
