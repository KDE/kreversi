/* Yo Emacs, this -*- C++ -*-
 *******************************************************************
 *******************************************************************
 *
 *
 * KREVERSI
 *
 *
 *******************************************************************
 *
 * A Reversi (or sometimes called Othello) game
 *
 *******************************************************************
 *
 * created 1997 by Mario Weilguni <mweilguni@sime.com>
 *
 *******************************************************************
 *
 * This file is part of the KDE project "KREVERSI"
 *
 * KREVERSI is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * KREVERSI is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with KREVERSI; see the file COPYING.  If not, write to
 * the Free Software Foundation, 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 *
 *******************************************************************
 */

#include "playsound.h"
#include <stdlib.h>
#include <kapp.h>

QString SOUNDDIR;

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifdef HAVE_MEDIATOOL

KAudio *audio = 0;

bool initAudio() {
  if(audio == 0) {
    audio = new KAudio();
    SOUNDDIR = kapp->kde_datadir() + "/kreversi/sounds/";

    if(audio == 0)
      return FALSE;

    if(audio->serverStatus() != 0) {
      audio = 0;
      return FALSE;
    }

    return TRUE;
  } else
    return FALSE;
}

bool playSound(const char *s) {
  if(!audio)
    return FALSE;
  
  // look in SOUNDDIR
  if(audio->play((char *)(SOUNDDIR + s).data())) {

    return TRUE;
  }
    
  return FALSE;
}

bool soundSync() {
//   if(audio)
//     audio->sync();

  return (bool)(audio != 0);
}

bool audioOK() {
  return (bool)(audio != 0);
}

bool doneAudio() {
  if(audio) {
    delete audio;
    audio = 0;
    return TRUE;
  } else
    return FALSE;
}

bool syncPlaySound(const char *s) {
  // to be done
  return playSound(s);
}

#else
bool initAudio() {return FALSE;}
bool doneAudio() {return FALSE;}
bool playSound(const char *) {return FALSE;}
bool syncPlaySound(const char *) {return FALSE;}
bool soundSync() {return FALSE;}
bool audioOK() {
  return FALSE;
}

#endif
