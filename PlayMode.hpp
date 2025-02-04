#include "PPU466.hpp"
#include "Mode.hpp"

#include <glm/glm.hpp>

#include <vector>
#include <deque>

struct PlayMode : Mode {
	PlayMode();
	virtual ~PlayMode();

	//functions called by main loop:
	virtual bool handle_event(SDL_Event const &, glm::uvec2 const &window_size) override;
	virtual void update(float elapsed) override;
	virtual void draw(glm::uvec2 const &drawable_size) override;

	void gameWin();
	void gameLose();

	//----- game state -----
	enum GameState{
		PLAYING,
		VICTORY,
		DEFEAT
	} gameState;

	// holders for win/lose background
	std::array< uint16_t, PPU466::BackgroundWidth * PPU466::BackgroundHeight > win_bg;
	std::array< uint16_t, PPU466::BackgroundWidth * PPU466::BackgroundHeight > lose_bg;

	//input tracking:
	struct Button {
		uint8_t downs = 0;
		uint8_t pressed = 0;
	} left, right, down, up;

	//Bullet struct
	struct Bullet {
		bool is_active;
		glm::vec2 sprite_at;
		glm::vec2 dir;
	};

	std::vector< Bullet > bullets;
	uint8_t active_bullet_count = 0;
	uint8_t total_bullet_count = 0;

	float elapsed_time_since = 0;
	float bullet_interval = 3;

	//some weird background animation:
	float background_fade = 0.0f;

	//player position:
	glm::vec2 player_at = glm::vec2(0.0f);

	//----- drawing handled by PPU466 -----

	PPU466 ppu;
};
