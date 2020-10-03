/* demo_menu.cpp source - f19 base6 - https://github.com/15-466/15-466-f19-base6 */

#include "demo_menu.hpp"
#include "Load.hpp"
#include "data_path.hpp"

#include "PlayMode.hpp"

#include <iostream>
#include <fstream>

//Load< SpriteAtlas > trade_font_atlas(LoadTagDefault, []() -> SpriteAtlas const* {
//	return new SpriteAtlas(data_path("trade-font"));
//});

std::shared_ptr< MenuMode > demo_menu;
MenuMode::Menus menus;
int scene_count = 2;

void load_text_scenes() {
  for (int i = 0; i < scene_count; i++) {
    std::ifstream file ("textScenes/scene" + std::to_string(i) + ".txt");
    std::string line;
    MenuMode::TextScene textScene;
    if (file.is_open()) {
      while (std::getline (file,line)) {
        if(line.rfind("-", 0) == 0) {
          int arrow = line.find("->");
          textScene.choices_text.emplace_back(line.substr(1, arrow - 1));
          textScene.next_scenes.emplace_back(std::stoi(line.substr(arrow + 2, 1)));
          textScene.choice_count++;
        } else {
          textScene.text = line;
        }
      }
      file.close();
      menus.textScenes.emplace_back(textScene);
      menus.scene_count++;
    }
  }
}

void go_to_next_scene(int choice) {
  menus.current_scene = menus.textScenes[menus.current_scene].next_scenes[choice];
}

Load< void > load_demo_menu(LoadTagDefault, []() {
  load_text_scenes();
  std::vector< MenuMode::Item > items;
  MenuMode::TextScene cur = menus.textScenes[0];
  items.emplace_back(cur.text);
  for (int i = 0; i < cur.choice_count; i++) {
    items.emplace_back(cur.choices_text[i]);
    items.back().on_select = [](MenuMode::Item const&) {
        Mode::set_current(std::make_shared< PlayMode >());
    };
  }
	/*items.emplace_back("lighting - forward");
	items.back().on_select = [](MenuMode::Item const&) {
		Mode::set_current(std::make_shared< DemoLightingForwardMode >());
	};
	items.emplace_back("lighting - deferred");
	items.back().on_select = [](MenuMode::Item const&) {
		Mode::set_current(std::make_shared< DemoLightingDeferredMode >());
	};*/


	demo_menu = std::make_shared< MenuMode >(items);
	demo_menu->selected = 1;
	//demo_menu->atlas = trade_font_atlas;
	demo_menu->view_min = glm::vec2(0.0f, 0.0f);
	demo_menu->view_max = glm::vec2(320.0f, 200.0f);

	demo_menu->layout_items(2.0f);

	//demo_menu->left_select = &trade_font_atlas->lookup(">");
	demo_menu->left_select_offset = glm::vec2(-5.0f - 3.0f, 0.0f);
	//demo_menu->right_select = &trade_font_atlas->lookup("<");
	demo_menu->right_select_offset = glm::vec2(0.0f + 3.0f, 0.0f);
	demo_menu->select_bounce_amount = 5.0f;
});