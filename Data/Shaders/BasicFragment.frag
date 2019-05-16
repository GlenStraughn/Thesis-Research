// Basic Fragment Shader
// Glen Straughn
// 09.10.2017

uniform vec4 u_diffColor;


//varying vec3 v_normal;
varying float v_originDistance;

void main()
{
	
	gl_FragColor = u_diffColor*(1.0 - v_originDistance);
	gl_FragColor.a = 1.0;
}