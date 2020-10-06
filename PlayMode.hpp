#include "Mode.hpp"

#include "Scene.hpp"
#include "Sound.hpp"

#include <glm/glm.hpp>

#include <vector>
#include <deque>
#include <ft2build.h>
#include FT_FREETYPE_H

struct PlayMode : Mode {
	PlayMode(int cur);
	virtual ~PlayMode();

	int cur_scene;

	//functions called by main loop:
	virtual bool handle_event(SDL_Event const &, glm::uvec2 const &window_size) override;
	virtual void update(float elapsed) override;
	virtual void draw(glm::uvec2 const &drawable_size) override;
	
	// Freetype initialization
	FT_Library  library;   /* handle to library     */
	FT_Face     face;      /* handle to face object */

	//----- game state -----

	//input tracking:
	struct Button {
		uint8_t downs = 0;
		uint8_t pressed = 0;
	} left, right, down, up, enter;

  //local copy of the game scene (so code can change it during gameplay):
  Scene scene;

	//hexapod leg to wobble:
	Scene::Transform *ground = nullptr;
	Scene::Transform *gourmet = nullptr;
	std::vector < Scene::Transform* > crumbs;

	int crumbs_count = 3;

	float amplitude = 0.001f;
	float t = 0.f;

	bool next_pressed = false;

	//music coming from the tip of the leg (as a demonstration):
	std::shared_ptr< Sound::PlayingSample > leg_tip_loop;
	
	//camera:
	Scene::Camera *camera = nullptr;

};
