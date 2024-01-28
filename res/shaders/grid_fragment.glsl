#version 460 core

out vec4 color;

in vec3 v_position;
in vec3 v_normal;
in vec2 v_texCoords;
in float v_texIndex;

uniform sampler2DArray u_sampler;
uniform vec3 u_translation;

void main(){
	float lightA = (-dot(v_normal, normalize(vec3(-1.0f, -2.0f, -1.5f))))*0.4f + 0.6f;

	vec3 lightPos = vec3(-0.5f, 5.5f, -0.5f);
	vec3 lightDir = normalize(v_position - lightPos);

	float lightD = (-dot(v_normal, lightDir)) * 4.0f/(pow(length(lightDir), 2.0f)+2.0f);
	lightD = max(lightD, 0.0f);

	vec3 viewDir = normalize(u_translation - v_position);
	vec3 reflectDir = reflect(lightDir, v_normal);
	float lightS = pow(max(dot(viewDir, reflectDir), 0.0f), 16);

	float intensity = 1.0f;

	vec4 texel = texture(u_sampler, vec3(v_texCoords, v_texIndex));
	if(texel.a < 0.9) discard;
	color = vec4(texel.rgb * (lightA * 1.0f + intensity*(lightD * 0.0f + lightS * 0.0f)), texel.a);
}
