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
	DrawTexts(glm::mat4 const &world_to_clip, std::string fontname);

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
	struct Vertex {
		Vertex(glm::vec3 const &Position_, glm::vec2 const &TexCoord_, glm::u8vec4 const &Color_) : Position(Position_), TexCoord(TexCoord_), Color(Color_) { }
		glm::vec3 Position;
		glm::vec2 TexCoord;
		glm::u8vec4 Color;
	};

	// struct Vertex {
	// 	Vertex(glm::vec3 const &Position_, glm::u8vec4 const &Color_) : Position(Position_), Color(Color_){ }
	// 	glm::vec3 Position;
	// 	glm::u8vec4 Color;
	// };
	std::vector< Vertex > attribs;

	/// Holds all state information relevant to a character as loaded using FreeType
	struct Character {
		Character(GLuint const &TextureID_, glm::vec2 const &Size_, glm::vec2 const &Bearing_, double const &Advance_) : 
			TextureID(TextureID_), Size(Size_), Bearing(Bearing_), Advance(Advance_) {}
		GLuint TextureID; // ID handle of the glyph texture
		glm::vec2   Size;      // Size of glyph
		glm::vec2   Bearing;   // Offset from baseline to left/top of glyph
		double Advance;   // Horizontal offset to advance to next glyph
	};

};
