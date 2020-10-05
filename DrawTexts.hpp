#pragma once

#include <glm/gtc/type_ptr.hpp>
#include "GL.hpp"
#include <vector>
#include <functional>
#include <string>
#include <ft2build.h>
#include <hb-ft.h>
#include <hb.h>

#include FT_FREETYPE_H

struct DrawTexts {
	//Start drawing; will remember world_to_clip matrix:
	DrawTexts(glm::mat4 const &world_to_clip);

	//draw wireframe text, start at anchor, move in x direction, mat gives x and y directions for text drawing:
	// (default character box is 1 unit high)
	void draw_texts(std::string const &text, 
					glm::vec3 const &anchor_in, 
					glm::vec3 const &x, 
					glm::vec3 const &y, 
					glm::u8vec4 const &color, 
					glm::vec3 *anchor_out = nullptr);
	void draw_glyph(hb_codepoint_t glyphid, 
					glm::u8vec4 color,  
					float x_pos, 
					float y_pos,
					glm::vec3 const &x, 
					glm::vec3 const &y,
					float w,
					float h);
	GLuint load_texture();

	//Finish drawing (push attribs to GPU):
	~DrawTexts();

    //----- font ------
	FT_Library  library;   /* handle to library     */
	FT_Face     face;      /* handle to face object */
	FT_Error ft_error;
	int8_t FONT_SIZE = 1;
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
};
