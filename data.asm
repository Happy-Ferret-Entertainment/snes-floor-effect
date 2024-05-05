.include "hdr.asm"

.section ".rodata1" superfree

terrainPic: .incbin "res/home/terrain.pic" 
terrainPic_end:

terrainMap: .incbin "res/home/terrain.map" 
terrainMap_end:

terrainPalette: .incbin "res/home/terrain.pal" 
terrainPalette_end:

oam1Palette: .incbin "res/sprites/oam1.pal" 
oam1Palette_end:

oam1Pic: .incbin "res/sprites/oam1.pic" 
oam1Pic_end:

.ends
