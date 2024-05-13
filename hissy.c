
#include <ncurses/curses.h>
#include <ncurses/ncurses.h>
#include <stdio.h>
#include <stdlib.h>

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

char *yellow = "\033[103m";
char *blue = "\033[104m";
char *reset = "\033[0m";

void draw(void) {
	clear();

	printw(" ");
	for (int i = 0; i < width; i++) {
		printw("__");
	}
	printw("_\n");

	for (int i = 0; i < height; i++) {
		printw("|");
		for (int j = 0; j < width; j++) {
			int has = 0;
			for (body *hiss_current = hiss; hiss_current != NULL; hiss_current = hiss_current->next) {
				if (i == hiss_current->pos.y && j == hiss_current->pos.x) {
					has = true;
					printw(" h");
					break;
				}
				if (hiss_current == hiss_tail) break;
			}
			if (i == food.y && j == food.x) printw(" o", yellow, reset);
			else if (!has) printw("  ");
		}
		printw(" |\n");
	}

	printw("|");
	for (int i = 0; i < width; i++) {
		printw("__");
	}
	printw("_|\n");
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
			body *hiss_next = malloc(sizeof(body));
			hiss_next->pos.x = hiss_tail->pos.x;
			hiss_next->pos.y = hiss_tail->pos.y;
			hiss_next->next = hiss;
			hiss_next->prev = hiss_tail;

			hiss->prev = hiss_next;

			hiss_tail->next = hiss_next;
			hiss_tail = hiss_next;

			place_food();
		}

		draw();
		iterations++;
	}

	endwin();

	free_hiss(hiss);

	return 0;
}