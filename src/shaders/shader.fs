#version 330 core
in vec3 vertexColor;

out vec4 FragColor;

uniform vec3 lightColor;

void main()
{
    FragColor = vec4(vertexColor * lightColor, 1.0f);
}