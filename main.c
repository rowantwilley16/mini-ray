//Ray-tracer in c, using SDL3.
//Author: Rowan Twilley

//tip = reparse the project if the autocomplete isn't working!

#include <stdio.h>
#include <math.h>
#include <stdbool.h>

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#define WIDTH           1400
#define HEIGHT          800

#define RAY_THICKNESS   2

#define COLOR_BLACK     0x00000000
#define COLOR_WHITE     0xffffffff
#define COLOR_GREY      0xAAAAAAAA
#define COLOR_RAYS      0xFFEE8C
#define FRAME_RATE      144
#define RAYS_NUMBER     700

typedef struct Circle {

    double x;
    double y;
    double radius;

}Circle;

typedef struct Ray {
    double x_start;
    double y_start;
    double angle;
    double x_end;
    double y_end;

}Ray;

void generate_rays(Circle circle, Ray rays[RAYS_NUMBER]) {
    for (int i = 0; i < RAYS_NUMBER; i++) {
        double angle = (2 * M_PI * i) / RAYS_NUMBER; // Angle evenly spaced in [0, 2pi)
        //sdouble angle = (i/ (double) RAYS_NUMBER) * 360; //Angle evenly spaced in [0,360)
        rays[i].x_start = circle.x;
        rays[i].y_start = circle.y;
        rays[i].angle = angle;

        // Extend the ray outwards, e.g., 1000 units from the center
        double length = 1000.0;
        rays[i].x_end = circle.x + cos(angle) * length;
        rays[i].y_end = circle.y + sin(angle) * length;
        //printf("%f\n", angle); --this causes a lot of lag
    }
}


void FillCircle(SDL_Surface* surface, Circle circle,Uint32 color){

    double radius_squared = pow(circle.radius,2);

    for (double x = circle.x - circle.radius; x <= (circle.radius + circle.x); x ++)
        {
        for (double y = circle.y - circle.radius; y <= (circle.radius + circle.y); y++)
        {
            double distance_sqruared = pow(x-circle.x,2) + pow(y-circle.y,2);
            if (distance_sqruared < radius_squared)
                {
                SDL_Rect pixel = (SDL_Rect){x,y,1,1};
                SDL_FillSurfaceRect(surface, &pixel,color);
            }
        }
    }
}
void FillRays(SDL_Surface* surface, Ray rays[RAYS_NUMBER], uint32_t color, Circle object){

    double radius_squared = pow(object.radius,2);

    for (int i=0; i < RAYS_NUMBER; i ++){
        Ray ray = rays[i];

        int end_of_screen = 0;
        int object_hit = 0;

        double step = 1;
        double x_draw = ray.x_start;
        double y_draw = ray.y_start;
        while (!end_of_screen && !object_hit)
        {
            x_draw += step*cos(ray.angle);
            y_draw += step*sin(ray.angle);

            SDL_Rect ray_point = (SDL_Rect){x_draw,y_draw, RAY_THICKNESS, RAY_THICKNESS};
            SDL_FillSurfaceRect(surface, &ray_point, color);

            if (x_draw <0 || x_draw >WIDTH){
                end_of_screen = 1;
            }
            if (y_draw<0 || y_draw > HEIGHT){
                end_of_screen = 1;
            }

            //Does the ray hit an object ?
            double distance_squared = pow(x_draw - object.x,2) + pow(y_draw - object.y,2);
            if (distance_squared < radius_squared){

                break;
            }
        }
    }
}
int main(int argc, char* argv[]) {

    SDL_Window *window;                    // Declare a pointer
    SDL_Surface *surface;

    bool done = false;

    SDL_Init(SDL_INIT_VIDEO);              // Initialize SDL3

    // Create an application window with the following settings:
    window = SDL_CreateWindow(
        "raytracer-c",                  // window title
        WIDTH,                          // width, in pixels
        HEIGHT,                         // height, in pixels
        SDL_WINDOW_OPENGL               // flags - see below
    );

    // Check that the window was successfully created
    if (window == NULL) {
        // In the case that the window could not be made...
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Could not create window: %s\n", SDL_GetError());
        return 1;
    }

    //define a surface for the window
    surface = SDL_GetWindowSurface(window);

    //define a rectangle that you want to draw
    //SDL_Rect rect = (SDL_Rect) {200,200,200,200};
    //SDL_FillSurfaceRect(surface, &rect, COLOR_WHITE);

    Circle circle = {200,200,40};
    Circle shadow_circle = {1000,300,80};

    SDL_Rect erase_rect = {0,0,WIDTH,HEIGHT};

    //Generate an array of rays structs
    Ray rays[RAYS_NUMBER];
    generate_rays(circle, rays);

    //calculate the frame time
    double frame_time = (1 / (double) FRAME_RATE) * pow(10,3);
    //printf("FRAME_RATE: %.2f FPS\n", (1/(double) frame_time) * (double) pow(10,3));

    //moving the shadow circle for visualization
    double obj_speed = 5;

    while (!done) {

        SDL_Event event;

        while (SDL_PollEvent(&event)) {
            // Quit Event - for keypress
            if (event.type == SDL_EVENT_QUIT) {
                done = true;
            }if (event.type == SDL_EVENT_MOUSE_MOTION && event.motion.state != 0){
                circle.x = event.motion.x;
                circle.y = event.motion.y;
                generate_rays(circle, rays);
            }
        }
        //Clear the Screen
        SDL_FillSurfaceRect(surface, &erase_rect, COLOR_BLACK);

        //Update the window by redrawing
        FillRays(surface, rays, COLOR_RAYS, shadow_circle);
        FillCircle(surface, circle, COLOR_WHITE);
        FillCircle(surface, shadow_circle, COLOR_WHITE);

        SDL_UpdateWindowSurface(window);
        SDL_Delay(frame_time); //frame rate of 100 fps

        // Do game logic, present a frame, etc.

        shadow_circle.y += obj_speed;

        if (shadow_circle.y - shadow_circle.radius <0){
            obj_speed *= -1;
        }else if (shadow_circle.y + shadow_circle.radius > HEIGHT){
            obj_speed *= -1;
        }
    }

    // Close and destroy the window
    SDL_DestroyWindow(window);

    // Clean up
    SDL_Quit();
    return 0;
}
