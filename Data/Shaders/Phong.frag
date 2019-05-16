// Basic Fragment Shader
// Glen Straughn
// 09.10.2017

uniform vec4 u_diffColor;
uniform vec3 u_lightDirection;
uniform float u_lightIntensity;

uniform float u_shinyFactor;
uniform float u_ambientFactor;

varying vec4 v_normal;
varying vec3 v_position;

const vec4 EYE_POSITION = vec4(0.0, 0.0, 0.0, 1.0); // Position of the eye in virtual space

vec4 lambertian(vec3 L, vec3 N)
{
	vec4 lamb = u_diffColor * max(dot(N,L), 0.0);
	lamb = clamp(lamb, 0.0, 1.0);
	return lamb;
}

float specular(vec3 L, vec3 N)
{
	vec3 R = normalize( -reflect(L, N) );
	vec3 V = normalize(-v_position);
	
	float dp = max(dot(R, V), 0.0);
	float spec = pow(dp, u_shinyFactor);
	spec = clamp(spec, 0.0, 1.0);
	return spec;
}

void main()
{
	vec3 L = normalize(u_lightDirection);
	vec3 N = normalize(v_normal.xyz);
	
	vec4 lambertianTerm = lambertian(L, N);
	lambertianTerm.a = 1.0;
	float highlight = specular(L, N);
	
	gl_FragColor = u_lightIntensity*(lambertianTerm + highlight) + (u_ambientFactor*u_diffColor);
}