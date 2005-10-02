/* dc_sample.c
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
#include <kos.h>

typedef struct snd_effect {
        uint32  locl, locr;
        uint32  len;
        uint32  rate;
        uint32  used;
        int     stereo;
        uint32  fmt;

        LIST_ENTRY(snd_effect)  list;
} snd_effect_t;

/*
 * os_beep
 *
 * Play a beep sound. Ideally, the sound should be high- (number == 1)
 * or low-pitched (number == 2).
 *
 */

void os_beep (int number)
{
}/* os_beep */

/*
 * os_prepare_sample
 *
 * Load the sample from the disk.
 *
 */
sfxhnd_t sounds[20]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
void os_prepare_sample (int number)
{
  char fname[100];
  char st[100];
  int i;
  for(i=0;i<strlen(story_name);i++) {
    if(story_name[i]=='.') break;
    st[i]=story_name[i];
  }
  st[i]='\0';
  if(number<10) {
    sprintf(fname,"%s/%s0%i.wav",st,st,number);
  } else {
    sprintf(fname,"%s/%s%i.wav",st,st,number);
  }
  printf("Sound: %s\n",fname);
  sounds[number]=snd_sfx_load(fname);
}/* os_prepare_sample */

/*
 * os_start_sample
 *
 * Play the given sample at the given volume (ranging from 1 to 8 and
 * 255 meaning a default volume). The sound is played once or several
 * times in the background (255 meaning forever). In Z-code 3 the
 * repeats value is always 0 and the number of repeats is taken from
 * the sound file itself. The end_of_sound function is called as soon
 * as the sound finishes.
 *
 */

int cursamp=0;
int lcnt=0;

void sndthd(void *v) {
  int s=cursamp;
  int c=lcnt;
  int i;
  snd_effect_t * t = (snd_effect_t *)sounds[s];

  for(i=0;i<c;i++) {
    snd_sfx_play(sounds[s],128,128);
    thd_sleep(((float)t->len/(float)t->rate)*1000.0f);
  }
}

void os_start_sample (int number, int volume, int repeats, zword eos)
{
  char fname[100];
  char st[100];
  int i;
  kthread_t * t1;
  if(sounds[number]==0) {
    for(i=0;i<strlen(story_name);i++) {
      if(story_name[i]=='.') break;
      st[i]=story_name[i];
    }
    st[i]='\0';
    if(number<10) {
      sprintf(fname,"%s/%s0%i.wav",st,st,number);
    } else {
      sprintf(fname,"%s/%s%i.wav",st,st,number);
    }
    printf("Sound: %s\n",fname);
    sounds[number]=snd_sfx_load(fname);
  }
  cursamp=number;
  lcnt=repeats;
  t1=thd_create(sndthd,NULL);
}/* os_start_sample */

/*
 * os_stop_sample
 *
 * Turn off the current sample.
 *
 */

void os_stop_sample (int blah)
{

    /* Not implemented */

}/* os_stop_sample */

/*
 * os_finish_with_sample
 *
 * Remove the current sample from memory (if any).
 *
 */

void os_finish_with_sample (int blah)
{
    /* Not implemented */

}/* os_finish_with_sample */

/*
 * os_wait_sample
 *
 * Stop repeating the current sample and wait until it finishes.
 *
 */

void os_wait_sample (void)
{

    /* Not implemented */

}/* os_wait_sample */
