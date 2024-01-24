#version 450

in vec2 v_texCoord;
in vec2 vcolor;

out vec4 color;

uniform sampler2D u_tex;

void main() {
	color = texture(u_tex, v_texCoord);
	//color = vec4(vcolor, 0.0, 1.0);
} 