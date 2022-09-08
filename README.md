# The Pawn-Hell

Author: Alan Lee / soohyun3

Design: This is a bullet hell game but you are a chess king trying to survive incoming bullet-pawns.
        This game makes use of basic resource management with some randomness and collision detection.

Screen Shot:

![Screen Shot](screenshot.png)

How Your Asset Pipeline Works:

My [asset generator](asset_generator.cpp) firstly creates the binary data to be used for loading sprites during runtime.

This script takes in the source png files (such as the background tile ![tile](assets/Chessboard_tile2.png) or the king ![king](assets/King_down.png)),
loads them through [load_png function](load_save_png.cpp) and saves the color data as a vector of `glm::u8vec4`.
With these preliminary data and direct knowledge regarding source asset specifications that I have from controlling the assets, 
vectors of palettes, tiles, sprites, and background are generated accordingly and gets `write_chunk`-ed to `dist/asset.bin`.

Having made these data, now the [asset loader](asset_loader.cpp) reads the binary file and sets up relevant
palettes, tiles, sprites, and so on for the game to use. Since the assets were already in vectors, as we reinterpret
the binary we can simply copy the results of `read_chunk` to `PlayMode`'s `ppu` instance.
At this point the `load_asset` also takes in a reference to the `bullet_count` variable of `PlayMode` class so that
the runtime program knows how many bullet sprites exist in-game without an extra loop in `PlayMode`.

How To Play:

Press any combination of the four direction keys to move towards Left/Right/Up/Down/Diagonal directions.

Your goal is to survive through 60 pawns flying towards you to kill you!
If you touch any of the pawns you die and lose the game (and face a sad red screen).
If you survive till the end, you win (and face a congratulatory green screen)!

The bullets get spawned from each wall of the screen taking turns, and the bullet respawn time and travel speed gets faster as game progresses.
There is no special trick or strategy in this game, you just need to be good with your arrow key controls.

This game was built with [NEST](NEST.md).