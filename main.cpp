#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL.h>
#include <iostream>
#include <cstdlib>
#include <cmath>

#define SCREEN_WIDTH 640 // Width of the game window
#define SCREEN_HEIGHT 480 // Height of the game window
#define MAX_SCORE 1

using namespace std;

SDL_Rect ball; // Rectangle to represent the ball's position and size
int left_score = 0, right_score = 0, target_x, target_y;
// Direction vectors for the ball's movement
float x_direction = 0.0f, y_direction = 0.0f;
// Tracks the last and current sides the ball is moving towards
char last_side = 'l', current_side = 'l', side_the_ball_goes;

SDL_Texture* creates_text_texture(
  SDL_Renderer* renderer, TTF_Font* font, const string& text, SDL_Color color
) {
  SDL_Surface* text_surface = TTF_RenderText_Solid(font, text.c_str(), color);
  SDL_Texture* text_texture = SDL_CreateTextureFromSurface(
    renderer, text_surface
  );
  SDL_FreeSurface(text_surface); // Frees the surface after creating the texture
  return text_texture;
}

void render_racket(SDL_Renderer* renderer, int x, int y, int width, int height) {
  // Sets the rectangle's position and size
  SDL_Rect racket;
  racket.x = x;
  racket.y = y;
  racket.w = width;
  racket.h = height;

  // Sets the color for the racket (white in this case)
  SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

  // Renders the racket as a filled rectangle
  SDL_RenderFillRect(renderer, &racket);
}

void change_ball_direction() {
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
    current_side = target_x > SCREEN_WIDTH / 2 ? 'r' : 'l';
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

bool next_match(SDL_Renderer* rend, TTF_Font* font) {
  SDL_Color text_color = {255, 255, 255, 255}; // White color
  std::string continue_message = "Press Enter to continue";

  SDL_Texture* message_texture = creates_text_texture(
    rend, font, continue_message, text_color
  );

  int text_width, text_height;
  SDL_QueryTexture(message_texture, NULL, NULL, &text_width, &text_height);

  // Position the message at the center of the screen
  SDL_Rect message_rect = {
    (SCREEN_WIDTH - text_width) / 2,
    (SCREEN_HEIGHT - text_height) / 2 - 75,
    text_width,
    text_height
  };

  SDL_RenderCopy(rend, message_texture, NULL, &message_rect);
  SDL_RenderPresent(rend); // Update the renderer to show the message

  SDL_DestroyTexture(message_texture);

  // Wait for user input
  bool continue_game = false;
  bool waiting_for_input = true;
  while (waiting_for_input) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_KEYDOWN) {
        switch (event.key.keysym.sym) {
          case SDLK_RETURN:  // Enter key to continue
            continue_game = true;
            waiting_for_input = false;
            break;
          case SDLK_ESCAPE: // Escape key to close the window
            waiting_for_input = false;
            break;
          default:
            break;
        }
      } else if (event.type == SDL_QUIT) { // Window close button
        waiting_for_input = false;
      }
    }
  }

  if (!continue_game) {
    SDL_Quit(); // Clean up and quit SDL
  }

  left_score = 0;
  right_score = 0;
  return continue_game;
}

void next_turn() {
  ball.x = (SCREEN_WIDTH - ball.w) / 2;
  ball.y = (SCREEN_HEIGHT - ball.h) / 2;
  throw_ball();
}

void render_winning_message(
  SDL_Renderer* rend, TTF_Font* font, const string& winner
) {
  SDL_Color text_color = {255, 255, 255, 255}; // White color
  SDL_Texture* message_texture = creates_text_texture(
    rend, font, winner, text_color
  );

  int text_width, text_height;
  SDL_QueryTexture(message_texture, NULL, NULL, &text_width, &text_height);

  // Position the message at the center of the screen
  SDL_Rect message_rect = {
    (SCREEN_WIDTH - text_width) / 2,
    (SCREEN_HEIGHT - text_height) / 2 - 115,
    text_width,
    text_height
  };

  SDL_RenderCopy(rend, message_texture, NULL, &message_rect);
  SDL_DestroyTexture(message_texture);
}

