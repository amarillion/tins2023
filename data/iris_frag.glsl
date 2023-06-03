#version 130

// derived from OpenGL Dev Cookbook Ch 3. 
// https://github.com/bagobor/opengl33_dev_cookbook_2013/blob/master/Chapter3/TwirlFilter/TwirlFilter/shaders/shader.vert

precision mediump float;

//input from the vertex shader
//smooth in vec2 vUV;						//2D texture coordinates
//uniform sampler2D textureMap;			//reflection map

uniform sampler2D al_tex;               // default texture, set by al_draw_bitmap
uniform float time;				        // waves move over time...

varying vec2 varying_texcoord;
varying vec2 pixel_position;

void main()
{
	vec2 uv = gl_FragCoord.xy;          // gl_FragCoord coordinate relative to viewport, not texture.

    float waveLen = 20;
    float speed = 6.282 * 25;
    vec4 color = texture(al_tex, varying_texcoord);
    float alpha = color.a;

    if (color.r == color.b && color.r > 0 && color.g == 0.0) {


    	float h = 0.5 + 0.5
    	    * sin((0.3 * uv.x + 0.5 * uv.y + 0.1 * time * speed) / waveLen)
    	    * sin((0.5 * uv.x + 1.0 * uv.y - 0.2 * time * speed) / waveLen * 2)
    	    * sin((0.8 * uv.x - 1.0 * uv.y + 0.3 * time * speed) / waveLen);

        if (h > 0.24 && h < 0.26) {
    	    gl_FragColor = vec4(1.0, 1.0, 1.0, alpha);
        }
        else if (h > 0.68 && h < 0.70) {
    	    gl_FragColor = vec4(1.0, 0.0, 1.0, alpha);
        }
    	else if (h > 0.54 && h < 0.56) {
    	    gl_FragColor = vec4(0.0, 1.0, 1.0, alpha);
        }
        else {
            gl_FragColor = vec4(0, 0, 0, alpha);
        }
   }
   else {
        gl_FragColor = color;
   }
}