#include <raylib.h>
#include <raymath.h>

#include <stdlib.h>

#define ROTATION_SCALE 0.005f

#define Y_AXIS ((Vector3) { 0.0f, 1.0f, 0.0f })
#define Z_AXIS ((Vector3) { 0.0f, 0.0f, 1.0f })

typedef struct {
    const char *name;
    int type;
    union {
        int integer;
        Vector3 vec3;
    } value;
} ShaderUniform;

typedef struct {
    const char *name;
    const char *filename;
    const ShaderUniform *uniforms;

    bool loaded;
    Shader shader;
} ShaderSettings;

static const ShaderUniform shiftingCracksUniforms[] = {
    { "applyModel", SHADER_UNIFORM_INT, { .integer = 1 } },
    { "crackColor", SHADER_ATTRIB_VEC3, { .vec3 = { 0.0f, 0.0f, 0.0f } } },
    { "surfaceColor", SHADER_ATTRIB_VEC3, { .vec3 = { 1.0f, 1.0f, 1.0f } } },
    { NULL }
};

static const ShaderUniform kintsugiUniforms[] = {
    { "applyModel", SHADER_UNIFORM_INT, { .integer = 0 } },
    { "crackColor", SHADER_ATTRIB_VEC3, { .vec3 = { 1.0f, 0.9f, 0.2f } } },
    { "surfaceColor", SHADER_ATTRIB_VEC3, { .vec3 = { 0.7f, 0.7f, 0.7f } } },
    { NULL }
};

static ShaderSettings shaderSettings[] = {
    { "Shifting Cracks", "kintsugi", shiftingCracksUniforms },
    { "Kintsugi", "kintsugi", kintsugiUniforms },
    { NULL },
};

static void loadSettings(ShaderSettings *settings) {
    if (!settings->loaded) {
        const char *vertex = TextFormat("shaders/%s.vs", settings->filename);
        const char *fragment = TextFormat("shaders/%s.fs", settings->filename);
        Shader shader = LoadShader(vertex, fragment);
        for (const ShaderUniform *uniform = settings->uniforms; uniform->name; uniform++) {
            int loc = GetShaderLocation(shader, uniform->name);
            SetShaderValue(shader, loc, &uniform->value, uniform->type);
        }
        settings->loaded = true;
        settings->shader = shader;
    }
}

int main() {
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(960, 540, "Shader demo");
    SetTargetFPS(60);

    Camera camera = { 0 };
    camera.position = (Vector3) { 12.0f, 2.0f, 0.0f };
    camera.target = (Vector3) { 0.0f, 1.0f, 0.0f };
    camera.up = Y_AXIS;
    camera.fovy = 45.0f;
    camera.projection = CAMERA_PERSPECTIVE;

    Model model = LoadModel("teapot.obj");

    Quaternion quat = QuaternionIdentity();

    ShaderSettings *settings = shaderSettings;
    loadSettings(settings);

    model.materials[0].shader = settings->shader;

    while (!WindowShouldClose()) {
        Vector2 delta = GetMouseDelta();

        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
            quat = QuaternionMultiply(QuaternionFromAxisAngle(Y_AXIS, delta.x * ROTATION_SCALE), quat);
            quat = QuaternionMultiply(QuaternionFromAxisAngle(Z_AXIS, -delta.y * ROTATION_SCALE), quat);
        }

        if (IsKeyPressed(KEY_R)) {
            quat = QuaternionIdentity();
        }

        const ShaderSettings *lastSettings = settings;

        if (IsKeyPressed(KEY_LEFT) && settings != shaderSettings) {
            --settings;
        }

        if (IsKeyPressed(KEY_RIGHT) && settings[1].name != NULL) {
            ++settings;
        }

        if (settings != lastSettings) {
            loadSettings(settings);
            model.materials[0].shader = settings->shader;
        }

        Vector3 rotAxis;
        float rotAngle;
        QuaternionToAxisAngle(quat, &rotAxis, &rotAngle);
        rotAngle *= RAD2DEG;

        BeginDrawing();
            ClearBackground(BLACK);

            BeginMode3D(camera);
                DrawModelEx(model, Vector3Zero(), rotAxis, rotAngle, Vector3One(), WHITE);
            EndMode3D();

            DrawText("Click and drag to rotate", 16, 16, 20, WHITE);
            DrawText("Press R to reset rotation", 16, 36, 20, WHITE);
            DrawText("Press Left and Right to select shader", 16, 56, 20, WHITE);
            DrawText(TextFormat("Current shader: %s", settings->name), 16, 76, 20, WHITE);
        EndDrawing();
    }

    for (ShaderSettings *settings = shaderSettings; settings[1].name; ++settings) {
        if (settings->loaded) {
            UnloadShader(settings->shader);
        }
    }

    UnloadModel(model);

    CloseWindow();

    return 0;
}
