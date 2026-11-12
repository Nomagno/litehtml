#include "raylib_container.h"
#include <raylib.h>

#define WIN_W 1024
#define WIN_H 1024

int main(int argc, char *argv[]) {
    std::string initial_file = "index.html";

    if (argc >= 2)
        initial_file = argv[1];

    SetTraceLogLevel(LOG_ERROR);
    InitWindow(WIN_W, WIN_H, "NomRayBrowser");

    raylib_container renderer = raylib_container(WIN_W, WIN_H);
    renderer.set_base_url("./");


    std::string html_string = "";

    std::string base_url;
    renderer.import_file(html_string, initial_file, base_url);

    // Parses HTML
    // Can also pass master css and user css strings, but we'll use default for now
    litehtml::document::ptr mydoc = litehtml::document::createFromString(html_string.c_str(), &renderer);


    while (!WindowShouldClose()) {
        BeginDrawing();

        ClearBackground(LIGHTGRAY);
        
        // Computes positions
        mydoc->render(WIN_W);


        // Native rendering
        litehtml::position clip;
        clip.x = 0;
        clip.y = 0;
        clip.width = WIN_W;
        clip.height = WIN_H;

        mydoc->draw((uintptr_t)NULL, 0, 0, &clip);


        // Sends event data

        litehtml::position::vector vec;
        if (IsMouseButtonDown(0)) {
            mydoc->on_lbutton_down(GetMouseX(), GetMouseY(), GetMouseX(), GetMouseY(), vec);
            mydoc->on_lbutton_up(GetMouseX(), GetMouseY(), GetMouseX(), GetMouseY(), vec);
        } else {
            mydoc->on_mouse_over(GetMouseX(), GetMouseY(), GetMouseX(), GetMouseY(), vec);
        }


        EndDrawing();
        WaitTime(0.1);
    }
}
