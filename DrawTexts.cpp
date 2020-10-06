#include "DrawTexts.hpp"
#include "ColorTextureProgram.hpp"
#include "data_path.hpp"
#include "PathFont.hpp"
#include "gl_errors.hpp"
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>

//n.b. declared static so they don't conflict with similarly named global variables elsewhere:
// static GLuint vertex_buffer = 0;
// static GLuint vertex_buffer_for_color_texture_program = 0;
static GLuint VAO = 0;
static GLuint VBO = 0;

// static Load< void > setup_buffers(LoadTagDefault, [](){
// 	//you may recognize this init code from PongMode.cpp in base0:

// 	{ //set up vertex buffer:
// 		glGenBuffers(1, &vertex_buffer);
// 		//for now, buffer will be un-filled.
// 	}

// 	{ //vertex array mapping buffer for color_texture_program:
// 		//ask OpenGL to fill vertex_buffer_for_color_texture_program with the name of an unused vertex array object:
// 		glGenVertexArrays(1, &vertex_buffer_for_color_texture_program);

// 		//set vertex_buffer_for_color_texture_program as the current vertex array object:
// 		glBindVertexArray(vertex_buffer_for_color_texture_program);

// 		//set vertex_buffer as the source of glVertexAttribPointer() commands:
// 		glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);

// 		//set up the vertex array object to describe arrays of PongMode::Vertex:
// 		glVertexAttribPointer(
// 			color_texture_program->Position_vec4, //attribute
// 			2, //size
// 			GL_FLOAT, //type
// 			GL_FALSE, //normalized
// 			sizeof(DrawTexts::Vertex), //stride
// 			(GLbyte *)0 + offsetof(DrawTexts::Vertex, Position) //offset
// 		);
// 		glEnableVertexAttribArray(color_texture_program->Position_vec4);
// 		//[Note that it is okay to bind a vec3 input to a vec4 attribute -- the w component will be filled with 1.0 automatically]

// 		glVertexAttribPointer(
// 			color_texture_program->TexCoord_vec2, //attribute
// 			2, //size
// 			GL_FLOAT, //type
// 			GL_FALSE, //normalized
// 			sizeof(DrawTexts::Vertex), //stride
// 			(GLbyte *)0 + offsetof(DrawTexts::Vertex, TexCoord) //offset
// 		);
// 		glEnableVertexAttribArray(color_texture_program->TexCoord_vec2);

// 		glVertexAttribPointer(
// 			color_texture_program->Color_vec4, //attribute
// 			4, //size
// 			GL_UNSIGNED_BYTE, //type
// 			GL_TRUE, //normalized
// 			sizeof(DrawTexts::Vertex), //stride
// 			(GLbyte *)0 + offsetof(DrawTexts::Vertex, Color) //offset
// 		);
// 		glEnableVertexAttribArray(color_texture_program->Color_vec4);

// 		//done referring to vertex_buffer, so unbind it:
// 		glBindBuffer(GL_ARRAY_BUFFER, 0);

// 		//done setting up vertex array object, so unbind it:
// 		glBindVertexArray(0);
// 	}

// 	GL_ERRORS(); //PARANOIA: make sure nothing strange happened during setup
// });


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


