// Backdrop Vertex Shader
// Glen Straughn
// 10.11.2017

//#version 110

attribute vec3 a_position;

varying vec2 v_uvCoordinates;

void main()
{
	v_uvCoordinates = vec2(1.0,1.0) - (a_position.xy + vec2(1.0,1.0))/2.0;
	v_uvCoordinates.x = -v_uvCoordinates.x;
	
	gl_Position = vec4(a_position, 1.0);
}