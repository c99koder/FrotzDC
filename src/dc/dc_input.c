/* dc_input.c
 * Copyright 2003, 2004 Sam Steele <sam.steele@gmail.com>.
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
#include <dc/vmu_pkg.h>
#include <GL/gl.h>
#include "font.h"
#include "input.h"
#include "window.h"
f_setup_t f_setup;

zchar os_read_key (int timeout, bool show_cursor)
{
  char c;
  set_show_cursor(show_cursor);
  if(timeout>=0) set_timeout(timeout);
  c=getch();
  return c;
}

zchar os_read_line (int max, zchar *buf, int timeout, int width, int continued)
{
  char c;
  int x,y;
  int i=0;
  int t=timeout*100;
  uint32 s,ms,tm;
  set_timeout(timeout);
  timer_ms_gettime(&s,&ms);

  i=strlen(buf);
  do {
    c=os_read_key(-1,1);
    if(c>=32 && c <= 176) c99_printf("%c",c);
    switch(c) {
    case 0:
    case 13:
      break;
    case 8:
      if(i>0) {
        i--;
        buf[i]='\0';
        getxy(&x,&y);
        locate(x-1,y);
      }
      break;
    default:
      buf[i]=c;
      i++;
      break;
    }
  } while (c!=13&&c!=0);
  buf[i]='\0';
  return c;
}

void os_more_prompt (void)
{
  c99_printf("[MORE]");
  video_refresh();
  getch();
}

void dc_init_input(void)
{
  h_flags |= UNDO_FLAG;

  if ((h_version >= V4))
    h_config |= CONFIG_TIMEDINPUT;

  if (h_version >= V5)
    h_flags &= ~(MOUSE_FLAG | MENU_FLAG);
}

zword os_read_mouse(void)
{
	/* NOT IMPLEMENTED */
}
