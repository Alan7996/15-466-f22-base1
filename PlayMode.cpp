#include "PlayMode.hpp"

//#include "asset_generator.hpp"
#include "asset_loader.hpp"

//for the GL_ERRORS() macro:
#include "gl_errors.hpp"

//for glm::value_ptr() :
#include <glm/gtc/type_ptr.hpp>

#include <random>

PlayMode::PlayMode() {
	//TODO:
	// you *must* use an asset pipeline of some sort to generate tiles.
	// don't hardcode them like this!
	// or, at least, if you do hardcode them like this,
	//  make yourself a script that spits out the code that you paste in here
	//   and check that script into your repository.

	//generate_data();
	load_asset(ppu);
	
	player_at = glm::vec2(ppu.sprites[0].x, ppu.sprites[0].y);

	// randomly generate bullet positions, taking turns on each side
	// we know there are 60 enemy bullets, so 15 on each side
	// random number generation based on https://stackoverflow.com/questions/5008804/generating-a-random-integer-from-a-range
	std::random_device rd;
	std::mt19937 rng(rd());

	uint32_t scrWidth = PPU466::ScreenWidth;
	uint32_t scrHeight = PPU466::ScreenHeight;

	std::uniform_int_distribution<int> uniWidth(0, scrWidth);
	std::uniform_int_distribution<int> uniHeight(0, scrHeight);
		
	for (uint32_t i = 1; i < 61; i++) {
		if (i % 4 == 0) { // top
			ppu.sprites[i].x = static_cast<uint8_t>(uniWidth(rng));
			ppu.sprites[i].y = static_cast<uint8_t>(scrHeight);
			ppu.sprites[i].index = 8;
		} else if (i % 4 == 1) { // right
			ppu.sprites[i].x = static_cast<uint8_t>(scrWidth) - 1;
			ppu.sprites[i].y = static_cast<uint8_t>(uniHeight(rng));
			ppu.sprites[i].index = 9;
		} else if (i % 4 == 2) { // bottom
			ppu.sprites[i].x = static_cast<uint8_t>(uniWidth(rng));
			ppu.sprites[i].y = 0u;
			ppu.sprites[i].index = 6;
		} else if (i % 4 == 3) { // left
			ppu.sprites[i].x = 0u;
			ppu.sprites[i].y = static_cast<uint8_t>(uniHeight(rng));
			ppu.sprites[i].index = 7;
		}
		ppu.sprites[i].attributes |= 0b10000000; // set behind background, so don't render for now
		
		bullets.push_back({glm::vec2(ppu.sprites[i].x, ppu.sprites[i].y), glm::vec2()});
	}

	// prepare backgrounds for win / lose condition
	win_bg.resize(ppu.BackgroundWidth * ppu.BackgroundHeight);
	uint16_t win_data = 0b0000010000001010u;
	std::fill(win_bg.begin(), win_bg.end(), win_data);

	lose_bg.resize(ppu.BackgroundWidth * ppu.BackgroundHeight);
	uint16_t lose_data = 0b0000010100001011u;
	std::fill(lose_bg.begin(), lose_bg.end(), lose_data);

	gameState = PLAYING;

}

PlayMode::~PlayMode() {
}

bool PlayMode::handle_event(SDL_Event const &evt, glm::uvec2 const &window_size) {

	if (evt.type == SDL_KEYDOWN) {
		if (evt.key.keysym.sym == SDLK_LEFT) {
			left.downs += 1;
			left.pressed = true;
			return true;
		} else if (evt.key.keysym.sym == SDLK_RIGHT) {
			right.downs += 1;
			right.pressed = true;
			return true;
		} else if (evt.key.keysym.sym == SDLK_UP) {
			up.downs += 1;
			up.pressed = true;
			return true;
		} else if (evt.key.keysym.sym == SDLK_DOWN) {
			down.downs += 1;
			down.pressed = true;
			return true;
		}
	} else if (evt.type == SDL_KEYUP) {
		if (evt.key.keysym.sym == SDLK_LEFT) {
			left.pressed = false;
			return true;
		} else if (evt.key.keysym.sym == SDLK_RIGHT) {
			right.pressed = false;
			return true;
		} else if (evt.key.keysym.sym == SDLK_UP) {
			up.pressed = false;
			return true;
		} else if (evt.key.keysym.sym == SDLK_DOWN) {
			down.pressed = false;
			return true;
		}
	}

	return false;
}

