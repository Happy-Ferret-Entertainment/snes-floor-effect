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

// RAM

u16 scrollBackgrounds;
u8 loadingCounters[7];
u8 loadingCounterIndex;
u16 moveTmp;
unsigned short pad0;
unsigned short padDown0;
unsigned short padUp0;

/*!\brief Handle Player move input.
*/
void handlePlayerMoveInput() {
    scrollBackgrounds = 0;

    if (pad0 & KEY_RIGHT) {
        scrollBackgrounds = 1;

    } else if (pad0 & KEY_LEFT) {
        scrollBackgrounds = 1;
    }
}

/*!\brief Initialize time counters.
*/
void initTime() {
    loadingCounterIndex = 0;
    for(loadingCounterIndex = 0; loadingCounterIndex < 7; loadingCounterIndex++) {
        loadingCounters[loadingCounterIndex] = 0;
    }
}

/*!\brief Wait for a given time. To be used if multiple time counters are needed. The maximum value of index is 4.
	\param time the time to wait for.
	\param index the time counter index.
*/
u8 waitForTimeEx(u16 time, u16 index) {
    loadingCounters[index] += 1;
    if (loadingCounters[index] == time) {
        loadingCounters[index] = 0;
        return 1;
    }
    return 0;
}

/*!\brief To be used for automatic scrolling parallax.
	\param time the time to wait for.
	\param index the time counter index.
	\param offset the offset.
*/
u8 getParallaxOffset(u16 time, u16 index, u16 offset) {
    if (scrollBackgrounds == 1) {
        if (waitForTimeEx(time, index) == 1) {
            return 1 + offset;
        }
        return offset;
    }

    if (waitForTimeEx(time, index) == 1) {
        return 1;
    }
    return 0;
}

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
    scrollBackgrounds = 0;
    initTime();

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

    // Configure the Home Background Scrolling.
    // Init HDMA table
    // Note: the maximum value is 128
    HDMATable16[0] = 16; *(u16*) &(HDMATable16+1) = 0;
    HDMATable16[3] = 8; *(u16*) &(HDMATable16+4) = 0;
    HDMATable16[6] = 8; *(u16*) &(HDMATable16+7) = 0;
    HDMATable16[9] = 40; *(u16*) &(HDMATable16+10) = 0;
    HDMATable16[12] = 24; *(u16*) &(HDMATable16+13) = 0;
    HDMATable16[15] = 24; *(u16*) &(HDMATable16+16) = 0;
    HDMATable16[18] = 24; *(u16*) &(HDMATable16+19) = 0;
    HDMATable16[21] = 48; *(u16*) &(HDMATable16+22) = 0;
    HDMATable16[24] = 32; *(u16*) &(HDMATable16+25) = 0;
    HDMATable16[27] = 0x00; // end of hdma table

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

/*!\brief Scroll BG2 by reading moveTmp value.
*/
void scrollBg2() {
    if(scrollBackgrounds == 1) {
        *(u16*) &(HDMATable16+10) += getParallaxOffset(7, 3, moveTmp);
        *(u16*) &(HDMATable16+13) += getParallaxOffset(6, 4, moveTmp);
        *(u16*) &(HDMATable16+16) += getParallaxOffset(5, 5, moveTmp);

        *(u16*) &(HDMATable16+19) = 0;

        *(u16*) &(HDMATable16+22) += getParallaxOffset(1, 6, moveTmp);
    }
}

/*!\brief Update the Level screen.
*/
void updateLevel() {
    // 409 cycles for Input
    scanPads();
    pad0 = padsCurrent(0);
    padDown0 = padsDown(0);
    padUp0 = padsUp(0);

    handlePlayerMoveInput();

    moveTmp = 1;
    scrollBg2();
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

    HDMATable16[0]= 0; *(u16*) &(HDMATable16+1)=0;
    HDMATable16[3]= 0; *(u16*) &(HDMATable16+4)=0;
    HDMATable16[6]= 0; *(u16*) &(HDMATable16+7)=0;
    HDMATable16[9]= 0; *(u16*) &(HDMATable16+10)=0;
    HDMATable16[12]= 0; *(u16*) &(HDMATable16+13)=0;
    HDMATable16[15]= 0; *(u16*) &(HDMATable16+16)=0;
    HDMATable16[18]= 0; *(u16*) &(HDMATable16+19)=0;
    HDMATable16[21]= 0; *(u16*) &(HDMATable16+22)=0;
    HDMATable16[24]= 0; *(u16*) &(HDMATable16+25)=0;
    HDMATable16[27]=0x00; // end of hdma table
}
