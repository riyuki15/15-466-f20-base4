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
std::shared_ptr< MenuMode > good_over_menu;
std::shared_ptr< MenuMode > bad_over_menu;
std::shared_ptr< MenuMode > instr_menu;
std::vector< std::shared_ptr< MenuMode > > menuModes;
MenuMode::Menus menus;
std::vector<int> correctChoices;
int total_scenes = 8;

void load_text_scenes() {
  for (int i = 0; i < total_scenes; i++) {
    std::ifstream file ("textScenes/scene" + std::to_string(i) + ".txt");
    std::string line;
    MenuMode::TextScene textScene;
    if (file.is_open()) {
      std::vector<std::string> description;
      while (std::getline (file,line)) {
        if(line.rfind("-", 0) == 0) {
          textScene.choices_text.emplace_back(line.substr(1));
        } else if (line.rfind("*", 0) == 0) {
          std::cout << "correct choice: " + line.substr(1) << std::endl;
          correctChoices.emplace_back(std::atoi(line.substr(1).c_str()));
        } else {
          textScene.text.emplace_back(line);
        }
      }
      file.close();
      menus.textScenes.emplace_back(textScene);
    }
  }
}
  

Load< void > load_demo_menu(LoadTagDefault, []() {
  load_text_scenes();
  for (int j = 0; j < total_scenes; j++) {
    std::shared_ptr< MenuMode > cur_menu;
    std::vector< MenuMode::Item > items;
    MenuMode::TextScene cur = menus.textScenes[j];
    for (int i = 0; i < cur.text.size(); i++) {
      items.emplace_back(cur.text[i], true);
    }
    for (int i = 0; i < cur.choices_text.size(); i++) {
      items.emplace_back(cur.choices_text[i], false);
    }

    cur_menu = std::make_shared< MenuMode >(items);
    cur_menu->selected = cur.text.size();
    std::cout << cur_menu->items[cur_menu->selected].content << std::endl;
    
    // //demo_menu->atlas = trade_font_atlas;
    // cur_menu->view_min = glm::vec2(0.0f, 0.0f);
    // cur_menu->view_max = glm::vec2(320.0f, 200.0f);

    // cur_menu->layout_items(2.0f);

    // //demo_menu->left_select = &trade_font_atlas->lookup(">");
    // cur_menu->left_select_offset = glm::vec2(-5.0f - 3.0f, 0.0f);
    // //demo_menu->right_select = &trade_font_atlas->lookup("<");
    // cur_menu->right_select_offset = glm::vec2(0.0f + 3.0f, 0.0f);
    // cur_menu->select_bounce_amount = 5.0f;
    menuModes.emplace_back(cur_menu);
  }

  demo_menu = menuModes[0];
  good_over_menu = menuModes[total_scenes - 2];
  bad_over_menu = menuModes[total_scenes - 1];
  instr_menu = menuModes[1];

  int last_failed = 0;
  // set on-selects
  for (int l = 0; l < total_scenes; l++) {
    if (l == 0) {
      std::cout << "at start menu" << std::endl;
      // start menu: start
      menuModes[l]->items[0].on_select = [](MenuMode::Item const &) {
        // Mode::set_current(std::make_shared<PlayMode>(0));
        Mode::set_current(menuModes[2]);
      };
      // start menu: instructions
      menuModes[l]->items[1].on_select = [](MenuMode::Item const &) {
          Mode::set_current(instr_menu);
      };
    } else if (l == 1) {
      // instruction menu: back
      std::cout << "at instruction menu" << std::endl;
      menuModes[l]->items[4].on_select = [](MenuMode::Item const &) {
          Mode::set_current(demo_menu);
      };
    } else if (l >= 2 && l < total_scenes - 2) {
      // game selection menu: both choices
      int first_choice = menuModes[l]->selected;
      for (int m = first_choice; m < menuModes[l]->items.size(); m++) {
        int correctChoice = correctChoices[l-2];
        if ((m-first_choice) == correctChoice) {
          menuModes[l]->items[m].on_select = [l](MenuMode::Item const &) {
            Mode::set_current(std::make_shared<PlayMode>(l-2));
          };
        } else {
          menuModes[l]->items[m].on_select = [&last_failed, l](MenuMode::Item const &) {
            last_failed = l;
            Mode::set_current(bad_over_menu);
          };
        }
        
      }
    } else if (l == total_scenes - 2) {
      for (int m = 1; m < menuModes[l]->items.size(); m++) {
        menuModes[l]->items[m].on_select = [](MenuMode::Item const &) {
            Mode::set_current(good_over_menu);
        };
      }
    } else if (l == total_scenes - 1) {
      menuModes[l]->items[menuModes[l]->items.size()-1].on_select = [last_failed](MenuMode::Item const &) {
            std::cout << "back at " + std::to_string(last_failed) << std::endl;
            Mode::set_current(menuModes[last_failed]);
      };
    }
//    for (int m = 0; m < menuModes[l]->items.size(); m++) {
//      if (menuModes[l]->items[m].name.compare(menus.textScenes[l].text) == 0) continue;
//      int next_scene = menus.textScenes[l].next_scenes[m - 1];
//      menuModes[l]->items[m].on_select = [next_scene](MenuMode::Item const&) {
//          Mode::set_current(menuModes[next_scene]);
//      };
//    }
  }
//  menuModes[0]->background = std::shared_ptr< PlayMode >();

});