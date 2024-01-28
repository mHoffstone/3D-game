#version 460 core

out vec4 color;

in vec3 v_position;
in vec2 v_texCoords;
in float v_texIndex;

uniform sampler2DArray u_sampler;
uniform vec3 u_translation;
uniform float u_time;

void main(){
	vec4 texel = texture(u_sampler, vec3(v_texCoords, v_texIndex));
	float variation = ((
		pow((
			sin(sin(v_texCoords.x) + sin(v_texCoords.y) + u_time)+
			sin(2*sin(0.1*v_texCoords.x) + 2*sin(0.1*v_texCoords.y) + u_time)
		)*0.5f, 2)
	)+1.0f)*0.2f+0.9f;
	/*float variation = ((
		pow((
			sin(100*v_texCoords.x + 2*sin(10*v_texCoords.y) + u_time)+
			sin(200*v_texCoords.x + 2*sin( 7*v_texCoords.y) + sin(u_time))
		)*0.5f, 2)
	)+1.0f)*0.2f+0.9f;*/
	/*float variation = ((
		sin(200*v_texCoords.x + u_time)+
		sin(400*v_texCoords.x + u_time)+
		sin(200*v_texCoords.x+100*v_texCoords.y + 2*u_time)+
		sin(50*v_texCoords.x+50*v_texCoords.y + 0.5f*u_time)
	)/3.0f+1)*0.2f+0.9f;*/
	float intensity = 1.0f - pow(dot(normalize(u_translation-v_position), vec3(0.0f, 1.0f, 0.0f)), 2)*0.9f;
	color = vec4(vec3(0.0f, 0.5f, 0.6f) * variation, intensity);
}
