#version 330 core
out vec4 FragColor;

in VS_OUT
{
	vec3 FragPos;
	vec3 Normal;
	vec2 TexCoords;
} fs_in;

uniform sampler2D diffuseTexture;
uniform samplerCube shadowMap;

uniform vec3 lightPos;
uniform vec3 viewPos;

uniform float far_plane;
uniform bool shadows;
uniform bool lightCube;

vec3 sampleOffsetDirections[20] = vec3[]
(
	vec3(1, 1, 1), vec3(1, -1, 1), vec3(-1, -1, 1), vec3(-1, 1, 1),
	vec3(1, 1, -1), vec3(1, -1, -1), vec3(-1, -1, -1), vec3(-1, 1, -1),
	vec3(1, 1, 0), vec3(1, -1, 0), vec3(-1, -1, 0), vec3(-1, 1, 0),
	vec3(1, 0, 1), vec3(-1, 0, 1), vec3(1, 0, -1), vec3(-1, 0, -1),
	vec3(0, 1, 1), vec3(0, -1, 1), vec3(0, -1, -1), vec3(0, 1, -1)
);

float ShadowCalculations(vec3 fragPos)
{
	vec3 fragToLight = fragPos - lightPos;
	
	//float closestDepth = texture(shadowMap, fragToLight).r;
	//closestDepth *= far_plane;
	float currentDepth = length(fragToLight);

	// Shadow Test
	//float bias = 0.05;
	//float shadow = currentDepth - bias > closestDepth ? 1.0 : 0.0;

	// PCF
	/*float shadow = 0.0;
	float bias = 0.05;
	float samples = 4.0;
	float offset = 0.1;
	for (float x = -offset; x < offset; x += offset / (samples * 0.5))
	{
		for (float y = -offset; y < offset; y += offset / (samples * 0.5))
		{
			for (float z = -offset; z < offset; z += offset / (samples * 0.5))
			{
				float closestDepth = texture(shadowMap, fragToLight + vec3(x, y, z)).r;
				closestDepth *= far_plane;
				if (currentDepth - bias > closestDepth)
					shadow += 1.0;
			}
		}
	}
	shadow /= (samples * samples * samples);*/

	float shadow = 0.0;
	float bias = 0.15;
	int samples = 20;
	float viewDistance = length(viewPos - fragPos);
	float diskRadius = (1.0 + (viewDistance / far_plane)) / 25.0;
	for (int i = 0; i < samples; ++i)
	{
		float closestDepth = texture(shadowMap, fragToLight + sampleOffsetDirections[i] * diskRadius).r;
		closestDepth *= far_plane;
		if (currentDepth - bias > closestDepth)
			shadow += 1.0;
	}
	shadow /= float(samples);

	// Shadow = 0.0 when light far_plane > 1.0
	//if (projCoords.z > 1.0)
	//	shadow = 0.0;

	//FragColor = vec4(vec3(closestDepth / far_plane), 1.0);

	return shadow;
}

void main()
{
	if (!lightCube)
	{
		vec3 color = texture(diffuseTexture, fs_in.TexCoords).rgb;
		vec3 normal = normalize(fs_in.Normal);
		vec3 lightColor = vec3(1.0);

		vec3 ambient = 0.3 * color;

		vec3 lightDir = normalize(lightPos - fs_in.FragPos);
		float diff = max(dot(lightDir, normal), 0.0);
		vec3 diffuse = diff * lightColor;

		vec3 viewDir = normalize(viewPos - fs_in.FragPos);
		vec3 halfwayDir = normalize(lightDir + viewDir);
		float spec = pow(max(dot(normal, halfwayDir), 0.0), 64.0);
		vec3 specular = spec * lightColor;

		float shadow = shadows ? ShadowCalculations(fs_in.FragPos) : 0.0;
		vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular)) * color;

		FragColor = vec4(lighting, 1.0);
	}
	else
		FragColor = vec4(1.0, 0.9, 0.2, 1.0);
};