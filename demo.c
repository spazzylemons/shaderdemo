#include <raylib.h>
#include <raymath.h>

#define ROTATION_SCALE 0.005f

#define Y_AXIS ((Vector3) { 0.0f, 1.0f, 0.0f })
#define Z_AXIS ((Vector3) { 0.0f, 0.0f, 1.0f })

int main() {
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(960, 540, "3D Voronoi shader demo");
    SetTargetFPS(60);

    Camera camera = { 0 };
    camera.position = (Vector3) { 12.0f, 2.0f, 0.0f };
    camera.target = (Vector3) { 0.0f, 1.0f, 0.0f };
    camera.up = Y_AXIS;
    camera.fovy = 45.0f;
    camera.projection = CAMERA_PERSPECTIVE;

    Model model = LoadModel("teapot.obj");
    Shader shader = LoadShader("vertex.glsl", "fragment.glsl");

    model.materials[0].shader = shader;

    Quaternion quat = QuaternionIdentity();

    while (!WindowShouldClose()) {
        Vector2 delta = GetMouseDelta();

        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
            quat = QuaternionMultiply(QuaternionFromAxisAngle(Y_AXIS, delta.x * ROTATION_SCALE), quat);
            quat = QuaternionMultiply(QuaternionFromAxisAngle(Z_AXIS, -delta.y * ROTATION_SCALE), quat);
        }

        if (IsKeyPressed(KEY_R)) {
            quat = QuaternionIdentity();
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
        EndDrawing();
    }

    UnloadModel(model);
    UnloadShader(shader);

    CloseWindow();

    return 0;
}
