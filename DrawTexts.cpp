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

	// disable byte-alignment restriction
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	// load first 128 characters of ASCII set
	for (unsigned char c = 0; c < 128; c++) {
		// Load character glyph 
		if (FT_Load_Char(face, c, FT_LOAD_RENDER))
		{
			std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
			continue;
		}
		// generate texture
		unsigned int texture;
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexImage2D(
			GL_TEXTURE_2D,
			0,
			GL_RED,
			face->glyph->bitmap.width,
			face->glyph->bitmap.rows,
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
		Character character = {
			texture,
			glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
			glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
			static_cast<unsigned int>(face->glyph->advance.x)
		};
		Characters.insert(std::pair<char, Character>(c, character));
	}
	glBindTexture(GL_TEXTURE_2D, 0);

	FT_Done_Face (face);
  	FT_Done_FreeType (library);

}


DrawTexts::~DrawTexts() {
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


void DrawTexts::draw_texts(std::string const &text, 
							glm::vec3 anchor, 
							float scale,
							glm::u8vec4 const &color) {
	/* harfbuzz code quoted 
	* from https://github.com/harfbuzz/harfbuzz-tutorial/blob/master/hello-harfbuzz-freetype.c
	*/

	std::cout << "trying to draw: " << text << std::endl;

	// iterate through all characters
    std::string::const_iterator c;
    for (c = text.begin(); c != text.end(); c++) 
    {
        Character ch = Characters[*c];

        float x_pos = anchor.x + ch.Bearing.x * scale;
        float y_pos = anchor.y - (ch.Size.y - ch.Bearing.y) * scale;

        float w = ch.Size.x * scale;
        float h = ch.Size.y * scale;

		printf("%c, x_pos=%f y_pos=%f w=%f h=%f\n", *c, x_pos, y_pos, w, h);
        
		attribs.emplace_back(glm::vec3(x_pos, 0.0f, 0.0f) + 
						 glm::vec3(0.0f, y_pos + h, 0.0f), color);
		attribs.emplace_back(glm::vec3(x_pos, 0.0f, 0.0f) + 
							glm::vec3(0.0f, y_pos, 0.0f), color);
		attribs.emplace_back(glm::vec3(x_pos + w, 0.0f, 0.0f) + 
							glm::vec3(0.0f, y_pos, 0.0f), color);

		attribs.emplace_back(glm::vec3(x_pos, 0.0f, 0.0f) + 
							glm::vec3(0.0f, y_pos + h, 0.0f), color);
		attribs.emplace_back(glm::vec3(x_pos + w, 0.0f, 0.0f) + 
							glm::vec3(0.0f, y_pos, 0.0f), color);
		attribs.emplace_back(glm::vec3(x_pos + w, 0.0f, 0.0f) + 
							glm::vec3(0.0f, y_pos + h, 0.0f), color);
        // now advance cursors for next glyph (note that advance is number of 1/64 pixels)
        anchor.x += (ch.Advance >> 6) * scale; // bitshift by 6 to get value in pixels (2^6 = 64 (divide amount of 1/64th pixels by 64 to get amount of pixels))
    }
}