#version 130

attribute vec4 al_pos;
attribute vec4 al_color;
attribute vec2 al_texcoord;
uniform mat4 al_projview_matrix;
varying vec2 varying_texcoord;
varying vec2 pixel_position;	// screen coordinates

void main()
{
   varying_texcoord = al_texcoord;
   pixel_position = al_pos.xy;
   gl_Position = al_projview_matrix * al_pos;
}
