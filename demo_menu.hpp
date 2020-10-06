/* demo_menu.hpp source - f19 base6 - https://github.com/15-466/15-466-f19-base6 */

#pragma once

#include "MenuMode.hpp"
//#include <memory>

extern std::shared_ptr< MenuMode > demo_menu;
extern std::shared_ptr< MenuMode > over_menu;
extern std::shared_ptr< MenuMode > instr_menu;
extern std::vector< std::shared_ptr< MenuMode > > menuModes;