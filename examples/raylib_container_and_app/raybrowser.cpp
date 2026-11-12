#include "raylib_container.h"
#include <raylib.h>

#define WIN_W 1024
#define WIN_H 1024

int main(void) {
    SetTraceLogLevel(LOG_ERROR);
    InitWindow(WIN_W, WIN_H, "NomRayBrowser");

    raylib_container renderer = raylib_container(WIN_W, WIN_H);
    renderer.set_base_url("./");


    std::string html_string = "";

    // It's called 'import css' but it's just an import file function in this impl so we'll reuse it
    std::string base_url;
    renderer.import_css(html_string, "index.html", base_url);

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
