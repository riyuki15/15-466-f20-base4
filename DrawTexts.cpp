#include "DrawTexts.hpp"
#include "ColorTextureProgram.hpp"
#include "ColorProgram.hpp"
#include "data_path.hpp"
#include "PathFont.hpp"
#include "gl_errors.hpp"
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>

//n.b. declared static so they don't conflict with similarly named global variables elsewhere:
static GLuint vertex_buffer = 0;
static GLuint vertex_buffer_for_color_program = 0;

// static Load< void > setup_buffers(LoadTagDefault, [](){
// 	{ //set up vertex buffer:
// 		glGenBuffers(1, &vertex_buffer);
// 		//for now, buffer will be un-filled.
// 	}

// 	{ //vertex array mapping buffer for color_program:
// 		//ask OpenGL to fill vertex_buffer_for_color_program with the name of an unused vertex array object:
// 		glGenVertexArrays(1, &vertex_buffer_for_color_program);

// 		//set vertex_buffer_for_color_program as the current vertex array object:
// 		glBindVertexArray(vertex_buffer_for_color_program);

// 		//set vertex_buffer as the source of glVertexAttribPointer() commands:
// 		glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);

// 		//set up the vertex array object to describe arrays of PongMode::Vertex:
// 		glVertexAttribPointer(
// 			color_texture_program->Position_vec4, //attribute
// 			3, //size
// 			GL_FLOAT, //type
// 			GL_FALSE, //normalized
// 			sizeof(DrawTexts::Vertex), //stride
// 			(GLbyte *)0 + offsetof(DrawTexts::Vertex, Position) //offset
// 		);
// 		glEnableVertexAttribArray(color_texture_program->Position_vec4);
// 		//[Note that it is okay to bind a vec3 input to a vec4 attribute -- the w component will be filled with 1.0 automatically]

// 		glVertexAttribPointer(
// 			color_texture_program->Color_vec4, //attribute
// 			4, //size
// 			GL_UNSIGNED_BYTE, //type
// 			GL_TRUE, //normalized
// 			sizeof(DrawTexts::Vertex), //stride
// 			(GLbyte *)0 + offsetof(DrawTexts::Vertex, Color) //offset
// 		);
// 		glEnableVertexAttribArray(color_texture_program->Color_vec4);

// 		glVertexAttribPointer(
// 			color_texture_program->TexCoord_vec2, //attribute
// 			2, //size
// 			GL_FLOAT, //type
// 			GL_FALSE, //normalized
// 			sizeof(DrawTexts::Vertex), //stride
// 			(GLbyte *)0 + offsetof(DrawTexts::Vertex, Texture) //offset
// 		);
// 		glEnableVertexAttribArray(color_texture_program->TexCoord_vec2);

// 		//done referring to vertex_buffer, so unbind it:
// 		glBindBuffer(GL_ARRAY_BUFFER, 0);

// 		//done setting up vertex array object, so unbind it:
// 		glBindVertexArray(0);
// 	}

// 	GL_ERRORS(); //PARANOIA: make sure nothing strange happened during setup
// });


// DrawTexts::DrawTexts(glm::mat4 const &world_to_clip_) : world_to_clip(world_to_clip_) {
//     ft_error = FT_Init_FreeType( &library );
// 	if (ft_error) { std::runtime_error("Freetype library error: " + std::to_string(ft_error));}

// 	ft_error = FT_New_Face(library,
// 				data_path("blue-eyes.otf").c_str(),
// 				0,
// 				&face );
// 	if (ft_error == FT_Err_Unknown_File_Format) {	
// 		throw std::runtime_error("The font file could be opened and read, but it appears that its font format is unsupported");
// 	} else if (ft_error) { 
// 		throw std::runtime_error("Another error code means that the font file could not be opened or read, or that it is broken.");
// 	}
// 	assert(face != nullptr);

// 	// ft_error = FT_Set_Char_Size(face, FONT_SIZE, FONT_SIZE, 0, 0);
// 	ft_error = FT_Set_Pixel_Sizes(face, 0, FONT_SIZE);
// 	if (ft_error) {
// 		std::runtime_error("Set char size failed.");
// 	}
	
// 	hb_font = hb_ft_font_create_referenced(face);
// 	hb_buffer = hb_buffer_create();
// }


// DrawTexts::~DrawTexts() {
// 	hb_buffer_destroy(hb_buffer);
// 	hb_font_destroy(hb_font);
// 	FT_Done_Face (face);
//   	FT_Done_FreeType (library);
// }


