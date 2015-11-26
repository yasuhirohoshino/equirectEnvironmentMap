#version 400

#define PI 3.14159265358979
#define TwoPI 6.28318530718

uniform sampler2D envMap;
uniform sampler2D tex;

in vec4 colorVarying;
in vec2 texCoordVarying;

in vec3 normalVarying;
in vec4 positionVarying;
in vec3 v_normalVarying;
in vec4 v_positionVarying;

in vec3 reflectVec;

out vec4 fragColor;

vec2 envMapEquirect(vec3 dir) {
    float phi = acos(-dir.y);
    float theta = atan(-1.0 * dir.x, dir.z) + PI;
    return vec2(theta / TwoPI, phi / PI);
}

void main (void) {
    fragColor = vec4(texture(envMap, envMapEquirect(reflectVec)).rgb, 1.0);
}