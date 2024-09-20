#include <raylib.h>
#include <raymath.h>
#include <iostream>
#include <vector>
#include <string>

// Class that holds the data for planets/stars
class Object {
    public:
    Vector2 position;
    Vector2 velocity;
    float mass;
    Color color;

    void Draw() {
        DrawCircle(position.x, position.y, 20, color);
    }

    void Update() {
        position.x += velocity.x;
        position.y += velocity.y;
    }

    Object(Vector2 Position, float Mass, Color clr) : position(Position), mass(Mass), color(clr) {}
};

class Player : public Object {
    private:
    Texture2D sprite;

    public:
    void LoadSprite() {
        sprite = LoadTexture("Sprites/astronaut.png");
    }

    void Draw() {
        DrawTexture(sprite, position.x, position.y, WHITE);
    }

    void CheckCollision(std::vector<Object*> objects) {
        for (Object* object : objects) {
            if (CheckCollisionCircleRec(object->position, 20, {position.x, position.y, 16, 16})) {
                std::cout << "Collision between player and planet has occured" << std::endl;
            }
        }
    }

    Player(Vector2 Position, float Mass, Color clr) : Object(Position, Mass, clr) {}
};

// Get the force at which two objects exert onto each other
Vector2 GravityForce(Object obj1, Object obj2) {
    const float g = 0.26;
    Vector2 direction = {obj2.position.x - obj1.position.x, obj2.position.y - obj1.position.y};

    float distance = sqrt(direction.x * direction.x + direction.y * direction.y);
    if (distance < 1.0f) distance = 1.0f;  // Avoid division by zero

    float force = (g * obj1.mass * obj2.mass) / (distance * distance);

    direction = Vector2Normalize(direction);

    return {direction.x * force, direction.y * force};
}

// Simulate all objects with gravity and update there velocities
void SimulateGravity(std::vector<Object*> objects) {
    for (size_t i = 0; i < objects.size(); i++) {
        for (size_t j = 0; j < objects.size(); j++) {
            Object& obj1 = *objects[i];
            Object& obj2 = *objects[j];

            Vector2 force = GravityForce(obj1, obj2);

            // Update velocities based on force and mass
            obj1.velocity.x += force.x / obj1.mass;
            obj1.velocity.y += force.y / obj1.mass;

            // Apply equal and opposite force to obj2
            obj2.velocity.x -= force.x / obj2.mass;
            obj2.velocity.y -= force.y / obj2.mass;
        }
    }
}

void UpdateGame() {
    
}

int Input(bool& debounce, bool& debounceD, int xVel) {
    if (IsKeyPressed(KEY_A) && debounce == false) {
        debounce = true;
        xVel += -1;
    }
    if (IsKeyPressed(KEY_D) && debounceD == false) {
        debounceD = true;
        xVel += 1;
    }

    if (IsKeyReleased(KEY_A) && debounce == true) {
        debounce = false;
        xVel += 1;
    }
    if (IsKeyReleased(KEY_D) && debounceD == true) {
        debounceD = false;
        xVel += -1;
    }

    return xVel;
}

void DrawGame() {

}

// Constant variables
static const int WIDTH = 900;
static const int HEIGHT = 600;
static const char* TITLE = "Solar System Simulation";
static const short int FPS = 60;

int main() {
    InitWindow(WIDTH, HEIGHT, TITLE);

    Object sun({450, 300}, 2000, YELLOW);
    Object planet({300, 300}, 10, GREEN);
    Object planet2({300, 400}, 10, WHITE);
    Player player({10, 10}, 5, WHITE);

    Camera2D camera;

    camera.target = player.position;
    camera.offset = {(float)WIDTH / 2, (float)HEIGHT / 2};  // Center the camera on the screen
    camera.rotation = 0.0f;
    camera.zoom = 0.5f;

    player.LoadSprite();

    SetTargetFPS(FPS);
    planet.velocity = {1, 2.25f};
    planet2.velocity = {1, 2};

    std::vector<Object*> objects;
    objects.push_back(&sun);
    objects.push_back(&planet);
    objects.push_back(&planet2);
    objects.push_back(&player);

    bool debounce = false;
    bool debounceD = false;
    int xVel = 0;

    while (WindowShouldClose() == false) {
        BeginDrawing();
        ClearBackground(BLACK);

        UpdateGame();
        SimulateGravity(objects);
        player.CheckCollision(objects);
        xVel = Input(debounce, debounceD, xVel);
        player.velocity = {(float)xVel + player.velocity.x, player.velocity.y};

        camera.target = player.position;

        BeginMode2D(camera);
        sun.Draw();
        planet.Draw();
        player.Draw();
        planet2.Draw();

        EndMode2D();

        planet.Update();
        planet2.Update();
        sun.Update();
        player.Update();

        DrawGame();

        EndDrawing();
    }

    CloseWindow();
    return 0;
}