
#include <locale.h>
#include <ncurses/ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>

int width = 10;
int height = 10;

typedef struct {
	int x;
	int y;
} vect;

typedef struct body {
	vect pos;
	struct body *next;
	struct body *prev;
} body;

body *hiss;
body *hiss_tail;

vect food;
vect velocity;

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

void draw(void) {
	clear();

	addch(ACS_ULCORNER);
	char *title = "hissy";

	for (int i = 0; i < width - 2; i++) {
		addch(ACS_HLINE);
	}
	for (int i = 0; i < 5; i++) {
		addch(title[i]);
	}
	for (int i = 0; i < width - 3; i++) {
		addch(ACS_HLINE);
	}
	addch(ACS_HLINE);
	addch(ACS_URCORNER);
	addch('\n');

	for (int i = 0; i < height; i++) {
		addch(ACS_VLINE);
		for (int j = 0; j < width; j++) {
			int has = 0;

			for (body *hiss_current = hiss; hiss_current != NULL; hiss_current = hiss_current->next) {
				if (i == hiss_current->pos.y && j == hiss_current->pos.x) {
					char ch = ' ';

					attron(COLOR_PAIR(1));

					if (hiss_current == hiss) {
						addch(' ');
						addch('H');
					} else if (hiss_current->pos.y == hiss_current->prev->pos.y && hiss_current->pos.y == hiss_current->next->pos.y) {
						addch(ACS_HLINE);
						addch(ACS_HLINE);
					} else {
						if (hiss_current->pos.x == hiss_current->prev->pos.x && hiss_current->pos.x == hiss_current->next->pos.x) {
							addch(' ');
							addch(ACS_VLINE);
						} else if (hiss_current->pos.x == hiss_current->prev->pos.x && hiss_current->pos.y < hiss_current->prev->pos.y && hiss_current->pos.x < hiss_current->next->pos.x) {
							addch(' ');
							addch(ACS_ULCORNER);
						} else if (hiss_current->pos.x == hiss_current->prev->pos.x && hiss_current->pos.y > hiss_current->prev->pos.y && hiss_current->pos.x < hiss_current->next->pos.x) {
							addch(' ');
							addch(ACS_LLCORNER);
						} else if (hiss_current->pos.y == hiss_current->prev->pos.y && hiss_current->pos.x < hiss_current->prev->pos.x && hiss_current->pos.y < hiss_current->next->pos.y) {
							addch(' ');
							addch(ACS_ULCORNER);
						} else if (hiss_current->pos.y == hiss_current->prev->pos.y && hiss_current->pos.x < hiss_current->prev->pos.x && hiss_current->pos.y > hiss_current->next->pos.y) {
							addch(' ');
							addch(ACS_LLCORNER);
						}

						else if (hiss_current->pos.x == hiss_current->prev->pos.x && hiss_current->pos.y > hiss_current->prev->pos.y && hiss_current->pos.x > hiss_current->next->pos.x) {
							addch(ACS_HLINE);
							addch(ACS_LRCORNER);
						} else if (hiss_current->pos.x == hiss_current->prev->pos.x && hiss_current->pos.y < hiss_current->prev->pos.y && hiss_current->pos.x > hiss_current->next->pos.x) {
							addch(ACS_HLINE);
							addch(ACS_URCORNER);
						} else if (hiss_current->pos.y == hiss_current->prev->pos.y && hiss_current->pos.x > hiss_current->prev->pos.x && hiss_current->pos.y < hiss_current->next->pos.y) {
							addch(ACS_HLINE);
							addch(ACS_URCORNER);
						} else if (hiss_current->pos.y == hiss_current->prev->pos.y && hiss_current->pos.x > hiss_current->prev->pos.x && hiss_current->pos.y > hiss_current->next->pos.y) {
							addch(ACS_HLINE);
							addch(ACS_LRCORNER);
						} else addch('O');
					}

					// if (hiss_current == hiss) printw(" %c", 'H');
					// else if (hiss_current->next->pos.x == hiss_current->pos.x) {
					// 	addch(' ');
					// 	if (hiss_current->prev->pos.x == hiss_current->pos.x) addch(ACS_VLINE);
					// 	else if (hiss_current->prev->pos.x > hiss_current->prev->pos.x) addch(ACS_LLCORNER);
					// 	else addch(ACS_LRCORNER);
					// } else if (hiss_current->next->pos.x == hiss_current->pos.x) {
					// 	addch(' ');
					// 	addch(ACS_VLINE);
					// } else if (hiss_current->next->pos.y == hiss_current->pos.y) {
					// 	addch(ACS_HLINE);
					// 	addch(ACS_HLINE);
					// } else printw(" %c", 'h');

					attroff(COLOR_PAIR(1));
					has = true;
					break;
				}
				if (hiss_current == hiss_tail) break;
			}
			if (i == food.y && j == food.x) {
				attron(COLOR_PAIR(2));
				printw(" o", yellow, reset);
				attroff(COLOR_PAIR(2));
			} else if (!has) printw("  ");
		}
		addch(' ');
		addch(ACS_VLINE);
		addch('\n');
	}

	addch(ACS_LLCORNER);
	for (int i = 0; i < width; i++) {
		addch(ACS_HLINE);
		addch(ACS_HLINE);
	}
	addch(ACS_HLINE);
	addch(ACS_LRCORNER);
	addch('\n');
}

void add(void) {
	body *hiss_next = malloc(sizeof(body));
	hiss_next->pos.x = hiss_tail->pos.x;
	hiss_next->pos.y = hiss_tail->pos.y;
	hiss_next->next = hiss;
	hiss_next->prev = hiss_tail;

	hiss->prev = hiss_next;

	hiss_tail->next = hiss_next;
	hiss_tail = hiss_next;
}

void place_food(void) {
	food.x = rand() % width;
	food.y = rand() % height;
}

void free_hiss() {
	body *next_hiss;
	while (hiss != NULL) {
		next_hiss = hiss->next;
		free(hiss);
		hiss = next_hiss;
	}
}

int main(int argc, char *argv[]) {

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

	place_food();

	int index = 0;

	velocity.x = 1;
	velocity.y = 0;

	timeout(100);
	while (1) {
		char c = getch();
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
		}

		hiss_tail->pos.x = hiss->pos.x + velocity.x;
		hiss_tail->pos.y = hiss->pos.y + velocity.y;

		hiss = hiss_tail;
		hiss_tail = hiss->prev;

		if (hiss->pos.x < 0) hiss->pos.x = width - 1;
		if (hiss->pos.y < 0) hiss->pos.y = height - 1;
		if (hiss->pos.x >= width) hiss->pos.x = 0;
		if (hiss->pos.y >= height) hiss->pos.y = 0;

		if (hiss->pos.x == food.x && hiss->pos.y == food.y) {
			add();
			place_food();
		}

		draw();
		iterations++;
	}

	endwin();

	free_hiss(hiss);

	return 0;
}