/* dc_init.c
 * (C) 2003, 2004 Sam Steele <sam.steele@gmail.com>
 *
 * Based on dumb-init.c:
 * Copyright 1997,1998 Alva Petrofsky <alva@petrofsky.berkeley.ca.us>.
 * Any use permitted provided this notice stays intact.
 */

#include "dc_frotz.h"
#include <kos.h>
#include <font.h>
#include <sys.h>
#include <screen.h>
#include "input.h"
#include "ini_icon.h"

f_setup_t f_setup;

static int user_screen_width = 80;
static int user_screen_height = 25;
static int user_interpreter_number = -1;
static int user_random_seed = -1;
static int user_tandy_bit = 0;
static char *graphics_filename = NULL;
static bool plain_ascii = FALSE;

extern char script_name[];
extern char command_name[];
extern char save_name[];
extern char auxilary_name[];

int scheme_fg,scheme_bg,scheme_bd,scheme_bold;

void save_vid_config();
void scheme_config();

void load_vid_config() {
  int fd;
  vmu_hdr_t hdr;
  fd=fs_open("/vmu/a1/frotz.ini",O_RDONLY);
  if(fd!=-1) {
    fs_read(fd,&hdr,sizeof(hdr)); //header
    fs_seek(fd,128+(hdr.icon_cnt*512),SEEK_SET);
    fs_read(fd,&scheme_fg,sizeof(int));
    fs_read(fd,&scheme_bg,sizeof(int));
    fs_read(fd,&scheme_bd,sizeof(int));
    fs_read(fd,&scheme_bold,sizeof(int));
    fs_read(fd,&user_screen_width,sizeof(int));
    fs_close(fd);
    printf("%i %i %i %i %i %i\n",fd,scheme_fg, scheme_bg, scheme_bd, scheme_bold, user_screen_width);
    user_screen_height=25;
  } else {
    scheme_config();
    save_vid_config();
  }
}

void scheme_config() {
  int dirty=1,loop=1,sel=0,fd=0;
  resize_screen(80,24,8,16);
  color(15,0);
  set_border(4);
  clear_screen();
  c99_printf("\n\n\n\n");
  c99_printf("                             Color scheme selector\n\n");
  c99_printf("                               1. ");
  color(7,0);
  c99_printf("Monochrome\n");
  color(15,0);
  c99_printf("                               2. ");
  color(9,1);
  c99_printf("Commodore 64\n");
  color(15,0);
  c99_printf("                               3. ");
  color(10,0);
  c99_printf("The Matrix\n");
  color(15,0);
  c99_printf("                               4. ");
  color(9,0);
  c99_printf("The Blues\n");
  color(15,0);
  c99_printf("                               5. ");
  color(12,0);
  c99_printf("Redness\n");
  color(15,0);
  c99_printf("\n\n\n");
  c99_printf("\n");
  c99_printf("          Use up and down on the d-pad or keyboard to select a scheme\n");
  c99_printf("                      Press start or enter to continue\n");

  while(loop) {
    if(dirty) {
      locate(26,sel+1+5);
      c99_printf("   >");
      locate(0,12);
      if(user_screen_width==80) {
        c99_printf("                 > 80 Column Mode            40 Column Mode\n");
      } else {
        c99_printf("                   80 Column Mode          > 40 Column Mode\n");
      }
      video_refresh();
      thd_sleep(100);
    }
    switch(poll_game_device(0)) {
    case START_BTN:
      loop=0;
      break;
    case MOVE_LEFT:
      user_screen_width=80;
      dirty=1;
      break;
    case MOVE_RIGHT:
      user_screen_width=40;
      dirty=1;
      break;
    case MOVE_UP:
      sel--;
      if(sel<0) { sel=0; break; }
      dirty=1;
      locate(26,sel+2+5);
      c99_printf("    ");
      break;
    case MOVE_DOWN:
      sel++;
      if(sel>4) { sel=4; break; }
      dirty=1;
      locate(26,sel+5);
      c99_printf("    ");
      break;
    }
    thd_sleep(50);
    //delay(.05);
  }
  switch(sel) {
  case 0:
    scheme_fg=7;
    scheme_bg=0;
    scheme_bd=0;
    scheme_bold=15;
    break;
  case 1:
    scheme_fg=9;
    scheme_bg=1;
    scheme_bd=9;
    scheme_bold=15;
    break;
  case 2:
    scheme_fg=10;
    scheme_bg=0;
    scheme_bd=0;
    scheme_bold=15;
    break;
  case 3:
    scheme_fg=9;
    scheme_bg=0;
    scheme_bd=0;
    scheme_bold=15;
    break;
  case 4:
    scheme_fg=12;
    scheme_bg=0;
    scheme_bd=0;
    scheme_bold=15;
    break;
  }
}

