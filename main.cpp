#include <raylib.h>
#include <raymath.h>
#include <iostream>
#include <vector>
#include <string>

// Class that holds the data for planets/stars
class Object {
    char* name;
    public:
    Vector2 position;
    Vector2 velocity;
    float mass;
    Color color;
    bool canCollide = true;

    void Draw() {
        DrawCircle(position.x, position.y, 20, color);
    }

    void Update() {
        std::cout << name << std::endl;

        // we need to check if an object is near us and push it relative to where we are going
        position.x += velocity.x;
        position.y += velocity.y;
    }

    Object(char* Name, Vector2 Position, float Mass, Color clr) : name(Name), position(Position), mass(Mass), color(clr) {}
};

class Player : public Object {
    private:
    Texture2D sprite;
    Rectangle boundingBox;  // Add bounding box for collision detection

    public:
    void LoadSprite() {
        sprite = LoadTexture("Sprites/astronaut.png");
        boundingBox = {position.x, position.y, (float)sprite.width, (float)sprite.height};  // Define bounding box size
    }

    void Draw() {
        DrawTexture(sprite, position.x, position.y, WHITE);
        // Uncomment for visualizing the bounding box
        DrawRectangleLinesEx(boundingBox, 2, RED);
    }

    void Update(std::vector<Object*>& objects) {
        bool hasCollided = false;
        
        // Check collisions with other objects
        for (Object* object : objects) {
            if (object != this && object->canCollide) {  // Ensure we're not colliding with ourselves
                if (CheckCollisionCircleRec(object->position, 20, {position.x + velocity.x, position.y + velocity.y, boundingBox.width, boundingBox.height})) {
                    std::cout << "Collision occured" << std::endl;
                    hasCollided = true;

                    velocity.x = 0;
                    velocity.y = 0;
                    break;  // Stop checking further once a collision is found
                }
            }
        }

        // Update position only if no collision was detected
        if (!hasCollided) {
            std::cout << velocity.x << " " << velocity.y << std::endl;
            position.x = position.x + velocity.x;
            position.y = position.y + velocity.y;
            boundingBox.x = position.x;
            boundingBox.y = position.y;  // Update bounding box position
        }
    }

    Player(char* Name, Vector2 Position, float Mass, Color clr) : Object(Name, Position, Mass, clr) {
        canCollide = true;
    }
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

// Simulate all objects with gravity and update their velocities
void SimulateGravity(std::vector<Object*> objects) {
    for (size_t i = 0; i < objects.size(); i++) {
        for (size_t j = i + 1; j < objects.size(); j++) {
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

void DrawGame() {
    // Additional drawing logic if needed
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

// Constant variables
static const int WIDTH = 900;
static const int HEIGHT = 600;
static const char* TITLE = "Solar System Simulation";
static const short int FPS = 60;

int main() {
    InitWindow(WIDTH, HEIGHT, TITLE);

    Object sun("sun", {450, 300}, 5000, YELLOW);
    Object planet("planet", {300, 300}, 10, GREEN);
    Player player("player", {100, 100}, 5, WHITE);

    Camera2D camera;
    camera.target = player.position;
    camera.offset = {(float)WIDTH / 2, (float)HEIGHT / 2};  // Center the camera on the screen
    camera.rotation = 0.0f;
    camera.zoom = 0.5f;

    player.LoadSprite();

    SetTargetFPS(FPS);
    planet.velocity = {1, 2.25f};

    std::vector<Object*> objects;
    objects.push_back(&sun);
    objects.push_back(&planet);
    objects.push_back(&player);

    bool debounce = false;
    bool debounceD = false;
    int xVel = 0;

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(BLACK);

        SimulateGravity(objects);
        xVel = Input(debounce, debounceD, xVel);
        player.velocity = {(float)xVel + player.velocity.x, player.velocity.y};

        camera.target = player.position;

        planet.Update();
        sun.Update();
        player.Update(objects);  // Pass objects for collision checking

        sun.Draw();
        planet.Draw();
        player.Draw();

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
