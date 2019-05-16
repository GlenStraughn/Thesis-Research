// Basic Vertex Shader
// Glen Straughn
// 09.10.2017



uniform mat4x4 u_pose;
uniform mat4x4 u_perspective;

attribute vec3 a_position;
attribute vec3 a_normal;

varying vec4 v_normal;
varying vec3 v_position;

void main()
{
	mat4x4 transform = u_perspective * u_pose;
	vec4 position = transform * vec4(a_position, 1.0);
	
	v_normal =  transform * vec4(a_normal, 0.0);
	v_position = position.xyz;
	
	gl_Position = position;
}