#include "asset_loader.hpp"

#include "PPU466.hpp"
#include "data_path.hpp"
#include "read_write_chunk.hpp"

#include <iostream>
#include <fstream>
#include <glm/glm.hpp>

void load_asset(PPU466 &ppu) {

    std::ifstream ifs(data_path("asset.bin"), std::ios::binary);

    // uint8_t palette_index = 0;
    // uint8_t tile_index = 0;
    // uint8_t sprite_index = 0;

    std::vector< PPU466::Palette > palettes;
    std::vector< PPU466::Tile > tiles;
    std::vector< PPU466::Sprite > sprites;
    std::vector< uint16_t > background;

    read_chunk(ifs, "pale", &palettes);
    read_chunk(ifs, "tile", &tiles);
    read_chunk(ifs, "spri", &sprites);
    read_chunk(ifs, "bgim", &background);

    for (uint32_t i = 0; i < palettes.size(); i++) {
		ppu.palette_table[i] = palettes[i];
	}
	for (uint32_t i = 0; i < tiles.size(); i++) {
		ppu.tile_table[i] = tiles[i];
	}
	for (uint32_t i = 0; i < sprites.size(); i++) {
		ppu.sprites[i] = sprites[i];
	}
    for (uint32_t i = 0; i < background.size(); i++) {
		ppu.background[i] = background[i];
	}
}