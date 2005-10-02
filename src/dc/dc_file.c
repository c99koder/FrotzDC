/* dc_file.c
 * Copyright 2003, 2004 Sam Steele <sam.steele@gmail.com>.
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
#include <dc/vmu_pkg.h>
#include <GL/gl.h>
#include "font.h"
#include "input.h"
#include "window.h"

#include "sav_icon.h"
f_setup_t f_setup;

char path[100];
int pathchanged=0;
int loopyloopy=0;

void doneClicked(int win_id, int but_id) {
  loopyloopy=0;
  printf("doneClicked\n");
}

void vmuClicked(int win_id, int but_id) {
  loopyloopy=0;
  strcpy(path,"/vmu/a1");
  pathchanged=1;
}

void cdClicked(int win_id, int but_id) {
  loopyloopy=0;
  strcpy(path,"/cd");
  pathchanged=1;
}

void pcClicked(int win_id, int but_id) {
  loopyloopy=0;
  strcpy(path,"/pc");
  pathchanged=1;
}

int filesel_input;

void listClick(int win_id, int list_id) {
  int x;
  struct C99ListBox *current=get_list(win_id,list_id);
  char tmp[100];
  strcpy(tmp,current->items[current->sel]);
  if(tmp[0]=='.' || tmp[strlen(tmp)-1]=='/') {
    if(tmp[0]=='.') {
      for(x=strlen(path)-1;x>=0;x--) {
        if(path[x]=='/') {
          path[x]='\0';
          break;
        }
      }
    } else {
      strcat(path,"/");
      tmp[strlen(tmp)-1]='\0';
      strcat(path,tmp);
    }
    pathchanged=1;
    loopyloopy=0;
    thd_sleep(100);
  }
}

void listChanged(int win_id, int list_id) {
  int x;
  struct C99ListBox *current=get_list(win_id,list_id);
  if(current->sel>=0) {
    set_inputbox_text(win_id,filesel_input,current->items[current->sel]);
  } else {
    set_inputbox_text(win_id,filesel_input,"");
  }
}

void vmuify(char *src, char *dst, char *sdesc, char *ldesc, char *appid) {
  file_t fd;
  int file_size=0;
  uint8 *data; //place to store the file in RAM  
  vmu_pkg_t pkg; //this is our VMU file header
  uint8 *pkg_out; //this will hold the file after the header is added
  int pkg_size; //this is the size of the file after the header is added 
  //load the file into RAM
  fd=fs_open(src,O_RDONLY);
  if(fd==0) {
    printf("File error (%s/%s)\n",fs_getwd(),src);
    return;
  }
  fs_seek(fd,0,SEEK_END); //seek to the end of the file
  file_size=fs_tell(fd);  //find out where we are and store it as the file size
  fs_seek(fd,0,SEEK_SET); //go back to the beginning of the file

  data=malloc(file_size+1); //allocate room to store the file in RAM
  fs_read(fd,data,file_size); //read the file into RAM
  fs_close(fd); 

  //write the file to VMU
  strcpy(pkg.desc_short, sdesc); //short description of the file
  strcpy(pkg.desc_long, ldesc); //longer description of the file
  strcpy(pkg.app_id, appid); //which app created this file
  pkg.icon_cnt = 1; //number of icons
  pkg.icon_anim_speed = 0; //speed the icons animate at
  memcpy(&pkg.icon_pal[0], sav_icon_data, 32);
  pkg.icon_data = sav_icon_data + 32;
  pkg.eyecatch_type = VMUPKG_EC_NONE; //which eyecatch to use
  pkg.data_len = file_size; //how big the file is without the header (from earlier)
  pkg.data = data; //data is our area in RAM where we stored our file
 
  vmu_pkg_build(&pkg, &pkg_out, &pkg_size); //tack on the header and compute the checksum.
 
  fs_unlink(dst);
  fd=fs_open(dst,O_WRONLY); //open our file on the VMU
  fs_write(fd,pkg_out,pkg_size); //write the file with the header added
  fs_close(fd);

  free(pkg_out);//clean up 
  free(data);
}


void copy_file(char *src, char *dst, int strip) {
  int fd1,fd2,l,cnt=0;
  char *buf;
  vmu_hdr_t vmuhdr;

  fs_unlink(dst);

  fd1=fs_open(src,O_RDONLY);
  if(fd1==0) {
    printf("File not found (%s)\n",src);
    return;
  }
  fd2=fs_open(dst,O_WRONLY);

  vmuhdr.data_len=0;

  if(strip) {
    fs_read(fd1,&vmuhdr,128);
    fs_seek(fd1,128+(vmuhdr.icon_cnt*512),SEEK_SET);
    printf("AppID: %s\n",vmuhdr.app_id);
    cnt=vmuhdr.data_len;
  } else {
    cnt=fs_total(fd1);
  }
  printf("File size: %i\n",cnt);
  buf=malloc(cnt+1);
  fs_read(fd1,buf,cnt);
  fs_write(fd2,buf,cnt);
  free(buf);
  fs_close(fd2);
  fs_close(fd1);
}


void select_file(char *filename, char *def, char *title, char *button, int flag) {
  dirent_t *de;
  uint32 d;
  int w,l,b,x,lab;
  char tmp[100];

  w=c99_create_window(320-((32*get_font_width()+4)/2),40,32*get_font_width()+4,292,title);
  c99_create_label(w,0,0,"Path:");
  lab=c99_create_label(w,6*get_font_width(),0,"/path/will/go/here");
  l=c99_create_list(w,2,18,32*get_font_width(),228);
  c99_list_set_changed(w,l,listChanged);
  c99_list_set_click(w,l,listClick);
  filesel_input=c99_create_inputbox(w,2,248,24*get_font_width(),20,def);
  b=c99_create_button(w,24*get_font_width()+4,248,7*get_font_width()+(get_font_width()-2),20,button);
  set_button_click(w,b,doneClicked);
  c99_button_set_def(w,b);
  b=c99_create_button(w,2,270,(32*get_font_width()-4)/3,20,"VMU");
  set_button_click(w,b,vmuClicked);
  b=c99_create_button(w,4+((32*get_font_width()-4)/3)*1,270,(32*get_font_width()-4)/3,20,"CD-ROM");
  set_button_click(w,b,cdClicked);
  b=c99_create_button(w,6+((32*get_font_width()-4)/3)*2,270,(32*get_font_width()-4)/3,20,"PC");
  set_button_click(w,b,pcClicked);

  strcpy(path,fs_getwd());
rescan:
  printf("Scanning: %s\n",path);
  c99_label_set_text(w,lab,path);
  c99_list_empty(w,l);
  c99_list_add_item(w,l,"..");
  pathchanged=0;
  //iso_ioctl(0,0,0);
  fs_chdir(path);
  d = fs_open(path,O_RDONLY|O_DIR);

  while ( (de=fs_readdir(d)) ) {
    printf("%s: %i\n",de->name,de->attr);
    if(de->name[0]!='.' && (de->attr==O_DIR ||
      de->name[strlen(de->name)-2]=='z' || de->name[strlen(de->name)-2]=='Z'||
      de->name[strlen(de->name)-3]=='s' || de->name[strlen(de->name)-2]=='S'||
      de->name[strlen(de->name)-3]=='d' || de->name[strlen(de->name)-2]=='D'
    )) {
      strcpy(tmp,de->name);
      if(de->attr==O_DIR) strcat(tmp,"/");
      c99_list_add_item(w,l,tmp);
    }
  }
  fs_close(d);

  loopyloopy=1;

  while(loopyloopy) {
    if(sys_render_begin()) { //make sure it's ok to draw (are we minimized?)
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
      //draw the text layer
      render_screen();
      render_windows();
      sys_render_finish();
    }
  }
  thd_sleep(100);
  if(pathchanged) goto rescan;
  strcpy(tmp,get_inputbox_text(w,filesel_input));
  if(tmp[0]=='/') {
    strcpy(path,tmp);
    goto rescan;
  }
  if(tmp[0]=='.' || tmp[strlen(tmp)-1]=='/') {
    if(tmp[0]=='.') {
      for(x=strlen(path)-1;x>=0;x--) {
        if(path[x]=='/') {
          path[x]='\0';
          break;
        }
      }
    } else {
      strcat(path,"/");
      tmp[strlen(tmp)-1]='\0';
      strcat(path,tmp);
    }
    set_inputbox_text(w,filesel_input,"");
    goto rescan;
  }
  fs_chdir(path);
  strcpy(filename,tmp);
  if(flag && path[1]=='v') {
    sprintf(tmp,"/ram/%s",filename);
    copy_file(filename,tmp,1);
    fs_chdir("/ram");
  }
  delete_window(w);
  printf("Returning: %s (%s)\n",filename,fs_getwd());
  return TRUE;    
}

int os_read_file_name (char *file_name, const char *default_name, int flag)
{
  char tmp[100];
  switch(flag) {
  case FILE_SAVE:
  case FILE_SAVE_AUX:
    fs_chdir("/vmu/a1");
    select_file(file_name,default_name,"Select a file","Save",0);
    break;
  default:
    fs_chdir("/vmu/a1");
    select_file(file_name,default_name,"Select a file","Load",1);
    break;
  }
  /*fs_chdir("/vmu/a1");

    print_string ("Enter a file name.\nDefault is \"");
    print_string (default_name);
    print_string ("\": ");

    read_string (MAX_FILE_NAME, file_name);
*/
    /* Use the default name if nothing was typed */

    if (file_name[0] == 0)
        strcpy (file_name, default_name);
/*  if(flag==FILE_RESTORE) {
    sprintf(tmp,"/ram/%s",file_name);
    copy_file(file_name,tmp,1);
    fs_chdir("/ram");
  }*/
  return TRUE;
}

