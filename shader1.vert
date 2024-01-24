#version 450

layout (location = 0) in vec2 in_pos;
layout (location = 1) in vec2 in_texCoord;

out vec2 v_texCoord;
out vec2 vcolor;

void main() {
	vcolor = in_pos;
	gl_Position = vec4(in_pos, 0.0, 1.0);
	v_texCoord = vec2(in_texCoord.x, in_texCoord.y);
}