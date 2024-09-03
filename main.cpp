#include <SDL2/SDL.h>
#include <SDL2/SDL_timer.h>
#include <cstdlib>
#include <cmath>
#include <iostream>

#define SCREEN_WIDTH 640 // Width of the game window
#define SCREEN_HEIGHT 480 // Height of the game window
// X-coordinate for the center of the screen
#define X_CENTER (SCREEN_WIDTH - ball.w) / 2
// Y-coordinate for the center of the screen
#define Y_CENTER (SCREEN_HEIGHT - ball.h) / 2

using namespace std;

SDL_Rect ball; // Rectangle to represent the ball's position and size
int target_x, target_y; // Target coordinates for the ball's direction
// Direction vectors for the ball's movement
float x_direction = 0.0f, y_direction = 0.0f;
// Tracks the last and current sides the ball is moving towards
char last_side = 'l', current_side = 'l';

void change_ball_direction(char side_the_ball_goes) {
  y_direction *= -1; // Reverses the Y direction

  // Adjusts x_direction based on the current side_the_ball_goes direction
  if (side_the_ball_goes == 'r') {
    x_direction += 0.1f; // Moves slightly to the right
  } else if (side_the_ball_goes == 'l') {
    x_direction -= 0.1f; // Moves slightly to the left
  } else if (x_direction == 0.0f) {
    // Moves the ball either to the left or right
    x_direction = ((rand() % 2) == 0) ? 0.1f : -0.1f;
  }
}

// Chooses a new direction for the ball
void choose_initial_ball_direction() {
  // Seeds the random number generator with the current time
  srand((unsigned) time(NULL));

  do {
    // Randomly chooses new target coordinates within screen boundaries
    target_x = rand() % SCREEN_WIDTH;
    target_y = rand() % SCREEN_HEIGHT;

    // Determines the side the board based on the direction and the X axis
    current_side = target_x > X_CENTER ? 'r' : 'l';
  } while (current_side == last_side);

  last_side = current_side; // Updates the last side for future checks
}

// Sets the initial movement direction of the ball
void throw_ball() {
  choose_initial_ball_direction(); // Determines a new direction for the ball

  // Calculates the difference between the current and the target position
  float dx = target_x - ball.x;
  float dy = target_y - ball.y;

  // Calculates the distance to the target position
  float distance = sqrt(dx * dx + dy * dy);

  // Normalizes the direction vector so that its length is 1
  x_direction = dx / distance;
  y_direction = dy / distance;
}

int main(int argc, char *argv[]) {
  // Initializes SDL (Simple DirectMedia Layer) library
  if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
    printf("Error initializing SDL: %s\n", SDL_GetError());
    return 1; // Returns an error code if initialization fails
  }

  // Creates a window with the title "PONG"
  SDL_Window* win = SDL_CreateWindow("PONG",
    SDL_WINDOWPOS_CENTERED,
    SDL_WINDOWPOS_CENTERED,
    SCREEN_WIDTH, SCREEN_HEIGHT, 0
  );

  // Creates a renderer for the window with hardware acceleration
  Uint32 render_flags = SDL_RENDERER_ACCELERATED;
  SDL_Renderer* rend = SDL_CreateRenderer(win, -1, render_flags);

  // Sets the ball's initial size
  ball.w = 20; // Sets the width of the ball
  ball.h = 20; // Sets the height of the ball

  // Sets the ball's initial position to the center of the screen
  ball.x = X_CENTER;
  ball.y = Y_CENTER;

  int close = 0; // Flag to control the game loop
  int speed = 5; // Speed at which the ball moves

  throw_ball(); // Initializes the ball's movement direction

  // Tracks the side the ball is currently moving towards
  char side_the_ball_goes;

  while (!close) { // Main game loop
    SDL_Event event;

    // Handles SDL events, such as window close events
    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_QUIT) {
        close = 1; // Sets close flag to exit the loop
      }
    }

    // Updates the ball's position based on its direction and speed
    ball.x += x_direction * speed;
    ball.y += y_direction * speed;

    // Checks for collisions with the left or right boundaries
    if (ball.x + ball.w >= SCREEN_WIDTH || ball.x <= 0) {
      // Resets the ball to the center of the screen
      ball.x = X_CENTER;
      ball.y = Y_CENTER;
      throw_ball(); // Re-throws the ball with a new direction
    }

    // Checks if it hits the bottom
    if (ball.y + ball.h >= SCREEN_HEIGHT || ball.y <= 0) {
      change_ball_direction(side_the_ball_goes);
    }

    // Updates the side_the_ball_goes based on the ball's horizontal direction
    if (x_direction > 0) {
      side_the_ball_goes = 'r'; // Ball is moving to the right
    } else if (x_direction < 0) {
      side_the_ball_goes = 'l'; // Ball is moving to the left
    } else {
      side_the_ball_goes = 'c'; // Ball is stationary or moving vertically
    }

    // Clears the screen, draw the ball, and updates the display
    SDL_SetRenderDrawColor(rend, 0, 0, 0, 255); // Sets the background to black
    SDL_RenderClear(rend);
    // Sets the ball color to white
    SDL_SetRenderDrawColor(rend, 255, 255, 255, 255);
    SDL_RenderFillRect(rend, &ball); // Draws the ball
    SDL_RenderPresent(rend);

    // Delays to maintain a frame rate of 60 FPS
    SDL_Delay(1000 / 60);
  }

  // Cleans up resources before exiting
  SDL_DestroyRenderer(rend);
  SDL_DestroyWindow(win);
  SDL_Quit();

  return 0;
}
