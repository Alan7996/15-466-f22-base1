#include "asset_generator.hpp"

#include "read_write_chunk.hpp"
#include "data_path.hpp"
#include "load_save_png.hpp"
#include "PPU466.hpp"

#include <iostream>
#include <fstream>
#include <algorithm>
#include <vector>
#include <glm/glm.hpp>

void generate_data() {
    
    std::vector< PPU466::Palette > palettes;
    std::vector< PPU466::Tile > tiles;
    std::vector< PPU466::Sprite > sprites;
    std::vector< uint16_t > background;

    glm::uvec2 palette_size = glm::uvec2(0, 0);
    std::vector< glm::u8vec4 > palette_data;
    std::vector< glm::u8vec4 > palette_added;

    std::array< uint8_t, 8 > bit0 = { 0, 0, 0, 0, 0, 0, 0, 0 };
	std::array< uint8_t, 8 > bit1 = { 0, 0, 0, 0, 0, 0, 0, 0 };

    // load_png call error catching and palette extraction code as suggested by David Lyons (dalyons)

    // Generate background (Chessboard)
    {// Background is consisted of two tiles alternating throughout the buffer, taking of indexes 0 and 1 of palette and tile.
        try {
            load_png(data_path("../assets/Chessboard_tile1.png"), &palette_size, &palette_data, UpperLeftOrigin);
        }
        catch (std::runtime_error& e) {
            std::cout << e.what();
        }
        assert(palette_size.x == 8 && palette_size.y == 8);

        for (uint32_t y = 0; y < palette_size.y; y++) {
            for (uint32_t x = 0; x < palette_size.x; x++) {
                glm::u8vec4 color = palette_data[y * palette_size.x + x];
                if (std::find(palette_added.begin(), palette_added.end(), color) == palette_added.end()) 
                    palette_added.push_back(color);

                if (palette_added[0] == color) {
                    bit1[y] |= 1 << x;
                } else if (palette_added[1] == color) {
                    bit1[y] |= 1 << x;
                    bit0[y] |= 1 << x;
                } else {
                    // shouldn't happen
                    std::cout << "The png has more than 2 colors!" << std::endl;
                }
            }
        }
        assert(palette_added.size() == 2); // should only contain white, gray, and black

        palettes.push_back({glm::u8vec4(),
                            glm::u8vec4(0, 0, 0, 255),
                            palette_added[0],
                            palette_added[1]});
        tiles.push_back({ bit0, bit1 });

        // reset intermediary storages
        palette_added.clear();
        bit0.fill(0);
        bit1.fill(0);

        try {
            load_png(data_path("../assets/Chessboard_tile2.png"), &palette_size, &palette_data, UpperLeftOrigin);
        }
        catch (std::runtime_error& e) {
            std::cout << e.what();
        }
        assert(palette_size.x == 8 && palette_size.y == 8);

        for (uint32_t y = 0; y < palette_size.y; y++) {
            for (uint32_t x = 0; x < palette_size.x; x++) {
                glm::u8vec4 color = palette_data[y * palette_size.x + x];
                if (std::find(palette_added.begin(), palette_added.end(), color) == palette_added.end()) 
                    palette_added.push_back(color);

                if (palette_added[0] == color) {
                    bit1[y] |= 1 << x;
                } else if (palette_added[1] == color) {
                    bit1[y] |= 1 << x;
                    bit0[y] |= 1 << x;
                } else {
                    // shouldn't happen
                    std::cout << "The png has more than 2 colors!" << std::endl;
                }
            }
        }
        assert(palette_added.size() == 2); // should only contain white, gray, and black

        palettes.push_back({glm::u8vec4(),
                            glm::u8vec4(0, 0, 0, 255),
                            palette_added[0],
                            palette_added[1]});
        tiles.push_back({ bit0, bit1 });

        // generate the background array
        for (uint32_t y = 0; y < PPU466::BackgroundHeight; y++) {
            for (uint32_t x = 0; x < PPU466::BackgroundWidth; x++) {
                if ((x % 2 + y % 2) % 2 == 0) {
                    background.push_back(0b0000000000000000);
                } else {
                    background.push_back(0b0000000100000001);
                }
            }
        }
        assert(background.size() == PPU466::BackgroundHeight * PPU466::BackgroundWidth);

        // reset intermediary storages
        palette_added.clear();
        bit0.fill(0);
        bit1.fill(0);
    }


    // Note that for some png assets, because we encounter 
    // the colored pixel first instead of transparent pixel first, the order
    // in which we write the bit arrays is flipped.

    {// Generate player (King)
        // King takes index 2~5 (U/R/D/L) of palette and tile, and index 0 of sprite, and is rendered in front of the background.
        try {
            load_png(data_path("../assets/King_up.png"), &palette_size, &palette_data, UpperLeftOrigin);
        }
        catch (std::runtime_error& e) {
            std::cout << e.what();
        }
        assert(palette_size.x == 8 && palette_size.y == 8);

        for (uint32_t y = 0; y < palette_size.y; y++) {
            for (uint32_t x = 0; x < palette_size.x; x++) {
                glm::u8vec4 color = palette_data[y * palette_size.x + x];
                if (std::find(palette_added.begin(), palette_added.end(), color) == palette_added.end()) 
                    palette_added.push_back(color);

                if (palette_added[0] == color) {
                    bit1[y] |= 1 << x;
                } else if (palette_added[1] == color) {
                    bit1[y] |= 1 << x;
                    bit0[y] |= 1 << x;
                } else {
                    // shouldn't happen
                    std::cout << "The png has more than 2 colors!" << std::endl;
                }
            }
        }
        assert(palette_added.size() == 2); // should only contain white and green

        palettes.push_back({glm::u8vec4(),
                            glm::u8vec4(0, 0, 0, 255),
                            palette_added[0],
                            palette_added[1]});
        tiles.push_back({ bit0, bit1 });
        sprites.push_back({ 128, 120, 2, 0b0000010 });

        // reset intermediary storages
        palette_added.clear();
        bit0.fill(0);
        bit1.fill(0);

        try {
            load_png(data_path("../assets/King_right.png"), &palette_size, &palette_data, UpperLeftOrigin);
        }
        catch (std::runtime_error& e) {
            std::cout << e.what();
        }
        assert(palette_size.x == 8 && palette_size.y == 8);

        for (uint32_t y = 0; y < palette_size.y; y++) {
            for (uint32_t x = 0; x < palette_size.x; x++) {
                glm::u8vec4 color = palette_data[y * palette_size.x + x];
                if (std::find(palette_added.begin(), palette_added.end(), color) == palette_added.end()) 
                    palette_added.push_back(color);

                if (palette_added[0] == color) {
                    bit1[y] |= 1 << x;
                } else if (palette_added[1] == color) {
                    bit1[y] |= 1 << x;
                    bit0[y] |= 1 << x;
                } else {
                    // shouldn't happen
                    std::cout << "The png has more than 2 colors!" << std::endl;
                }
            }
        }
        assert(palette_added.size() == 2); // should only contain white and green

        tiles.push_back({ bit0, bit1 });

        // reset intermediary storages
        palette_added.clear();
        bit0.fill(0);
        bit1.fill(0);

        try {
            load_png(data_path("../assets/King_down.png"), &palette_size, &palette_data, UpperLeftOrigin);
        }
        catch (std::runtime_error& e) {
            std::cout << e.what();
        }
        assert(palette_size.x == 8 && palette_size.y == 8);

        for (uint32_t y = 0; y < palette_size.y; y++) {
            for (uint32_t x = 0; x < palette_size.x; x++) {
                glm::u8vec4 color = palette_data[y * palette_size.x + x];
                if (std::find(palette_added.begin(), palette_added.end(), color) == palette_added.end()) 
                    palette_added.push_back(color);

                if (palette_added[0] == color) {
                    bit1[y] |= 1 << x;
                    bit0[y] |= 1 << x;
                } else if (palette_added[1] == color) {
                    bit1[y] |= 1 << x;
                } else {
                    // shouldn't happen
                    std::cout << "The png has more than 2 colors!" << std::endl;
                }
            }
        }
        assert(palette_added.size() == 2); // should only contain white and green

        tiles.push_back({ bit0, bit1 });

        // reset intermediary storages
        palette_added.clear();
        bit0.fill(0);
        bit1.fill(0);

        try {
            load_png(data_path("../assets/King_left.png"), &palette_size, &palette_data, UpperLeftOrigin);
        }
        catch (std::runtime_error& e) {
            std::cout << e.what();
        }
        assert(palette_size.x == 8 && palette_size.y == 8);

        for (uint32_t y = 0; y < palette_size.y; y++) {
            for (uint32_t x = 0; x < palette_size.x; x++) {
                glm::u8vec4 color = palette_data[y * palette_size.x + x];
                if (std::find(palette_added.begin(), palette_added.end(), color) == palette_added.end()) 
                    palette_added.push_back(color);

                if (palette_added[0] == color) {
                    bit1[y] |= 1 << x;
                    bit0[y] |= 1 << x;
                } else if (palette_added[1] == color) {
                    bit1[y] |= 1 << x;
                } else {
                    // shouldn't happen
                    std::cout << "The png has more than 2 colors!" << std::endl;
                }
            }
        }
        assert(palette_added.size() == 2); // should only contain white and green

        tiles.push_back({ bit0, bit1 });

        // reset intermediary storages
        palette_added.clear();
        bit0.fill(0);
        bit1.fill(0);
    }

    // Generate bullet (Pawn)
    {// Pawn takes index 6~9 (U/R/D/L) of palette and tile, and index 1 of sprite, and is rendered in front of the background.
        try {
            load_png(data_path("../assets/Pawn_up.png"), &palette_size, &palette_data, UpperLeftOrigin);
        }
        catch (std::runtime_error& e) {
            std::cout << e.what();
        }
        assert(palette_size.x == 8 && palette_size.y == 8);

        for (uint32_t y = 0; y < palette_size.y; y++) {
            for (uint32_t x = 0; x < palette_size.x; x++) {
                glm::u8vec4 color = palette_data[y * palette_size.x + x];
                if (std::find(palette_added.begin(), palette_added.end(), color) == palette_added.end()) 
                    palette_added.push_back(color);

                if (palette_added[0] == color) {
                    bit1[y] |= 1 << x;
                } else if (palette_added[1] == color) {
                    bit1[y] |= 1 << x;
                    bit0[y] |= 1 << x;
                } else {
                    // shouldn't happen
                    std::cout << "The png has more than 2 colors!" << std::endl;
                }
            }
        }
        assert(palette_added.size() == 2); // should only contain white and green

        palettes.push_back({glm::u8vec4(),
                            glm::u8vec4(0, 0, 0, 255),
                            palette_added[0],
                            palette_added[1]});
        tiles.push_back({ bit0, bit1 });
        for (uint32_t i = 0; i < 60; i++) {
            sprites.push_back({ 0, 0, 6, 0b0000011 });
        }

        // reset intermediary storages
        palette_added.clear();
        bit0.fill(0);
        bit1.fill(0);

        try {
            load_png(data_path("../assets/Pawn_right.png"), &palette_size, &palette_data, UpperLeftOrigin);
        }
        catch (std::runtime_error& e) {
            std::cout << e.what();
        }
        assert(palette_size.x == 8 && palette_size.y == 8);

        for (uint32_t y = 0; y < palette_size.y; y++) {
            for (uint32_t x = 0; x < palette_size.x; x++) {
                glm::u8vec4 color = palette_data[y * palette_size.x + x];
                if (std::find(palette_added.begin(), palette_added.end(), color) == palette_added.end()) 
                    palette_added.push_back(color);

                if (palette_added[0] == color) {
                    bit1[y] |= 1 << x;
                } else if (palette_added[1] == color) {
                    bit1[y] |= 1 << x;
                    bit0[y] |= 1 << x;
                } else {
                    // shouldn't happen
                    std::cout << "The png has more than 2 colors!" << std::endl;
                }
            }
        }
        assert(palette_added.size() == 2); // should only contain white and green

        tiles.push_back({ bit0, bit1 });

        // reset intermediary storages
        palette_added.clear();
        bit0.fill(0);
        bit1.fill(0);

        try {
            load_png(data_path("../assets/Pawn_down.png"), &palette_size, &palette_data, UpperLeftOrigin);
        }
        catch (std::runtime_error& e) {
            std::cout << e.what();
        }
        assert(palette_size.x == 8 && palette_size.y == 8);

        for (uint32_t y = 0; y < palette_size.y; y++) {
            for (uint32_t x = 0; x < palette_size.x; x++) {
                glm::u8vec4 color = palette_data[y * palette_size.x + x];
                if (std::find(palette_added.begin(), palette_added.end(), color) == palette_added.end()) 
                    palette_added.push_back(color);

                if (palette_added[0] == color) {
                    bit1[y] |= 1 << x;
                } else if (palette_added[1] == color) {
                    bit1[y] |= 1 << x;
                    bit0[y] |= 1 << x;
                } else {
                    // shouldn't happen
                    std::cout << "The png has more than 2 colors!" << std::endl;
                }
            }
        }
        assert(palette_added.size() == 2); // should only contain white and green

        tiles.push_back({ bit0, bit1 });

        // reset intermediary storages
        palette_added.clear();
        bit0.fill(0);
        bit1.fill(0);

        try {
            load_png(data_path("../assets/Pawn_left.png"), &palette_size, &palette_data, UpperLeftOrigin);
        }
        catch (std::runtime_error& e) {
            std::cout << e.what();
        }
        assert(palette_size.x == 8 && palette_size.y == 8);

        for (uint32_t y = 0; y < palette_size.y; y++) {
            for (uint32_t x = 0; x < palette_size.x; x++) {
                glm::u8vec4 color = palette_data[y * palette_size.x + x];
                if (std::find(palette_added.begin(), palette_added.end(), color) == palette_added.end()) 
                    palette_added.push_back(color);

                if (palette_added[0] == color) {
                    bit1[y] |= 1 << x;
                } else if (palette_added[1] == color) {
                    bit1[y] |= 1 << x;
                    bit0[y] |= 1 << x;
                } else {
                    // shouldn't happen
                    std::cout << "The png has more than 2 colors!" << std::endl;
                }
            }
        }
        assert(palette_added.size() == 2); // should only contain white and green

        tiles.push_back({ bit0, bit1 });

        // reset intermediary storages
        palette_added.clear();
        bit0.fill(0);
        bit1.fill(0);
    }

    // Generate Win/Lose tiles
    { // Win : Palette 4 & Tile 10 / Lost : Palette 5 & Tile 11
        try {
            load_png(data_path("../assets/Win.png"), &palette_size, &palette_data, UpperLeftOrigin);
        }
        catch (std::runtime_error& e) {
            std::cout << e.what();
        }
        assert(palette_size.x == 8 && palette_size.y == 8);

        for (uint32_t y = 0; y < palette_size.y; y++) {
            for (uint32_t x = 0; x < palette_size.x; x++) {
                glm::u8vec4 color = palette_data[y * palette_size.x + x];
                if (std::find(palette_added.begin(), palette_added.end(), color) == palette_added.end()) 
                    palette_added.push_back(color);

                if (palette_added[0] != color) {
                    // shouldn't happen
                    std::cout << "The png has more than 1 colors!" << std::endl;
                }
            }
        }
        assert(palette_added.size() == 1); // should only contain white, gray, and black

        palettes.push_back({palette_added[0],
                            glm::u8vec4(),
                            glm::u8vec4(),
                            glm::u8vec4()});
        tiles.push_back({ bit0, bit1 });

        // reset intermediary storages
        palette_added.clear();
        bit0.fill(0);
        bit1.fill(0);

        try {
            load_png(data_path("../assets/Lose.png"), &palette_size, &palette_data, UpperLeftOrigin);
        }
        catch (std::runtime_error& e) {
            std::cout << e.what();
        }
        assert(palette_size.x == 8 && palette_size.y == 8);

        for (uint32_t y = 0; y < palette_size.y; y++) {
            for (uint32_t x = 0; x < palette_size.x; x++) {
                glm::u8vec4 color = palette_data[y * palette_size.x + x];
                if (std::find(palette_added.begin(), palette_added.end(), color) == palette_added.end()) 
                    palette_added.push_back(color);
                    
                if (palette_added[0] != color) {
                    // shouldn't happen
                    std::cout << "The png has more than 1 colors!" << std::endl;
                }
            }
        }
        assert(palette_added.size() == 1); // should only contain white, gray, and black

        palettes.push_back({palette_added[0],
                            glm::u8vec4(),
                            glm::u8vec4(),
                            glm::u8vec4()});
        tiles.push_back({ bit0, bit1 });

        assert(background.size() == PPU466::BackgroundHeight * PPU466::BackgroundWidth);

        // reset intermediary storages
        palette_added.clear();
        bit0.fill(0);
        bit1.fill(0);
    }


    // sanity check on what I'm importing
    assert(palettes.size() == 6);
    assert(tiles.size() == 12);
    assert(sprites.size() == 61);

    // export binary data to be read during runtime
    std::ofstream ofs(data_path("asset.bin"), std::ios::out | std::ios::binary);
    
    write_chunk("pale", palettes, &ofs);
    write_chunk("tile", tiles, &ofs);
    write_chunk("spri", sprites, &ofs);
    write_chunk("bgim", background, &ofs);
}