#include "state.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "snake_utils.h"

// Definiciones de funciones de ayuda.
static void set_board_at(game_state_t* state, unsigned int row, unsigned int col, char ch);
static bool is_tail(char c);
static bool is_head(char c);
static bool is_snake(char c);
static char body_to_tail(char c);
static char head_to_body(char c);
static unsigned int get_next_row(unsigned int cur_row, char c);
static unsigned int get_next_col(unsigned int cur_col, char c);
static void find_head(game_state_t* state, unsigned int snum);
static char next_square(game_state_t* state, unsigned int snum);
static void update_tail(game_state_t* state, unsigned int snum);
static void update_head(game_state_t* state, unsigned int snum);

/* Tarea 1 */
game_state_t* create_default_state() {
  // TODO: Implementar esta funcion.
  
  game_state_t* state = malloc(sizeof(game_state_t));
  state -> num_rows = 18;
  state -> num_snakes = 1;

  state -> board = malloc(sizeof(char*)*18);
  for (int i = 0; i < 18; i++) {
    state -> board[i] = malloc(sizeof(char)*21); 
    for (int j = 0; j < 20; j++) {
      if (i == 0 || i == 17 || j == 0 || j == 19) {
        state -> board[i][j] = '#';
      } else {
        state -> board[i][j] = ' ';
      }
    }
    state -> board[i][20] = '\0'; 
  }

  state -> board[2][2] = 'd';  // cola
  state -> board[2][3] = '>';  // cuerpo
  state -> board[2][4] = 'D';  // cabeza
 
  state -> board[2][9] = '*';

  state -> snakes = malloc(sizeof(snake_t));
  snake_t* snake = &state->snakes[0];

  snake -> tail_row = 2;
  snake -> tail_col = 2;
  snake -> head_row = 2;
  snake -> head_col = 4;
  snake -> live = true;
  
  return state;
}


/* Tarea 2 */
void free_state(game_state_t* state) {
  // TODO: Implementar esta funcion.
  
  if (state == NULL) {
    return;
  }
  for (unsigned int i = 0; i < state->num_rows; i++) {
    free(state->board[i]);
  }
  free(state -> board);
  free(state -> snakes);
  free(state);
  
  return;
}


/* Tarea 3 */
void print_board(game_state_t* state, FILE* fp) {
  // TODO: Implementar esta funcion.
  
  if (state == NULL || fp == NULL) {
    return;
  }

  for (unsigned int i = 0; i < state->num_rows; i++) {
    fprintf(fp, "%s\n", state->board[i]);
   }
  
  return;
}


/**
 * Guarda el estado actual a un archivo. No modifica el objeto/struct state.
 * (ya implementada para que la utilicen)
*/
void save_board(game_state_t* state, char* filename) {
  FILE* f = fopen(filename, "w");
  print_board(state, f);
  fclose(f);
}

/* Tarea 4.1 */


/**
 * Funcion de ayuda que obtiene un caracter del tablero dado una fila y columna
 * (ya implementado para ustedes).
*/
char get_board_at(game_state_t* state, unsigned int row, unsigned int col) {
  return state->board[row][col];
}


/**
 * Funcion de ayuda que actualiza un caracter del tablero dado una fila, columna y
 * un caracter.
 * (ya implementado para ustedes).
*/
static void set_board_at(game_state_t* state, unsigned int row, unsigned int col, char ch) {
  state->board[row][col] = ch;
}


/**
 * Retorna true si la variable c es parte de la cola de una snake.
 * La cola de una snake consiste de los caracteres: "wasd"
 * Retorna false de lo contrario.
*/
static bool is_tail(char c) {
  // TODO: Implementar esta funcion.
 
  return c == 'w' || c == 'a' || c == 's' || c == 'd';
 
  return true;
}


/**
 * Retorna true si la variable c es parte de la cabeza de una snake.
 * La cabeza de una snake consiste de los caracteres: "WASDx"
 * Retorna false de lo contrario.
*/
static bool is_head(char c) {
  // TODO: Implementar esta funcion.
  
  return c == 'W' || c == 'A' || c == 'S' || c == 'D' || c == 'x';
  
  return true;
}


/**
 * Retorna true si la variable c es parte de una snake.
 * Una snake consiste de los siguientes caracteres: "wasd^<v>WASDx"
*/
static bool is_snake(char c) {
  // TODO: Implementar esta funcion.
  
  return is_tail(c) || is_head(c) || c == '^' || c == 'v' || c == '<' || c == '>';
  
  return true;
}


