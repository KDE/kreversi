/* Yo Emacs, this -*- C++ -*-ud
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
#include <kapplication.h>
#include <kstandarddirs.h>


#ifdef HAVE_CONFIG_H
#include <config.h>
#endif


bool audio = 0;


bool initAudio() {
    audio = true;
    return TRUE;
}

bool playSound(const char *s) {
  // look in SOUNDDIR
  KAudioPlayer::play(locate("data", QString("kreversi/sounds/")+s));
  return TRUE;
}

bool soundSync() {
  return (bool)(audio != 0);
}

bool audioOK() {
  return (bool)(audio != 0);
}

bool doneAudio() {
    audio = 0;
    return TRUE;
}

bool syncPlaySound(const char *s) {
  // to be done
  return playSound(s);
}