int main(int argc, char *argv[]) {
  // Initializes SDL (Simple DirectMedia Layer) library
  if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
    printf("Error initializing SDL: %s\n", SDL_GetError());
    return 1; // Returns an error code if initialization fails
  }

  // Initialize SDL_ttf
  if (TTF_Init() != 0) {
    printf("Error initializing SDL_ttf: %s\n", TTF_GetError());
    return 1;
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

  // Load fonts
  TTF_Font* score_font = TTF_OpenFont("bit5x3.ttf", 96);
  TTF_Font* message_font = TTF_OpenFont("bit5x3.ttf", 48);
  SDL_Color text_color = {255, 255, 255, 255}; // White color

  // Sets the ball's initial size
  ball.w = 20; // Sets the width of the ball
  ball.h = 20; // Sets the height of the ball

  // Sets the ball's initial position to the center of the screen
  ball.x = (SCREEN_WIDTH - ball.w) / 2;
  ball.y = (SCREEN_HEIGHT - ball.h) / 2;

  int close = 0; // Flag to control the game loop
  int speed = 5; // Speed at which the ball moves

  throw_ball();
  bool play = true;

  while (!close && play) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_QUIT) {
        // Close the game when the window X button is pressed
        close = true;
      } else if (event.type == SDL_KEYDOWN) {
        // Close the game when Esc is pressed
        if (event.key.keysym.sym == SDLK_ESCAPE) {
          close = true;
        }
      }
    }

    ball.x += x_direction * speed;
    ball.y += y_direction * speed;

    if (ball.x + ball.w >= SCREEN_WIDTH) {
      left_score += 1;
      next_turn();
    }
    if (ball.x <= 0) {
      right_score += 1;
      next_turn();
    }

    if (ball.y + ball.h >= SCREEN_HEIGHT || ball.y <= 0) {
      change_ball_direction();
    }

    if (x_direction > 0) {
      side_the_ball_goes = 'r';
    } else if (x_direction < 0) {
      side_the_ball_goes = 'l';
    } else {
      side_the_ball_goes = 'c';
    }

    SDL_SetRenderDrawColor(rend, 0, 0, 0, 255);
    SDL_RenderClear(rend);

    // Draw the separator squares only if there is no winner yet
    if (left_score < MAX_SCORE && right_score < MAX_SCORE) {
      SDL_SetRenderDrawColor(rend, 255, 255, 255, 255);
      int screen_divisor_height = 10;
      int spacing = 10;

      for (int y = 5; y < SCREEN_HEIGHT; y += screen_divisor_height + spacing) {
        SDL_Rect square;
        square.w = 1;
        square.h = screen_divisor_height;
        square.x = (SCREEN_WIDTH / 2) - (screen_divisor_height / 2);
        square.y = y;
        SDL_RenderFillRect(rend, &square);
      }
    }
    int left_racket_x = 100;
    int left_racket_y = (SCREEN_HEIGHT - 100) / 2;
    int right_racket_x = SCREEN_WIDTH - 100;
    int right_racket_y = (SCREEN_HEIGHT - 100) / 2;
    int racket_width = 10;
    int racket_height = 100;

    // Create and render the players' rackets
    render_racket(
      rend, left_racket_x, left_racket_y, racket_width, racket_height
    );
    render_racket(
      rend, right_racket_x, right_racket_y, racket_width, racket_height
    );
    SDL_RenderFillRect(rend, &ball);

    // Create and render score textures
    SDL_Texture* left_score_texture = creates_text_texture(
      rend, score_font, to_string(left_score), text_color
    );
    SDL_Texture* right_score_texture = creates_text_texture(
      rend, score_font, to_string(right_score), text_color
    );

    int text_width, text_height;
    SDL_QueryTexture(
      left_score_texture, NULL, NULL, &text_width, &text_height
    );
    SDL_QueryTexture(
      right_score_texture, NULL, NULL, &text_width, &text_height
    );

    // Calculate the X positions for centering the scores
    int left_score_x = (SCREEN_WIDTH / 4) - (text_width / 2);
    int right_score_x = (3 * SCREEN_WIDTH / 4) - (text_width / 2);

    // Position scores with a padding of 30
    SDL_Rect left_score_obj = {left_score_x, 30, text_width, text_height};
    SDL_Rect right_score_obj = {right_score_x, 30, text_width, text_height};

    SDL_RenderCopy(rend, left_score_texture, NULL, &left_score_obj);
    SDL_RenderCopy(rend, right_score_texture, NULL, &right_score_obj);

    SDL_DestroyTexture(left_score_texture);
    SDL_DestroyTexture(right_score_texture);

    if (left_score == MAX_SCORE || right_score == MAX_SCORE) {
      string winner =
        left_score == MAX_SCORE ? "Player 1 Wins!" : "Player 2 Wins!";

      render_winning_message(rend, message_font, winner);
      SDL_RenderPresent(rend);
      play = next_match(rend, message_font);
      if (play) {
        next_turn();
      }
    } else {
      SDL_RenderPresent(rend);
    }

    SDL_Delay(1000 / 60);
  }

  TTF_CloseFont(score_font);
  TTF_CloseFont(message_font);
  TTF_Quit();

  // Cleans up resources before exiting
  SDL_DestroyRenderer(rend);
  SDL_DestroyWindow(win);
  SDL_Quit();

  return 0;
}