/**
 * Convierte un caracter del cuerpo de una snake ("^<v>")
 * al caracter que correspondiente de la cola de una
 * snake ("wasd").
*/
static char body_to_tail(char c) {
  // TODO: Implementar esta funcion.
  
  switch (c) {
    case '^': return 'w';
    case 'v': return 's';
    case '<': return 'a';
    case '>': return 'd';
    default:  return '?';
  }
  
  return '?';
}


/**
 * Convierte un caracter de la cabeza de una snake ("WASD")
 * al caracter correspondiente del cuerpo de una snake
 * ("^<v>").
*/
static char head_to_body(char c) {
  // TODO: Implementar esta funcion.
  
  switch (c) {
    case 'W': return '^';
    case 'S': return 'v';
    case 'A': return '<';
    case 'D': return '>';
    default:  return '?';
  }
  
  return '?';
}


/**
 * Retorna cur_row + 1 si la variable c es 'v', 's' o 'S'.
 * Retorna cur_row - 1 si la variable c es '^', 'w' o 'W'.
 * Retorna cur_row de lo contrario
*/
static unsigned int get_next_row(unsigned int cur_row, char c) {
  // TODO: Implementar esta funcion.
  
  if (c == 'v' || c == 's' || c == 'S') return cur_row + 1;
  if (c == '^' || c == 'w' || c == 'W') return cur_row - 1;
  
  return cur_row;
}


/**
 * Retorna cur_col + 1 si la variable c es '>' or 'd' or 'D'.
 * Retorna cur_col - 1 si la variable c es '<' or 'a' or 'A'.
 * Retorna cur_col de lo contrario
*/
static unsigned int get_next_col(unsigned int cur_col, char c) {
  // TODO: Implementar esta funcion.
  
  if (c == '>' || c == 'd' || c == 'D') return cur_col + 1;
  if (c == '<' || c == 'a' || c == 'A') return cur_col - 1;
  
  return cur_col;
}


/**
 * Tarea 4.2
 *
 * Funcion de ayuda para update_state. Retorna el caracter de la celda
 * en donde la snake se va a mover (en el siguiente paso).
 *
 * Esta funcion no deberia modificar nada de state.
*/
static char next_square(game_state_t* state, unsigned int snum) {
  // TODO: Implementar esta funcion.
  
  if (state == NULL || snum >= state->num_snakes) {
    return '?';
  }

  snake_t snake = state->snakes[snum];
  unsigned int row = snake.head_row;
  unsigned int col = snake.head_col;

  char head_char = get_board_at(state, row, col);

  unsigned int next_row = get_next_row(row, head_char);
  unsigned int next_col = get_next_col(col, head_char);

  return get_board_at(state, next_row, next_col);
  
  return '?';
}


/**
 * Tarea 4.3
 *
 * Funcion de ayuda para update_state. Actualiza la cabeza de la snake...
 *
 * ... en el tablero: agregar un caracter donde la snake se va a mover (¿que caracter?)
 *
 * ... en la estructura del snake: actualizar el row y col de la cabeza
 *
 * Nota: esta funcion ignora la comida, paredes, y cuerpos de otras snakes
 * cuando se mueve la cabeza.
*/
static void update_head(game_state_t* state, unsigned int snum) {
  // TODO: Implementar esta funcion.
  
  snake_t* snake = &state->snakes[snum];

  unsigned int row = snake->head_row;
  unsigned int col = snake->head_col;

  char head_char = get_board_at (state, row, col);

   unsigned int next_row = get_next_row(row, head_char);
   unsigned int next_col = get_next_col(col, head_char);

  set_board_at(state, row, col, head_to_body(head_char));
  set_board_at(state, next_row, next_col, head_char);

  snake->head_row = next_row;
  snake->head_col = next_col;
  
  return;
}


/**
 * Tarea 4.4
 *
 * Funcion de ayuda para update_state. Actualiza la cola de la snake...
 *
 * ... en el tablero: colocar un caracter blanco (spacio) donde se encuentra
 * la cola actualmente, y cambiar la nueva cola de un caracter de cuerpo (^<v>)
 * a un caracter de cola (wasd)
 *
 * ...en la estructura snake: actualizar el row y col de la cola
*/
static void update_tail(game_state_t* state, unsigned int snum) {
  // TODO: Implementar esta funcion.
  
  snake_t* snake = &state->snakes[snum];

  unsigned int row = snake->tail_row;
  unsigned int col = snake->tail_col;

  char tail_char = get_board_at (state, row, col);

  unsigned int next_row = get_next_row(row, tail_char);
  unsigned int next_col = get_next_col(col, tail_char);

  set_board_at(state, row, col, ' ');


  char next_char = get_board_at(state, next_row, next_col);

  set_board_at(state, next_row, next_col, body_to_tail(next_char));


  snake->tail_row = next_row;
  snake->tail_col = next_col;
  
  return;
}

