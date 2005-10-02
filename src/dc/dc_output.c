/* dc_output.c
 * (C) 2003, 2004 Sam Steele <sam.steele@gmail.com>
 *
 * This file is part of Frotz.
 *
 * Frotz is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * Frotz is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA
 */

#include "dc_frotz.h"

int current_style;

int fg,bg;

f_setup_t f_setup;

static int cursor_row = 0, cursor_col = 0;

int os_char_width (zchar z)
{
  return 1;
}

int os_string_width (const zchar *s)
{
  int width = 0;
  zchar c;

  while ((c = *s++) != 0)
    if (c == ZC_NEW_STYLE || c == ZC_NEW_FONT)
      s++;
    else
      width += os_char_width(c);

  return width;
}

void os_set_cursor(int row, int col)
{
  cursor_row = row - 1; cursor_col = col - 1;
  if (cursor_row >= h_screen_rows)
    cursor_row = h_screen_rows - 1;
  locate(cursor_col,cursor_row);
}

void os_set_text_style(int x)
{
  if(x & REVERSE_STYLE) {
    color(bg,fg);
  } else if (x & BOLDFACE_STYLE) {
    color(15,bg);
  } else {
    color(fg,bg);
  }
}

void os_display_char (zchar c)
{
  locate(cursor_col,cursor_row);
  if (c >= ZC_LATIN1_MIN && c <= ZC_LATIN1_MAX) {
    c99_printf("%c",c);
  } else if (c >= 32 && c <= 126) {
    c99_printf("%c",c);
  } else if (c == ZC_GAP) {
    c99_printf("  ");
  } else if (c == ZC_INDENT) {
    c99_printf("   ");
  }
  cursor_col++;
  if (cursor_col >= h_screen_cols) {
    cursor_col = 0; cursor_row++;
  }
  return;
}


/* Haxor your boxor? */
void os_display_string (const zchar *s)
{
  zchar c;
  while ((c = *s++) != 0)
    if (c == ZC_NEW_FONT)
      s++;
    else if (c == ZC_NEW_STYLE)
      os_set_text_style(*s++);
    else {
     os_display_char (c); 
     }
}

void os_erase_area (int top, int left, int bottom, int right)
{
  int row, col;
  top--; left--; bottom; right;
  clear_area(left,top,right,bottom);
}

void os_scroll_area (int top, int left, int bottom, int right, int units)
{
  int row, col;
  top--; left--; bottom; right;
  scroll_text(left,top,right,bottom,units);
  video_refresh();
}

int os_font_data(int font, int *height, int *width)
{
    if (font == TEXT_FONT) {
      *height = 1; *width = 1; return 1;
    }
    return 0;
}

int frotz_to_cbmcolor(int frotz) {
  switch(frotz) {
    case BLACK_COLOUR:
      return 0;
    case RED_COLOUR:
      return 12;
    case GREEN_COLOUR:
      return 10;
    case YELLOW_COLOUR:
      return 14;
    case BLUE_COLOUR:
      return 9;
    case MAGENTA_COLOUR:
      return 15;
    case CYAN_COLOUR:
      return 11;
    case WHITE_COLOUR:
      return 15;
    case GREY_COLOUR:
      return 7;
    default:
      return 7;
  }
}

int cbmcolor_to_frotz(int cbm) {
  switch(cbm) {
    case 0:
      return BLACK_COLOUR;
    case 12:
      return RED_COLOUR;
    case 10:
      return GREEN_COLOUR;
    case 14:
      return YELLOW_COLOUR;
    case 9:
      return BLUE_COLOUR;
    case 13:
      return MAGENTA_COLOUR;
    case 11:
      return CYAN_COLOUR;
    case 15:
      return WHITE_COLOUR;
    case 7:
      return GREY_COLOUR;
    default:
      return WHITE_COLOUR;
  }
}

void os_set_colour (int new_fg, int new_bg) {
  fg=frotz_to_cbmcolor(new_fg);
  bg=frotz_to_cbmcolor(new_bg);
  color(fg,bg);
}
void os_set_font (int x) {}

void os_reset_screen(void)
{
 
}

void dc_init_output(void)
{
  if (h_version == V3) {
    h_config |= CONFIG_SPLITSCREEN;
    h_flags &= OLD_SOUND_FLAG;
  }

  h_flags |= SOUND_FLAG;

  h_screen_height = h_screen_rows;
  h_screen_width = h_screen_cols;

  h_font_width = 1; h_font_height = 1;

  h_config |= CONFIG_COLOUR;
  h_config |= CONFIG_BOLDFACE;
  h_flags |= COLOUR_FLAG; /* FIXME: beyond zork handling? */
  os_erase_area(1, 1, h_screen_rows, h_screen_cols);
}
