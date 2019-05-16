// Backdrop Fragment Shader
// Glen Straughn
// 10.11.2017

//#version 110

uniform sampler2D u_texture;
uniform vec3 u_debugColor;

varying vec2 v_uvCoordinates;

void main()
{
	//gl_FragColor = vec4(u_debugColor, 1.0);
	gl_FragColor = vec4(texture2D(u_texture, v_uvCoordinates).xyz, 1.0);
}