/* Tarea 4.5 */
void update_state(game_state_t* state, int (*add_food)(game_state_t* state)) {
  // TODO: Implementar esta funcion.
  
  for (unsigned int i = 0; i < state->num_snakes; i++) {
    snake_t* snake = &state->snakes[i];

    if (!snake->live) {
      continue; 
    }

    char next = next_square(state, i);

    if (next == '#' || is_snake(next)) {
      // Morir: poner 'x' donde está la cabeza
      set_board_at(state, snake->head_row, snake->head_col, 'x');
      snake->live = false;
      continue;
    }

    update_head(state, i);

    if (next == '*') {
      // Comió fruta → no se actualiza la cola
      if (add_food != NULL) {
        add_food(state);
      }
    } else {
      // No comió fruta → mover cola
      update_tail(state, i);
    }
  }
  
  return;
}

/* Tarea 5 */
game_state_t* load_board(char* filename) {
  // TODO: Implementar esta funcion.
  
  FILE* fp = fopen (filename, "r");
  if (fp == NULL) {
  return NULL; 
  }
  
  size_t capacity = 8;
  size_t count = 0;
  char** temp_rows = malloc(sizeof(char*) * capacity);
  char buffer [1024];
  
  while (fgets(buffer, sizeof(buffer), fp)) {
 
    size_t len = strlen(buffer);
    if (len > 0 && buffer[len - 1] == '\n') {
      buffer[len - 1] = '\0';
      len--;
    }
    
    char* line = malloc(len + 1);
    strcpy(line, buffer);
    
    if (count >= capacity) {
    capacity *= 2;
    temp_rows = realloc (temp_rows, sizeof  (char*)* capacity);
    }
  
 temp_rows [count++] = line;
 }
 
 fclose(fp);
 
 game_state_t* state = malloc(sizeof(game_state_t));
 state->num_rows = (unsigned int) count;
 state->board = malloc(sizeof(char*)* count);
 
 for (size_t i = 0; i < count; i++) {
  state->board[i] = temp_rows[i];
  }
  free(temp_rows);
  
  state->num_snakes = 0;
   state->snakes = NULL;
  
  return state;
}


/**
 * Tarea 6.1
 *
 * Funcion de ayuda para initialize_snakes.
 * Dada una structura de snake con los datos de cola row y col ya colocados,
 * atravezar el tablero para encontrar el row y col de la cabeza de la snake,
 * y colocar esta informacion en la estructura de la snake correspondiente
 * dada por la variable (snum)
*/
static void find_head(game_state_t* state, unsigned int snum) {
  // TODO: Implementar esta funcion.
  
  snake_t* snake = &state->snakes[snum];

  unsigned int row = snake->tail_row;
  unsigned int col = snake->tail_col;
  char c = get_board_at(state, row, col);

  while (!is_head(c)) {

    row = get_next_row(row, c);
    col = get_next_col(col, c);

    c = get_board_at(state, row, col);
    }

  snake->head_row = row;
  snake->head_col = col;
  
  return;
}

/* Tarea 6.2 */
game_state_t* initialize_snakes(game_state_t* state) {
  // TODO: Implementar esta funcion.
  
  unsigned int count = 0;
  for (unsigned int i = 0; i < state->num_rows; i++) {
    for (unsigned int j = 0; state->board[i][j] != '\0'; j++) {
      if (is_tail(state->board[i][j])) {
        count++;
      }
    }
  }


  state->num_snakes = count;
  state->snakes = malloc(sizeof(snake_t) * count);

 
  unsigned int index = 0;
  for (unsigned int i = 0; i < state->num_rows; i++) {
    for (unsigned int j = 0; state->board[i][j] != '\0'; j++) {
      if (is_tail(state->board[i][j])) {
        snake_t* snake = &state->snakes[index++];
        snake->tail_row = i;
        snake->tail_col = j;
        snake->live = true;
        find_head(state, index - 1);
      }
    }
  }
  
  return state;
}