void DrawTexts::draw_texts(std::string const &text, 
							glm::vec3 anchor, 
							float scale,
							glm::u8vec4 const &color) {
	/* harfbuzz code quoted 
	* from https://github.com/harfbuzz/harfbuzz-tutorial/blob/master/hello-harfbuzz-freetype.c
	*/

//	std::cout << "trying to draw: " << text << std::endl;

	hb_buffer_add_utf8(hb_buffer, text.data(), -1, 0, -1);
	hb_buffer_set_direction(hb_buffer, HB_DIRECTION_LTR);
	hb_buffer_set_script(hb_buffer, HB_SCRIPT_LATIN);
	hb_buffer_set_language(hb_buffer, hb_language_from_string("en", -1));

	/* Shape it! */
	hb_shape (hb_font, hb_buffer, NULL, 0);

	// disable byte-alignment restriction
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	unsigned int glyph_count;
	hb_glyph_info_t* glyph_info = hb_buffer_get_glyph_infos(hb_buffer, &glyph_count);
	hb_glyph_position_t* glyph_pos = hb_buffer_get_glyph_positions(hb_buffer, &glyph_count);

	std::vector<Character> Characters;

	for (size_t i = 0; i < glyph_count; ++i) {
		hb_codepoint_t glyphid = glyph_info[i].codepoint; // the id specifies the specific letter

		// load character glyph
		ft_error = FT_Load_Glyph(face, glyphid, FT_LOAD_DEFAULT);
		if (ft_error) throw std::runtime_error("Error on FT_Load_Glyph()");
		ft_error = FT_Render_Glyph(face->glyph, FT_RENDER_MODE_NORMAL);
		if (ft_error) throw std::runtime_error("Error on FT_Render_Glyph()");

		double x_offset = glyph_pos[i].x_offset / 64.0;
		double y_offset = glyph_pos[i].y_offset / 64.0;
		double x_advance = glyph_pos[i].x_advance / 64.0;
		int w = face->glyph->bitmap.width;
		int h = face->glyph->bitmap.rows;

		// generate texture
		GLuint texture;
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexImage2D(
			GL_TEXTURE_2D,
			0,
			GL_RED,
			w,
			h,
			0,
			GL_RED,
			GL_UNSIGNED_BYTE,
			face->glyph->bitmap.buffer
		);
		// set texture options
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		// now store character for later use
		Characters.emplace_back(texture,
							glm::vec2(w, h),
							glm::vec2(x_offset, y_offset),
							x_advance);
	}
    
    // configure VAO/VBO for texture quads
    // -----------------------------------
	glBindTexture(GL_TEXTURE_2D, 0);
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);


	//set color_program as current program:
	glUseProgram(color_texture_program->program);
	glUniform3f(glGetUniformLocation(color_texture_program->program, "textColor"), color.x, color.y, color.z);
	//upload OBJECT_TO_CLIP to the proper uniform location:
	glUniformMatrix4fv(color_texture_program->OBJECT_TO_CLIP_mat4, 1, GL_FALSE, glm::value_ptr(world_to_clip));
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(VAO);

	// iterate through all characters
    for (size_t i = 0; i < glyph_count; ++i) {
        Character ch = Characters[i];

        float x_pos = anchor.x + ch.Bearing.x * scale;
        float y_pos = anchor.y - (ch.Size.y - ch.Bearing.y) * scale;

        float w = ch.Size.x * scale;
        float h = ch.Size.y * scale;

//		printf("x_pos=%f y_pos=%f w=%f h=%f\n", x_pos, y_pos, w, h);

		// update VBO for each character
        float vertices[6][4] = {
            { x_pos,     y_pos + h,   0.0f, 0.0f },            
            { x_pos,     y_pos,       0.0f, 1.0f },
            { x_pos + w, y_pos,       1.0f, 1.0f },

            { x_pos,     y_pos + h,   0.0f, 0.0f },
            { x_pos + w, y_pos,       1.0f, 1.0f },
            { x_pos + w, y_pos + h,   1.0f, 0.0f }           
        };
        
		// render glyph texture over quad
        glBindTexture(GL_TEXTURE_2D, ch.TextureID);
        // update content of VBO memory
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); // be sure to use glBufferSubData and not glBufferData

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        // render quad
        glDrawArrays(GL_TRIANGLES, 0, 6);
        // now advance cursors for next glyph (note that advance is number of 1/64 pixels)
        anchor.x += ch.Advance * scale; // bitshift by 6 to get value in pixels (2^6 = 64 (divide amount of 1/64th pixels by 64 to get amount of pixels))
    }
	glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
	glUseProgram(0);
}


DrawTexts::~DrawTexts() {
	// destroy FreeType once we're finished
    FT_Done_Face(face);
    FT_Done_FreeType(library);
	// hb_buffer_destroy(hb_buffer);
	// hb_font_destroy(hb_font);
}