// // code adpated from @wdlzz926 in Discord
// GLuint DrawTexts::load_texture() {
// 	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
// 	GLuint tex;
// 	glGenTextures(1, &tex);
// 	glBindTexture(GL_TEXTURE_2D, tex);
// 	glTexImage2D(
// 		GL_TEXTURE_2D,
// 		0,
// 		GL_RED,
// 		face->glyph->bitmap.width,
// 		face->glyph->bitmap.rows,
// 		0,
// 		GL_RED,
// 		GL_UNSIGNED_BYTE,
// 		face->glyph->bitmap.buffer
// 	);
// 	// set texture options
// 	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
// 	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
// 	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
// 	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

// 	// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_R, GL_ONE);
// 	// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_G, GL_ONE);
// 	// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_B, GL_ONE);
// 	// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_A, GL_RED);
// 	return tex;
// }

// // Draws a glyph
// void DrawTexts::draw_glyph(hb_codepoint_t glyphid, 
// 							glm::u8vec4 color, 
// 							double x_pos, 
// 							double y_pos,
// 							double w,
// 							double h) {
// 	char glyphname[32];
// 	hb_font_get_glyph_name(hb_font, glyphid, glyphname, sizeof(glyphname));
// 	printf("%d %s %f %f %f %f\n", glyphid, glyphname, x_pos, y_pos, w, h);
// 	std::vector<Vertex> attribs;

// 	attribs.emplace_back(glm::vec3(x_pos, 0.0f, 0.0f) + 
// 							glm::vec3(0.0f, y_pos + h, 0.0f), color, glm::vec2(0.0f, 0.0f));
// 	attribs.emplace_back(glm::vec3(x_pos, 0.0f, 0.0f) + 
// 							glm::vec3(0.0f, y_pos, 0.0f), color, glm::vec2(0.0f, 1.0f));
// 	attribs.emplace_back(glm::vec3(x_pos + w, 0.0f, 0.0f) + 
// 							glm::vec3(0.0f, y_pos, 0.0f), color, glm::vec2(1.0f, 1.0f));

// 	attribs.emplace_back(glm::vec3(x_pos, 0.0f, 0.0f) + 
// 							glm::vec3(0.0f, y_pos + h, 0.0f), color, glm::vec2(0.0f, 0.0f));
// 	attribs.emplace_back(glm::vec3(x_pos + w, 0.0f, 0.0f) + 
// 							glm::vec3(0.0f, y_pos, 0.0f), color, glm::vec2(1.0f, 1.0f));
// 	attribs.emplace_back(glm::vec3(x_pos + w, 0.0f, 0.0f) + 
// 							glm::vec3(0.0f, y_pos + h, 0.0f), color, glm::vec2(1.0f, 0.0f));
	
// 	GLuint tex = load_texture();

// 	std::cout << "attrib size = " + std::to_string(attribs.size()) << std::endl;

// 	//upload vertices to vertex_buffer:
// 	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer); //set vertex_buffer as current
// 	glBufferData(GL_ARRAY_BUFFER, attribs.size() * sizeof(attribs[0]), attribs.data(), GL_STREAM_DRAW); //upload attribs array
// 	glBindBuffer(GL_ARRAY_BUFFER, 0);

// 	//set color_program as current program:
// 	glUseProgram(color_texture_program->program);

// 	//upload OBJECT_TO_CLIP to the proper uniform location:
// 	glUniformMatrix4fv(color_texture_program->OBJECT_TO_CLIP_mat4, 1, GL_FALSE, glm::value_ptr(world_to_clip));

// 	//use the mapping vertex_buffer_for_color_program to fetch vertex data:
// 	glBindVertexArray(vertex_buffer_for_color_program);

// 	//bind texture to location zero:
// 	glActiveTexture(GL_TEXTURE0);
// 	glBindTexture(GL_TEXTURE_2D, tex);

// 	//run the OpenGL pipeline:
// 	glDrawArrays(GL_TRIANGLES, 0, GLsizei(attribs.size()));

// 	//unbind the sprite texture:
// 	glBindTexture(GL_TEXTURE_2D, 0);

// 	//reset vertex array to none:
// 	glBindVertexArray(0);

// 	//reset current program to none:
// 	glUseProgram(0);
// }


