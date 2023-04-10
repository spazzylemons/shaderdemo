#version 330 core

in vec3 vertexPosition;
in vec3 vertexNormal;
in vec2 vertexTexCoord;
in vec4 vertexColor;

uniform mat4 mvp;
uniform mat4 matModel;
uniform mat4 matNormal;

uniform bool applyModel;

out vec3 fragPosition;
out vec3 fragNormal;
out vec2 fragTexCoord;
out vec4 fragColor;

void main() {
    // pass parameters to fragment shader
    fragTexCoord = vertexTexCoord;
    fragColor = vertexColor;
    // project position
    gl_Position = mvp * vec4(vertexPosition, 1.0);
    // send fragment shader the location of the vertex in model or world space
    if (applyModel) {
        fragPosition = (matModel * vec4(vertexPosition, 1.0)).xyz;
    } else {
        fragPosition = vertexPosition;
    }
    // send fragment shader the vertex normal for shading
    fragNormal = normalize(vec3(matNormal * vec4(vertexNormal, 1.0)));
}