void PlayMode::update(float elapsed) {

	if (gameState != PLAYING) return;

	constexpr float PlayerSpeed = 30.0f;
	if (left.pressed) {
		player_at.x -= PlayerSpeed * elapsed;
		ppu.sprites[0].index = 5;
	}
	if (right.pressed) {
		player_at.x += PlayerSpeed * elapsed;
		ppu.sprites[0].index = 3;
	}
	if (down.pressed) {
		player_at.y -= PlayerSpeed * elapsed;
		ppu.sprites[0].index = 4;
	}
	if (up.pressed) {
		player_at.y += PlayerSpeed * elapsed;
		ppu.sprites[0].index = 2;
	}

	//reset button press counters:
	left.downs = 0;
	right.downs = 0;
	up.downs = 0;
	down.downs = 0;

	// move activated bullets
	static float bulletSpeed = 20.0f;

	for (uint8_t i = 0; i < active_bullet_count; i++) {
		bullets[i].sprite_at.x += bullets[i].dir.x * bulletSpeed * elapsed;
		bullets[i].sprite_at.y += bullets[i].dir.y * bulletSpeed * elapsed;

		// if the bullet exits screen, reset it
		if (bullets[i].sprite_at.x < 0 || bullets[i].sprite_at.x > PPU466::ScreenWidth ||
			bullets[i].sprite_at.y < 0 || bullets[i].sprite_at.y > PPU466::ScreenHeight) {
			ppu.sprites[i + 1].attributes |= 0b10000000; // make invisible
			bullets[i].dir = glm::vec2(); // reset movement
		}
	}

	// activate a new bullet every certain time interval
	elapsed_time_since += elapsed;
	if (gameState == PLAYING && elapsed_time_since > bullet_interval) {
		if (active_bullet_count == 60) {
			gameWin();
			return;
		}

		elapsed_time_since = 0;
		bullet_interval -= 0.05f;

		ppu.sprites[1 + active_bullet_count].attributes &= 0b01111111; // make visible

		// initialize new bullet
		bullets[active_bullet_count].sprite_at = glm::vec2(ppu.sprites[1 + active_bullet_count].x, ppu.sprites[1 + active_bullet_count].y);
		bullets[active_bullet_count].dir = glm::vec2(
			player_at.x - bullets[active_bullet_count].sprite_at.x == 0 ? 0 : (player_at.x - bullets[active_bullet_count].sprite_at.x) / std::abs(player_at.x - bullets[active_bullet_count].sprite_at.x), 
			player_at.y - bullets[active_bullet_count].sprite_at.y == 0 ? 0 : (player_at.y - bullets[active_bullet_count].sprite_at.y) / std::abs(player_at.y - bullets[active_bullet_count].sprite_at.y));

		active_bullet_count++;
		bulletSpeed += 1.0f;
	}
}

void PlayMode::draw(glm::uvec2 const &drawable_size) {
	//--- set ppu state based on game state ---

	//background scroll:
	ppu.background_position.x = int32_t(-1.5f * player_at.x);
	ppu.background_position.y = int32_t(-1.5f * player_at.y);

	//player sprite:
	ppu.sprites[0].x = int8_t(player_at.x);
	ppu.sprites[0].y = int8_t(player_at.y);

	//draw activated bullet sprites' at updated positions:
	for (uint32_t i = 1u; i < 1u + active_bullet_count; i++) {
		ppu.sprites[i].x = int8_t(bullets[i - 1].sprite_at.x);
		ppu.sprites[i].y = int8_t(bullets[i - 1].sprite_at.y);
	}

	//--- actually draw ---
	ppu.draw(drawable_size);
}

void PlayMode::gameWin() {

	gameState = VICTORY;

	// make player and all bullets invisible
	for (uint8_t i = 0; i < 61; i++) {
		ppu.sprites[i].attributes |= 0b10000000;
	}

	// activate victory background
    for (uint32_t i = 0; i < win_bg.size(); i++) {
		ppu.background[i] = win_bg[i];
	}
}
