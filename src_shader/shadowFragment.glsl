#version 410 core

in float v_lightLevel;

void main() {
    // turn lightlevel into 0-1 based on lightLevel / 15
    float light = v_lightLevel / 15.0f;
    light = clamp(light, 0, 1);

    gl_FragColor = vec4(0, 0, 0, 1);
}