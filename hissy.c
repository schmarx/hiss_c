
#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <wchar.h>

#ifdef __linux__
#include <ncurses/ncurses.h>
#else
#include <ncurses/ncurses.h>
#endif

int width = 10;
int height = 10;

typedef struct {
	int x;
	int y;
} vect;

typedef struct body {
	vect pos;
	chtype shape[2];
	struct body *next;
	struct body *prev;
} body;

body *hiss;
body *hiss_tail;

vect food;
vect velocity;
vect last_pos;

int iterations = 0;

char *yellow = "\033[93m";
char *blue = "\033[94m";
char *reset = "\033[0m";
char *underline = "\033[4m";

// wchar_t middle_vertical = L'\u2551';   // ║
// wchar_t middle_horizontal = L'\u2550'; // ═
// wchar_t top_right = L'\u2557';		   // ╗
// wchar_t bottom_right = L'\u255d';	   // ╝
// wchar_t bottom_left = L'\u255a';	   // ╚
// wchar_t top_left = L'\u2554';		   // ╔

wchar_t middle_vertical = 186;	 // ║
wchar_t middle_horizontal = 205; // ═
wchar_t top_right = 187;		 // ╗
wchar_t bottom_right = 188;		 // ╝
wchar_t bottom_left = 200;		 // ╚
wchar_t top_left = 201;			 // ╔

int score = -1;

int turn_length = 150;

char *title = "hissy";

/** print the title */
void print_title() {
	for (int i = 0; i < 5; i++) {
		addch(title[i]);
	}
}

/** make horizontal line of certain length */
void line(int len) {
	for (int i = 0; i < len; i++) {
		addch(ACS_HLINE);
	}
}

/** update hissy's shape based on other segments' positions */
void update_shape(chtype vals[2], int curr_x, int curr_y, int prev_x, int prev_y, int next_x, int next_y) {
	if (curr_x == prev_x && curr_x == next_x) {
		vals[0] = ' ';
		vals[1] = ACS_VLINE;
	} else if (curr_y == prev_y && curr_y == next_y) {
		vals[0] = ACS_HLINE;
		vals[1] = ACS_HLINE;
	}

	else if (curr_x == prev_x && curr_y < prev_y && curr_x < next_x) {
		vals[0] = ' ';
		vals[1] = ACS_ULCORNER;
	} else if (curr_x == prev_x && curr_y < prev_y && curr_x > next_x) {
		vals[0] = ACS_HLINE;
		vals[1] = ACS_URCORNER;
	} else if (curr_x == prev_x && curr_y > prev_y && curr_x < next_x) {
		vals[0] = ' ';
		vals[1] = ACS_LLCORNER;
	} else if (curr_x == prev_x && curr_y > prev_y && curr_x > next_x) {
		vals[0] = ACS_HLINE;
		vals[1] = ACS_LRCORNER;
	}

	else if (curr_y == prev_y && curr_x < prev_x && curr_y < next_y) {
		vals[0] = ' ';
		vals[1] = ACS_ULCORNER;
	} else if (curr_y == prev_y && curr_x < prev_x && curr_y > next_y) {
		vals[0] = ' ';
		vals[1] = ACS_LLCORNER;
	} else if (curr_y == prev_y && curr_x > prev_x && curr_y < next_y) {
		vals[0] = ACS_HLINE;
		vals[1] = ACS_URCORNER;
	} else if (curr_y == prev_y && curr_x > prev_x && curr_y > next_y) {
		vals[0] = ACS_HLINE;
		vals[1] = ACS_LRCORNER;
	}

	else {
		vals[0] = 'O';
		vals[1] = 'O';
	}
}

/** draw the game board */
void draw(void) {
	clear();

	addch(ACS_ULCORNER);

	line(2);
	print_title();
	line(2 * width - 2 + 1 - strlen(title));

	addch(ACS_URCORNER);
	addch('\n');

	for (int i = 0; i < height; i++) {
		addch(ACS_VLINE);
		for (int j = 0; j < width; j++) {
			int has = 0;

			for (body *hiss_current = hiss; hiss_current != NULL; hiss_current = hiss_current->next) {
				if (i == hiss_current->pos.y && j == hiss_current->pos.x) {
					has = true;
					attron(COLOR_PAIR(1));

					// the head
					if (hiss_current == hiss) {
						addch(' ');
						if (velocity.x == 1) addch('<');
						else if (velocity.x == -1) addch('>');
						else if (velocity.y == 1) addch('A');
						else if (velocity.y == -1) addch('Y');
					} else {
						addch(hiss_current->shape[0]);
						addch(hiss_current->shape[1]);
					}

					attroff(COLOR_PAIR(1));
					break;
				}
				if (hiss_current == hiss_tail) break;
			}
			if (i == food.y && j == food.x) {
				attron(COLOR_PAIR(2));
				addch(' ');
				addch('o');
				attroff(COLOR_PAIR(2));
			} else if (!has) {
				addch(' ');
				addch(' ');
			}
		}
		addch(' ');
		addch(ACS_VLINE);
		addch('\n');
	}

	addch(ACS_LLCORNER);
	line(width * 2 + 1);
	addch(ACS_LRCORNER);
	addch('\n');
}

