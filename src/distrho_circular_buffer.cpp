/*
  circularbuffer.c:

  Copyright (C) 2012 Victor Lazzarini

  This file is part of Csound.

  The Csound Library is free software; you can redistribute it
  and/or modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  Csound is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with Csound; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
  02110-1301 USA
*/

#include <stdio.h>
#include <string.h>


typedef struct _distrho_circular_buffer {
  char *buffer;
  int  wp;
  int rp;
  int numelem;
  int elemsize; /* in number of bytes */
} distrho_circular_buffer;

void *distrhoCreateCircularBuffer(int numelem, int elemsize){
    distrho_circular_buffer *p;
    if ((p = (distrho_circular_buffer *)
         operator new(sizeof(distrho_circular_buffer))) == NULL) {
      return NULL;
    }
    p->numelem = numelem;
    p->wp = p->rp = 0;
    p->elemsize = elemsize;

    if ((p->buffer = (char *) operator new(numelem*elemsize)) == NULL) {
      return NULL;
    }
    memset(p->buffer, 0, numelem*elemsize);
    printf("numelem %d \n", numelem);
    return (void *)p;
}

int checkspace(distrho_circular_buffer *p, int writeCheck){
    int wp = p->wp, rp = p->rp, numelem = p->numelem;
    if(writeCheck){
      if (wp > rp) return rp - wp + numelem - 1;
      else if (wp < rp) return rp - wp - 1;
      else return numelem - 1;
    }
    else {
      if (wp > rp) return wp - rp;
      else if (wp < rp) return wp - rp + numelem;
      else return 0;
    }
}

int distrhoReadCircularBuffer(void *p, void *out, int items)
{
    if (p == NULL) return 0;
    {
      int remaining;
      int itemsread, numelem = ((distrho_circular_buffer *)p)->numelem;
      int elemsize = ((distrho_circular_buffer *)p)->elemsize;
      int i=0, rp = ((distrho_circular_buffer *)p)->rp;
      char *buffer = ((distrho_circular_buffer *)p)->buffer;
      if ((remaining = checkspace((distrho_circular_buffer *)p, 0)) == 0) {
        return 0;
      }
      itemsread = items > remaining ? remaining : items;
      for (i=0; i < itemsread; i++){
        memcpy((char *) out + (i * elemsize),
               &(buffer[elemsize * rp++]),  elemsize);
        if (rp == numelem) {
          rp = 0;
        }
      }
      ((distrho_circular_buffer *)p)->rp = rp;
      return itemsread;
    }
}

int distrhoPeekCircularBuffer(void *p, void *out, int items)
{
    if (p == NULL) return 0;
    int remaining;
    int itemsread, numelem = ((distrho_circular_buffer *)p)->numelem;
    int elemsize = ((distrho_circular_buffer *)p)->elemsize;
    int i=0, rp = ((distrho_circular_buffer *)p)->rp;
    char *buffer = ((distrho_circular_buffer *)p)->buffer;
    if ((remaining = checkspace((distrho_circular_buffer *)p, 0)) == 0) {
        return 0;
    }
    itemsread = items > remaining ? remaining : items;
    for(i=0; i < itemsread; i++){
        memcpy((char *) out + (i * elemsize),
               &(buffer[elemsize * rp++]),  elemsize);
        if (rp == numelem) {
            rp = 0;
        }
    }
    return itemsread;
}

void distrhoFlushCircularBuffer(void *p)
{
    if (p == NULL) return;
    int remaining;
    int itemsread, numelem = ((distrho_circular_buffer *)p)->numelem;
    int i=0, rp = ((distrho_circular_buffer *)p)->rp;
    //MYFLT *buffer = ((distrho_circular_buffer *)p)->buffer;
    if ((remaining = checkspace((distrho_circular_buffer *)p, 0)) == 0) {
        return;
    }
    itemsread = numelem > remaining ? remaining: numelem;
    for (i=0; i < itemsread; i++){
        rp++;
        if(rp == numelem) rp = 0;
    }
    ((distrho_circular_buffer *)p)->rp = rp;
}


int distrhoWriteCircularBuffer(void *p, const void *in, int items)
{
    if (p == NULL) return 0;
    int remaining;
    int itemswrite, numelem = ((distrho_circular_buffer *)p)->numelem;
    int elemsize = ((distrho_circular_buffer *)p)->elemsize;
    int i=0, wp = ((distrho_circular_buffer *)p)->wp;
    char *buffer = ((distrho_circular_buffer *)p)->buffer;
    if ((remaining = checkspace((distrho_circular_buffer *)p, 1)) == 0) {
        return 0;
    }
    itemswrite = items > remaining ? remaining : items;
    for(i=0; i < itemswrite; i++){
        memcpy(&(buffer[elemsize * wp++]),
                ((char *) in) + (i * elemsize),  elemsize);
        if(wp == numelem) wp = 0;
    }
    ((distrho_circular_buffer *)p)->wp = wp;
    return itemswrite;
}

void distrhoDestroyCircularBuffer(void *p){
    if(p == NULL) return;
    operator delete(((distrho_circular_buffer *)p)->buffer);
    operator delete(p);
}
