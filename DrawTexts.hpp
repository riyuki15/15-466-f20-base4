#pragma once

#include <glm/gtc/type_ptr.hpp>
#include "GL.hpp"
#include <vector>
#include <functional>
#include <string>
#include <ft2build.h>
#include <hb-ft.h>
#include <hb.h>
#include <map>

#include FT_FREETYPE_H

struct DrawTexts {
	//Start drawing; will remember world_to_clip matrix:
	DrawTexts(glm::mat4 const &world_to_clip);

	//draw wireframe text, start at anchor, move in x direction, mat gives x and y directions for text drawing:
	// (default character box is 1 unit high)
	void draw_texts(std::string const &text, 
					glm::vec3 anchor, 
					float scale,
					glm::u8vec4 const &color);

	//Finish drawing (push attribs to GPU):
	~DrawTexts();

    //----- font ------
	FT_Library  library;   /* handle to library     */
	FT_Face     face;      /* handle to face object */
	FT_Error ft_error;
	int8_t FONT_SIZE = 48;
	hb_font_t *hb_font;
	hb_buffer_t *hb_buffer;

	glm::mat4 world_to_clip;
	// struct Vertex {
	// 	Vertex(glm::vec3 const &Position_, glm::u8vec4 const &Color_, glm::vec2 const &Texture_) : Position(Position_), Color(Color_), Texture(Texture_){ }
	// 	glm::vec3 Position;
	// 	glm::u8vec4 Color;
	// 	glm::vec2 Texture;
	// };

	struct Vertex {
		Vertex(glm::vec3 const &Position_, glm::u8vec4 const &Color_) : Position(Position_), Color(Color_){ }
		glm::vec3 Position;
		glm::u8vec4 Color;
	};
	std::vector< Vertex > attribs;

	/// Holds all state information relevant to a character as loaded using FreeType
	struct Character {
		unsigned int TextureID; // ID handle of the glyph texture
		glm::ivec2   Size;      // Size of glyph
		glm::ivec2   Bearing;   // Offset from baseline to left/top of glyph
		unsigned int Advance;   // Horizontal offset to advance to next glyph
	};

	std::map<GLchar, Character> Characters;
};
