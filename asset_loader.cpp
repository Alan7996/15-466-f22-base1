#include "asset_loader.hpp"

#include "PPU466.hpp"
#include "data_path.hpp"
#include "read_write_chunk.hpp"

#include <iostream>
#include <fstream>
#include <glm/glm.hpp>

void load_asset(PPU466 &ppu) {

    std::string asset_path = data_path("asset.txt");

    std::ifstream ifs;
    ifs.open(asset_path, std::ios::binary);

    // uint8_t palette_index = 0;
    // uint8_t tile_index = 0;
    // uint8_t sprite_index = 0;

    std::vector< glm::u8vec4 > palette_colors;
    std::vector< uint8_t > tile_bits;

    read_chunk(ifs, "pale", &palette_colors);
    read_chunk(ifs, "tile", &tile_bits);

}