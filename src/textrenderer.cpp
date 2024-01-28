#include "textrenderer.h"

TextRenderer::TextRenderer(){

}

TextRenderer::~TextRenderer(){
	maybeDestroy();
}

void TextRenderer::create(const Texture& fontSheet){
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	createLife();

	vec2f vertices[6] = {{0.0f,0.0f}, {1.0f,0.0f}, {1.0f,1.0f},  {0.0f,0.0f}, {1.0f,1.0f}, {0.0f,1.0f}};
	glBufferData(GL_ARRAY_BUFFER, 6*2*sizeof(float), &vertices[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2*sizeof(float), (void*)(0));

	program.createProgram();
	Shader vertex(GL_VERTEX_SHADER, util::readFile("res/shaders/text_vertex.glsl"));
	Shader fragment(GL_FRAGMENT_SHADER, util::readFile("res/shaders/text_fragment.glsl"));
	program.setShaders({&vertex, &fragment});
	program.useProgram();

	translation.setLocation(program, "u_translation");
	vpSize.setLocation(program, "u_vpSize");
	textUniform.setLocation(program, "u_text");
	sampler.setLocation(program, "u_sampler");

	fontTexture = fontSheet;
	fontTexture.create(GL_NEAREST, GL_CLAMP_TO_EDGE);
	fontTexture.apply();

	hasCreated = true;
}

void TextRenderer::render(Renderer& r, const std::string& text, vec2f pos){
	if(!hasCreated) throw std::runtime_error("TextRenderer was not created!");
	glDisable(GL_DEPTH_TEST);
	program.useProgram();
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);

	glUniform2f(translation.getLocation(), pos.x, pos.y);
	glUniform2f(vpSize.getLocation(), r.getViewportWidth(), r.getViewportHeight());
	int textData[128];
	for(int i = 0; i < 128; i++) textData[i] = i < (int)text.size() ? text[i] : ' ';
	glUniform1iv(textUniform.getLocation(), 128, &textData[0]);
	fontTexture.bindTexture();
	glDrawArrays(GL_TRIANGLES, 0, 6);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glEnable(GL_DEPTH_TEST);
}

void TextRenderer::destroy(){
	glBindVertexArray(vao);
	glDeleteBuffers(1, &vbo);
	glDeleteVertexArrays(1, &vao);
}

