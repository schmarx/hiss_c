
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
} body;

body hiss;
vect food;
vect velocity;

int iterations = 0;

void draw(void) {
	clear();
	printw("%i head: (%i, %i)\n", iterations, hiss.pos.x, hiss.pos.y);

	printw(" ");
	for (int i = 0; i < width; i++) {
		printw("__");
	}
	printw("_\n");

	for (int i = 0; i < height; i++) {
		printw("|");
		for (int j = 0; j < width; j++) {
			if (i == hiss.pos.y && j == hiss.pos.x) printw(" h");
			else if (i == food.y && j == food.x) printw(" 1");
			else printw("  ");
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

int main(int argc, char *argv[]) {
	initscr();
	noecho();
	srand(clock());

	hiss.pos.x = 0;
	hiss.pos.y = 0;

	place_food();

	int x = 0;
	int y = 0;

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

		hiss.pos.x += velocity.x;
		hiss.pos.y += velocity.y;

		if (hiss.pos.x < 0) hiss.pos.x = width - 1;
		if (hiss.pos.y < 0) hiss.pos.y = height - 1;
		if (hiss.pos.x >= width) hiss.pos.x = 0;
		if (hiss.pos.y >= height) hiss.pos.y = 0;

		if (hiss.pos.x == food.x && hiss.pos.y == food.y) {

			place_food();
		}

		draw();
		iterations++;
	}

	endwin();

	free_hiss(hiss);

	return 0;
}