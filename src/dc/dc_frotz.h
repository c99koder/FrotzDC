/* dc_frotz.h
 * Frotz os functions for the Sega Dreamcast
 * (C) 2003, 2004 Sam Steele <sam.steele@gmail.com>
 *
 * based on dumb-frotz.h:
 * Copyright 1997, 1998 Alembic Petrofsky <alembic@petrofsky.berkeley.ca.us>.
 * Any use permitted provided this notice stays intact.
 */
#include "../common/frotz.h"

#define FILENAME_MAX 128

/* from ../common/setup.h */
extern f_setup_t f_setup;

/* From input.c.  */
bool is_terminator (zchar);

/* dumb-input.c */
void dc_init_input(void);

/* dumb-output.c */
void dc_init_output(void);

/* dumb-pic.c */
void dumb_init_pictures(char *graphics_filename);