// void DrawTexts::draw_texts(std::string const &text, 
// 						glm::vec3 const &anchor_in, 
// 						glm::u8vec4 const &color, 
// 						glm::vec3 *anchor_out) {
// 	/* harfbuzz code quoted 
// 	* from https://github.com/harfbuzz/harfbuzz-tutorial/blob/master/hello-harfbuzz-freetype.c
// 	*/
// 	glm::vec3 anchor = anchor_in;

// 	std::cout << "trying to draw: " << text << std::endl;

// 	hb_buffer_reset(hb_buffer);
// 	hb_buffer_add_utf8(hb_buffer, text.data(), -1, 0, -1);
// 	hb_buffer_set_direction(hb_buffer, HB_DIRECTION_LTR);
// 	hb_buffer_set_script(hb_buffer, HB_SCRIPT_LATIN);
// 	hb_buffer_set_language(hb_buffer, hb_language_from_string("en", -1));

// 	/* Shape it! */
// 	hb_shape (hb_font, hb_buffer, NULL, 0);

// 	/* Get glyph information and positions out of the buffer. */
// 	unsigned int glyph_count = hb_buffer_get_length (hb_buffer);
// 	hb_glyph_info_t *glyph_info    = hb_buffer_get_glyph_infos(hb_buffer, nullptr);
// 	hb_glyph_position_t *glyph_pos = hb_buffer_get_glyph_positions(hb_buffer, nullptr);

// 	// Iterate over each glyph
// 	for (unsigned int i = 0; i < glyph_count; ++i) {
// 		hb_codepoint_t glyphid = glyph_info[i].codepoint;

// 		ft_error = FT_Load_Glyph(face, glyphid, FT_LOAD_DEFAULT);
// 		if (ft_error) throw std::runtime_error("Error on FT_Load_Glyph()");
// 		ft_error = FT_Render_Glyph(face->glyph, FT_RENDER_MODE_NORMAL);
// 		if (ft_error) throw std::runtime_error("Error on FT_Render_Glyph()");

// 		double x_offset = glyph_pos[i].x_offset / 64.0f;
// 		// double y_offset = glyph_pos[i].y_offset / 64.0f;
// 		double x_advance = glyph_pos[i].x_advance / 64.0f;
// 		double w = x_advance;
// 		double h = x_advance;

// 		std::cout << "x_offset: " + std::to_string(x_offset) + 
// 					" x_advance: " + std::to_string(x_advance) + 
// 					" anchor: " + glm::to_string(anchor)<< std::endl;

// 		double x_pos = anchor.x + x_offset;
// 		double y_pos = anchor.y;
		
// 		draw_glyph(glyphid, color, x_pos*0.2f, y_pos*0.2f, w, h);

// 		anchor.x += x_advance;
// 	}

// 	if (anchor_out) *anchor_out = anchor;
// }





static Load< void > setup_buffers(LoadTagDefault, [](){
	//you may recognize this init code from DrawSprites.cpp:

	{ //set up vertex buffer:
		glGenBuffers(1, &vertex_buffer);
		//for now, buffer will be un-filled.
	}

	{ //vertex array mapping buffer for color_program:
		//ask OpenGL to fill vertex_buffer_for_color_program with the name of an unused vertex array object:
		glGenVertexArrays(1, &vertex_buffer_for_color_program);

		//set vertex_buffer_for_color_program as the current vertex array object:
		glBindVertexArray(vertex_buffer_for_color_program);

		//set vertex_buffer as the source of glVertexAttribPointer() commands:
		glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);

		//set up the vertex array object to describe arrays of PongMode::Vertex:
		glVertexAttribPointer(
			color_program->Position_vec4, //attribute
			3, //size
			GL_FLOAT, //type
			GL_FALSE, //normalized
			sizeof(DrawTexts::Vertex), //stride
			(GLbyte *)0 + offsetof(DrawTexts::Vertex, Position) //offset
		);
		glEnableVertexAttribArray(color_program->Position_vec4);
		//[Note that it is okay to bind a vec3 input to a vec4 attribute -- the w component will be filled with 1.0 automatically]

		glVertexAttribPointer(
			color_program->Color_vec4, //attribute
			4, //size
			GL_UNSIGNED_BYTE, //type
			GL_TRUE, //normalized
			sizeof(DrawTexts::Vertex), //stride
			(GLbyte *)0 + offsetof(DrawTexts::Vertex, Color) //offset
		);
		glEnableVertexAttribArray(color_program->Color_vec4);

		//done referring to vertex_buffer, so unbind it:
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		//done setting up vertex array object, so unbind it:
		glBindVertexArray(0);
	}

	GL_ERRORS(); //PARANOIA: make sure nothing strange happened during setupetup
});