/** add length to hissy */
void add(void) {
	turn_length = turn_length * 0.95;

	body *hiss_next = malloc(sizeof(body));
	hiss_next->pos.x = hiss_tail->pos.x;
	hiss_next->pos.y = hiss_tail->pos.y;
	hiss_next->next = hiss;
	hiss_next->prev = hiss_tail;

	hiss_next->shape[0] = hiss_tail->shape[0];
	hiss_next->shape[1] = hiss_tail->shape[1];

	hiss->prev = hiss_next;

	hiss_tail->next = hiss_next;
	hiss_tail = hiss_next;
}

/** randomly place food */
void place_food(void) {
	int possible = 0;
	int *possible_values = malloc(sizeof(int) * width * height);

	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			int occupied = 0;
			for (body *hiss_current = hiss; hiss_current != NULL; hiss_current = hiss_current->next) {
				if (hiss_current->pos.y == i && hiss_current->pos.x == j) {
					occupied = 1;
					break;
				}

				if (hiss_current == hiss_tail) break;
			}
			if (!occupied) possible_values[possible++] = i * width + j;
		}
	}
	if (possible == 0) {
		// game win
	} else {
		score++;

		int cell = possible_values[rand() % possible];
		food.x = cell % width;
		food.y = cell / width;
	}

	free(possible_values);
}

/** frees memory */
void free_hiss() {
	body *next_hiss;
	int done = 0;
	while (hiss != NULL && !done) {
		if (hiss == hiss_tail) done = true;
		next_hiss = hiss->next;
		free(hiss);
		hiss = next_hiss;
	}
}

/** initialises game */
void init(void) {
	initscr();
	noecho();
	srand(clock());

	start_color();
	init_pair(1, COLOR_BLUE, COLOR_BLACK);
	init_pair(2, COLOR_YELLOW, COLOR_BLACK);

	hiss = malloc(sizeof(body));
	hiss->pos.x = 0;
	hiss->pos.y = 0;
	hiss->next = hiss;
	hiss->prev = hiss;

	hiss_tail = hiss;

	velocity.x = 1;
	velocity.y = 0;

	last_pos.x = 0;
	last_pos.y = 0;
}

int main(int argc, char *argv[]) {
	init();

	place_food();

	while (1) {
		// user input
		timeout(turn_length);
		char c = getch();

		// time_t start_time = clock();
		// while ((clock() - start_time) * 1000 / CLOCKS_PER_SEC < turn_length) {
		// 	timeout(turn_length - (clock() - start_time) * 1000 / CLOCKS_PER_SEC);
		// 	char ch = getch();
		// 	if (ch == 'w' || ch == 'a' || ch == 's' || ch == 'd') c = ch;
		// }
		// printf("moving\n");

		if (c == 'w' && velocity.y == 0) {
			velocity.x = 0;
			velocity.y = -1;
		} else if (c == 'a' && velocity.x == 0) {
			velocity.x = -1;
			velocity.y = 0;
		} else if (c == 's' && velocity.y == 0) {
			velocity.x = 0;
			velocity.y = 1;
		} else if (c == 'd' && velocity.x == 0) {
			velocity.x = 1;
			velocity.y = 0;
		} else if (c == 'q') {
			break;
		}

		// move tail piece to the front
		last_pos.x = hiss_tail->pos.x;
		last_pos.y = hiss_tail->pos.y;

		hiss_tail->pos.x = hiss->pos.x + velocity.x;
		hiss_tail->pos.y = hiss->pos.y + velocity.y;

		hiss = hiss_tail;
		hiss_tail = hiss->prev;

		// check for collisions
		for (body *hiss_current = hiss; hiss_current != NULL; hiss_current = hiss_current->next) {
			if (hiss_current == hiss_tail) break;

			if (hiss_current->next->pos.y == hiss->pos.y && hiss_current->next->pos.x == hiss->pos.x) {
				printf("game over\n");
				return 0;
			}
		}

		// update shape
		update_shape(hiss->next->shape, hiss->next->pos.x, hiss->next->pos.y, hiss->pos.x, hiss->pos.y, hiss->next->next->pos.x, hiss->next->next->pos.y);

		// check board bounds
		if (hiss->pos.x < 0) hiss->pos.x = width - 1;
		if (hiss->pos.y < 0) hiss->pos.y = height - 1;
		if (hiss->pos.x >= width) hiss->pos.x = 0;
		if (hiss->pos.y >= height) hiss->pos.y = 0;

		// check whether you are in contact with food
		if (hiss->pos.x == food.x && hiss->pos.y == food.y) {
			add();
			place_food();
		}

		draw();
		iterations++;
	}

	endwin();
	free_hiss(hiss);

	printf("\ngoodbye :)\n\n");

	return 0;
}