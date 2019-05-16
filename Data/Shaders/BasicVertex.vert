// Basic Vertex Shader
// Glen Straughn
// 09.10.2017



uniform mat4x4 u_pose;
uniform mat4x4 u_perspective;

attribute vec3 a_position;
//attribute vec3 a_normal;

varying float v_originDistance;

void main()
{
/*
	if(sum == 0.0)
	{
		error = 1.0;
		gl_Position = vec4(a_position, 1.0);
	}
	else
	{
		gl_Position = u_pose*vec4(a_position, 1.0);
	}
	*/
	//v_normal = a_normal;
	//gl_Position = vec4(a_position, 1.0); u_perspective*
	
	v_originDistance = distance(vec3(0.0,0.0,0.0), a_position);
	
	gl_Position =  u_perspective*u_pose*vec4(a_position, 1.0);
}