DrawTexts::DrawTexts(glm::mat4 const &world_to_clip_) : world_to_clip(world_to_clip_) {
    ft_error = FT_Init_FreeType( &library );
	if (ft_error) { std::runtime_error("Freetype library error: " + std::to_string(ft_error));}

	ft_error = FT_New_Face(library,
				data_path("blue-eyes.otf").c_str(),
				0,
				&face );
	if (ft_error == FT_Err_Unknown_File_Format) {	
		throw std::runtime_error("The font file could be opened and read, but it appears that its font format is unsupported");
	} else if (ft_error) { 
		throw std::runtime_error("Another error code means that the font file could not be opened or read, or that it is broken.");
	}
	assert(face != nullptr);

	// ft_error = FT_Set_Char_Size(face, 0, 4, 0, 0);
	ft_error = FT_Set_Pixel_Sizes(face, 0, FONT_SIZE);
	if (ft_error) {
		std::runtime_error("Set char size failed.");
	}
	
	hb_font = hb_ft_font_create_referenced(face);
	hb_buffer = hb_buffer_create();
}


DrawTexts::~DrawTexts() {
	hb_buffer_destroy(hb_buffer);
	hb_font_destroy(hb_font);
	FT_Done_Face (face);
  	FT_Done_FreeType (library);

	//upload vertices to vertex_buffer:
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer); //set vertex_buffer as current
	glBufferData(GL_ARRAY_BUFFER, attribs.size() * sizeof(attribs[0]), attribs.data(), GL_STREAM_DRAW); //upload attribs array
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	//set color_program as current program:
	glUseProgram(color_program->program);

	//upload OBJECT_TO_CLIP to the proper uniform location:
	glUniformMatrix4fv(color_program->OBJECT_TO_CLIP_mat4, 1, GL_FALSE, glm::value_ptr(world_to_clip));

	//use the mapping vertex_buffer_for_color_program to fetch vertex data:
	glBindVertexArray(vertex_buffer_for_color_program);

	//run the OpenGL pipeline:
	glDrawArrays(GL_TRIANGLES, 0, GLsizei(attribs.size()));

	//reset vertex array to none:
	glBindVertexArray(0);

	//reset current program to none:
	glUseProgram(0);
}


// Draws a glyph
void DrawTexts::draw_glyph(hb_codepoint_t glyphid, 
							glm::u8vec4 color, 
							float x_pos, 
							float y_pos,
							glm::vec3 const &x, 
							glm::vec3 const &y,  
							float w,
							float h) {
	char glyphname[32];
	hb_font_get_glyph_name(hb_font, glyphid, glyphname, sizeof(glyphname));
	printf("%d %s x_pos=%f y_pos=%f w=%f h=%f\n", glyphid, glyphname, x_pos, y_pos, w, h);

	attribs.emplace_back(x * glm::vec3(x_pos, 0.0f, 0.0f) + 
						y * glm::vec3(0.0f, y_pos + h, 0.0f), color);
	attribs.emplace_back(x * glm::vec3(x_pos, 0.0f, 0.0f) + 
						y * glm::vec3(0.0f, y_pos, 0.0f), color);
	attribs.emplace_back(x * glm::vec3(x_pos + w, 0.0f, 0.0f) + 
						y * glm::vec3(0.0f, y_pos, 0.0f), color);

	attribs.emplace_back(x * glm::vec3(x_pos, 0.0f, 0.0f) + 
						y * glm::vec3(0.0f, y_pos + h, 0.0f), color);
	attribs.emplace_back(x * glm::vec3(x_pos + w, 0.0f, 0.0f) + 
						y * glm::vec3(0.0f, y_pos, 0.0f), color);
	attribs.emplace_back(x * glm::vec3(x_pos + w, 0.0f, 0.0f) + 
						y * glm::vec3(0.0f, y_pos + h, 0.0f), color);
}