void save_vid_config() {
  int loop=1,fd,pkg_size;
  uint8 data[250], *pkg_out;
  vmu_pkg_t pkg;

  strcpy(pkg.desc_short,"frotz.ini");
  strcpy(pkg.desc_long,"FrotzDC Config File");
  strcpy(pkg.app_id,"FrotzDC");
  pkg.icon_cnt=1;
  pkg.icon_anim_speed=0;
  memcpy(&pkg.icon_pal[0], ini_icon_data, 32);
  pkg.icon_data = ini_icon_data + 32;
  pkg.eyecatch_type = VMUPKG_EC_NONE;
  pkg.data_len = (5*sizeof(float))+(4*sizeof(int));
  pkg.data=data;

  memcpy(data+(sizeof(int)*0),&scheme_fg,sizeof(int));
  memcpy(data+(sizeof(int)*1),&scheme_bg,sizeof(int));
  memcpy(data+(sizeof(int)*2),&scheme_bd,sizeof(int));
  memcpy(data+(sizeof(int)*3),&scheme_bold,sizeof(int));
  memcpy(data+(sizeof(int)*4),&user_screen_width,sizeof(int));
  vmu_pkg_build(&pkg, &pkg_out, &pkg_size);

  fs_unlink("/vmu/a1/frotz.ini");
  fd=fs_open("/vmu/a1/frotz.ini",O_WRONLY);

  if(!fd) {
    color(15,0);
    c99_printf("Config file save failure!");
    thd_sleep(2000);
    //delay(2);
  } else {
    fs_write(fd,pkg_out,pkg_size);
    fs_close(fd);
  }
}

void os_process_arguments(int argc, char *argv[]) 
{
}

void os_init_screen(void)
{
  if (h_version == V3 && user_tandy_bit)
      h_config |= CONFIG_TANDY;

  if (h_version >= V5 && f_setup.undo_slots == 0)
      h_flags &= ~UNDO_FLAG;

  if (user_interpreter_number > 0)
    h_interpreter_number = user_interpreter_number;
  else {
    /* Use ms-dos for v6 (because that's what most people have the
     * graphics files for), but don't use it for v5 (or Beyond Zork
     * will try to use funky characters).  */
    h_interpreter_number = h_version == 6 ? INTERP_MSDOS : INTERP_DEC_20;
  }
  h_interpreter_version = 'F';

  set_border(scheme_bd);
  set_do_scroll(0);
  set_do_wrap(0);
  set_echo(0);
  dc_init_output();
  dc_init_input();
  //dumb_init_pictures(graphics_filename);
}

int os_random_seed (void)
{
    return (time(0) & 0x7fff);
}

void os_restart_game (int stage) {}

void os_fatal (const char *s)
{
  printf("\nFatal error: %s\n", s);
  exit(0);
}

FILE *os_path_open(const char *name, const char *mode)
{
	FILE *fp;

	fp = fopen(name, mode);
  return fp;
}

void os_init_setup(void)
{
  char stripped_story_name[100];
  int i;

  c99_sys_init("FrotzDC");
  snd_stream_init();
  mp3_init(); //for sound effects
  arch_set_exit_path(ARCH_EXIT_MENU);
  fs_chdir("/cd");
  load_font("hybrid.fnt");
  init_screen();
  //fs_unlink("/vmu/a1/frotz.ini");
  load_vid_config();
  h_default_foreground=cbmcolor_to_frotz(scheme_fg);
  h_default_background=cbmcolor_to_frotz(scheme_bg);

  h_screen_rows = user_screen_height;
  h_screen_cols = user_screen_width;

  if(h_screen_cols > 40)
    c99_resize_screen(h_screen_cols,h_screen_rows,8,16);
  else
    c99_resize_screen(h_screen_cols,h_screen_rows,16,16);

  set_border(scheme_bd);
  color(scheme_fg,scheme_bg);
  set_do_scroll(0);
  clear_screen();
  color(15,scheme_bg);
  c99_printf("FrotzDC 1.1");
  color(scheme_fg,scheme_bg);
  c99_printf(" by Sam Steele.\nhttp://www.c99.org/dc/");
  locate(0,22);
  c99_printf("Based on ");
  color(15,scheme_bg);
  c99_printf("UNIX Frotz 2.43");
  color(scheme_fg,scheme_bg);
  c99_printf(" by\nJim Dunleavy and David Griffith.");

  story_name=malloc(100);
  select_file(story_name,"","Select a game","Open",1);

  strcpy(stripped_story_name,story_name);
  for(i=0;i<strlen(stripped_story_name);i++) {
    if(stripped_story_name[i]=='.') {
      stripped_story_name[i]='\0';
      break;
    }
  }


    /* Create nice default file names */

    strcpy (script_name, stripped_story_name);
    strcpy (command_name, stripped_story_name);
    strcpy (save_name, stripped_story_name);
    strcpy (auxilary_name, stripped_story_name);

    /* Don't forget the extensions */

    strcat (script_name, ".scr");
    strcat (command_name, ".rec");
    strcat (save_name, ".sav");
    strcat (auxilary_name, ".aux");  

	f_setup.attribute_assignment = 0;
	f_setup.attribute_testing = 0;
	f_setup.context_lines = 0;
	f_setup.object_locating = 0;
	f_setup.object_movement = 0;
	f_setup.left_margin = 0;
	f_setup.right_margin = 0;
	f_setup.ignore_errors = 0;
	f_setup.piracy = 0;
	f_setup.undo_slots = MAX_UNDO_SLOTS;
	f_setup.expand_abbreviations = 0;
	f_setup.script_cols = 80;
	f_setup.save_quetzal = 1;
	f_setup.sound = 1;
	f_setup.err_report_mode = 0;
}
