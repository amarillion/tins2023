#version 130

// derived from OpenGL Dev Cookbook Ch 3. 
// https://github.com/bagobor/opengl33_dev_cookbook_2013/blob/master/Chapter3/TwirlFilter/TwirlFilter/shaders/shader.vert

precision mediump float;

out vec4 vFragColor;	//fragment shader output

//input from the vertex shader
smooth in vec2 vUV;						//2D texture coordinates

//shader uniforms
uniform sampler2D textureMap;			//the image to twirl
uniform float time;				        //teh amount of twirl

void main()
{
	vec2 uv = vUV;

    float frequency = 20.0;
    float amplitude = 4.0;
    float speed = 6.282 / 10;

    vec4 color = texture(textureMap, uv);
    if (color == vec4(1.0, 0.0, 1.0, 1.0)) {
    	//vec2 h = vec2(
         //       sin(  (length( uv ) * frequency ) + ( time * speed ) ),
          //      cos(  (length( uv ) * frequency ) + ( time * speed ) )
          //  // Scale amplitude to make input more convenient for users
        //);

    	float h = 0.5 + 0.5
    	    * sin((0.3 * uv.x + 0.5 * uv.y + 0.1 * time * speed) * frequency)
    	    * sin((0.5 * uv.x + 1.0 * uv.y - 0.2 * time * speed) * frequency * 2)
    	    * sin((0.8 * uv.x - 1.0 * uv.y + 0.3 * time * speed) * frequency);

        if (h > 0.24 && h < 0.26) {
    	    vFragColor = vec4(1.0, 1.0, 1.0, 1.0);
        }
        else if (h > 0.68 && h < 0.70) {
    	    vFragColor = vec4(1.0, 0.0, 1.0, 1.0);
        }
    	else if (h > 0.54 && h < 0.56) {
    	    vFragColor = vec4(0.0, 1.0, 1.0, 1.0);
        }
        else {
            vFragColor = vec4(0, 0, 0, 1.0);
        }
   }
   else {
        vFragColor = color;
   }
}