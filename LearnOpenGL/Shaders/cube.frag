#version 410 core
in vec3 vNormal;
in vec3 vColor;
out vec4 FragColor;

uniform vec3 uLightDir;

void main() {
    vec3 n = normalize(vNormal);
    float diff = max(dot(n, normalize(-uLightDir)), 0.0);
    vec3 c = vColor * (0.3 + 0.7 * diff);
    FragColor = vec4(c, 1.0);
}