void DrawTexts::draw_texts(std::string const &text, 
						glm::vec3 const &anchor_in, 
						glm::vec3 const &x, 
						glm::vec3 const &y,
						glm::u8vec4 const &color, 
						glm::vec3 *anchor_out) {
	/* harfbuzz code quoted 
	* from https://github.com/harfbuzz/harfbuzz-tutorial/blob/master/hello-harfbuzz-freetype.c
	*/
	glm::vec3 anchor = anchor_in;

	std::cout << "trying to draw: " << text << std::endl;

	hb_buffer_reset(hb_buffer);
	hb_buffer_add_utf8(hb_buffer, text.data(), -1, 0, -1);
	hb_buffer_set_direction(hb_buffer, HB_DIRECTION_LTR);
	hb_buffer_set_script(hb_buffer, HB_SCRIPT_LATIN);
	hb_buffer_set_language(hb_buffer, hb_language_from_string("en", -1));

	/* Shape it! */
	hb_shape (hb_font, hb_buffer, NULL, 0);

	/* Get glyph information and positions out of the buffer. */
	unsigned int glyph_count = hb_buffer_get_length (hb_buffer);
	hb_glyph_info_t *glyph_info    = hb_buffer_get_glyph_infos(hb_buffer, nullptr);
	hb_glyph_position_t *glyph_pos = hb_buffer_get_glyph_positions(hb_buffer, nullptr);

	// Iterate over each glyph
	for (unsigned int i = 0; i < glyph_count; ++i) {
		hb_codepoint_t glyphid = glyph_info[i].codepoint;

		ft_error = FT_Load_Glyph(face, glyphid, FT_LOAD_DEFAULT);
		if (ft_error) throw std::runtime_error("Error on FT_Load_Glyph()");
		ft_error = FT_Render_Glyph(face->glyph, FT_RENDER_MODE_NORMAL);
		if (ft_error) throw std::runtime_error("Error on FT_Render_Glyph()");

		auto x_offset = glyph_pos[i].x_offset / 64.0f;
		auto y_offset = glyph_pos[i].y_offset / 64.0f;
		auto x_advance = glyph_pos[i].x_advance / 64.0f;
		auto w = x_advance;
		auto h = x_advance;

		std::cout << "x_offset: " + std::to_string(x_offset) + 
					" x_advance: " + std::to_string(x_advance) + 
					" anchor: " + glm::to_string(anchor)<< std::endl;

		auto x_pos = anchor.x + x_offset;
		auto y_pos = anchor.y + y_offset;
		
		draw_glyph(glyphid, color, x_pos, y_pos, x, y, w, h);

		anchor.x += x_advance;
	}

	if (anchor_out) *anchor_out = anchor;

	// glm::vec3 anchor = anchor_in;

	// uint32_t start = 0;
	// while (start < text.size()) {
	// 	uint32_t end = start;
	// 	uint32_t glyph = -1U;
	// 	while (end < text.size()) {
	// 		end += 1;
	// 		auto f = PathFont::font.glyph_map.find(text.substr(start, end-start));
	// 		if (f == PathFont::font.glyph_map.end()) {
	// 			end -= 1;
	// 			break;
	// 		}
	// 		glyph = f->second;
	// 	}
	// 	if (glyph == -1U) {
	// 		assert(start == end);
	// 		end += 1;
	// 		//missing! draw a tofu:
	// 		for (const auto &pt : {
	// 			glm::vec2(0.1f, 0.1f), glm::vec2(0.6f, 0.1f),
	// 			glm::vec2(0.6f, 0.1f), glm::vec2(0.6f, 0.9f),
	// 			glm::vec2(0.9f, 0.6f), glm::vec2(0.1f, 0.9f),
	// 			glm::vec2(0.1f, 0.9f), glm::vec2(0.1f, 0.1f)
	// 		}) {
	// 			attribs.emplace_back(anchor + pt.x * x + pt.y * y, color);
	// 		}
	// 		anchor += x * 0.6f;
	// 	} else {
	// 		for (uint32_t c = PathFont::font.glyph_coord_starts[glyph]; c + 1 < PathFont::font.glyph_coord_starts[glyph+1]; c += 2) {
	// 			attribs.emplace_back(anchor + glm::vec3(0.2f, 0.0f, 0.0f) * PathFont::font.coords[c] + glm::vec3(0.0f, 0.2f, 0.0f) * PathFont::font.coords[c+1], color);
	// 			std::cout << text.substr(start, end-start) + " anchor: " + glm::to_string(anchor) + 
	// 					" position: " + glm::to_string(anchor + x * PathFont::font.coords[c] + y * PathFont::font.coords[c+1]) << std::endl;
	// 		}
	// 		anchor += x * PathFont::font.glyph_widths[glyph];
	// 		std::cout << "width: " + std::to_string(PathFont::font.glyph_widths[glyph]) << std::endl;
	// 	}
	// 	start = end;
	// }

	// if (anchor_out) *anchor_out = anchor;
}