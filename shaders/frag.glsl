#version 410 core
in vec2 vUV;
out vec4 fragColor;
uniform sampler2D uImage;

void main() {
    vec3 c = texture(uImage, vec2(vUV.x, 1.0 - vUV.y)).rgb;
    c = c / (c + 1.0);                  // Reinhard tonemap
    c = pow(c, vec3(1.0 / 2.2));        // approximate sRGB
    fragColor = vec4(c, 1.0);
}