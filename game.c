#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <math.h>

#include "game.h"

#define die(error, ...) _die(error, __FILE__, __LINE__, __VA_ARGS__)
#define MIN(x, y) (x < y ? x : y)

void _die(int error, char* filename, int line, const char* format, ...)
{
	va_list args;

	fprintf(stderr, "[%s:%d] ", filename, line);
	va_start(args, format);
	vfprintf(stderr, format, args);
	va_end(args);
	fprintf(stderr, "\n");
	exit(error);
}

void game_init(int w, int h, int fps)
{
	if (!al_init())
		die(1, "can't init allegro\n");
	if (!(al_init_primitives_addon()))
		die(1, "can't init primitives addon\n");
	if (!(G.display = al_create_display(w, h)))
		die(2, "can't create display");
	if (!(G.ev = al_create_event_queue()))
		die(3, "can't create event queue");
	if (!(G.tick = al_create_timer(1.0 / fps)))
		die(4, "invalid fps");

	/* current position is the begining */
	G.gi.cur[0] = G.gi.start[0];
	G.gi.cur[1] = G.gi.start[1];

	G.w = w;
	G.h = h;
	al_register_event_source(G.ev, al_get_display_event_source(G.display));
	al_register_event_source(G.ev, al_get_timer_event_source(G.tick));
	al_start_timer(G.tick);
}

void game_render(void)
{
	ALLEGRO_COLOR colors[] = {
		al_map_rgb(0, 0, 0),		/* x */
		al_map_rgb(0, 128, 128),	/* s */
		al_map_rgb(128, 128, 128),	/* a */
		al_map_rgb(128, 128, 128),	/* r */
		al_map_rgb(128, 128, 128),	/* p */
	};
	int i, j, scale = MIN(G.w / G.gi.map_size[0], G.h / G.gi.map_size[1]);
	al_clear_to_color(al_map_rgb(0,0,0));

	/* tiles */
	for (i=0; i<G.gi.map_size[0]; i++) {
		for (j=0; j<G.gi.map_size[1]; j++) {
			int map_index = i + j * G.gi.map_size[1];
			al_draw_filled_rectangle(scale*i, scale*j,
					scale*(i + 1), scale*(j + 1),
					colors[G.gi.mapv[map_index]]);
		}
	}

	/* prize */
	for (i=0; i<G.gi.num_prizes; i++) {
		float x = G.gi.prizes[i][0] + 0.5;
		float y = G.gi.prizes[i][1] + 0.5;
		al_draw_filled_triangle(scale*(x), scale*(y-0.3),
				scale*(x+0.3), scale*(y+0.3),
				scale*(x-0.3), scale*(y+0.3),
				al_map_rgb(255, 255, 0));
	}

	/* start */
	al_draw_filled_circle((float)scale * (G.gi.start[0]+0.5),
			(float)scale * (G.gi.start[1]+0.5), scale/2,
			al_map_rgb(0, 255, 0));
	/* end */
	al_draw_filled_circle((float)scale * (G.gi.end[0]+0.5),
			(float)scale * (G.gi.end[1]+0.5), scale/2,
			al_map_rgb(255, 0, 0));
	/* player */
	al_draw_filled_circle((float)scale * (G.gi.cur[0]+0.5),
			(float)scale * (G.gi.cur[1]+0.5), scale/2,
			al_map_rgb(0, 0, 255));
	al_flip_display();
}

void game_loop(void)
{
	int done = 0;
	while (!done){
		ALLEGRO_EVENT ev;
		al_wait_for_event(G.ev, &ev);
		game_render();
	}
}

void game_end(void)
{
	al_destroy_display(G.display);
	al_destroy_timer(G.tick);
	al_destroy_event_queue(G.ev);
}
