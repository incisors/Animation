#version 330 core

// Inputs to the fragment shader are the outputs of the same name from the vertex shader.
// Note that you do not have access to the vertex shader's default output, gl_Position.

in vec3 fragNormal;

// uniforms used for lighting
uniform vec3 AmbientColor = vec3(0.2);
uniform vec3 LightDirection1 = normalize(vec3(1, 5, 2));
uniform vec3 LightColor1 = vec3(0.7,1,0.2);
uniform vec3 DiffuseColor;	// passed in from c++ side NOTE: you can also set the value here and then remove 
							// color from the c++ side
uniform vec3 LightColor2 = vec3(1,0, 0.4);
uniform vec3 LightDirection2 = normalize(vec3(-1, -5, 3));


// You can output many things. The first vec4 type output determines the color of the fragment
out vec4 fragColor;

void main()
{

	// Compute irradiance (sum of ambient & direct lighting)
	vec3 irradiance = AmbientColor + LightColor1 * max(0, dot(LightDirection1, fragNormal));
	irradiance += LightColor2 * max(0, dot(LightDirection2, fragNormal));

	// Diffuse reflectance
	vec3 reflectance = irradiance * DiffuseColor;

	// Gamma correction
	fragColor = vec4(sqrt(reflectance), 1);
}