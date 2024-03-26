#version 330 core

// Inputs to the fragment shader are the outputs of the same name from the vertex shader.
// Note that you do not have access to the vertex shader's default output, gl_Position.

in vec3 fragNormal;
in vec3 diffuseColor;

// uniforms used for lighting
uniform vec3 AmbientColor = vec3(0.2);
uniform vec3 LightDirection = normalize(vec3(1, 5, 2));
uniform vec3 LightColor = vec3(1.0, 1.0, 0.0);

uniform vec3 LightDirection_2 = normalize(vec3(-1, -5, -2));
uniform vec3 LightColor_2 = vec3(0.0, 0.0, 1.0);

// You can output many things. The first vec4 type output determines the color of the fragment
out vec4 fragColor;

void main()
{

	// Compute irradiance (sum of ambient & direct lighting)
	vec3 irradiance = AmbientColor + LightColor * max(0, dot(LightDirection, fragNormal));
    irradiance += LightColor_2 * max(0, dot(LightDirection_2, fragNormal));

	// Diffuse reflectance
	vec3 reflectance = irradiance * diffuseColor;

	// Gamma correction
	fragColor = vec4(sqrt(reflectance), 1);
    //fragColor = vec4(fragNormal, 1);
}