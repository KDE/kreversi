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

#ifndef __APP__H__
#define __APP__H__

#include "misc.h"
#include <kstatusbar.h>
#include <ktoolbar.h>
#include <ktopwidget.h>
#include <kmenubar.h>
#include <qarray.h>
#include <time.h>
#include <qlist.h>
#include <qfileinf.h>
#include <qdialog.h>
#include <qlined.h>
#include "board.h"
#include <klocale.h>

const int HIGHSCORE_MAX = 10;

struct HighScore {
  char name[32];
  int  color;
  int  winner;
  int  loser;
  float rating;
  time_t date;
};


class App : public KTopLevelWidget {
  Q_OBJECT
public:
  App();
  ~App();

  void recover();

private:
  void createMenuBar();
  void createToolBar();
  void createStatusBar();
  void enableItems();
  void readHighscore();
  void writeHighscore();
  int  insertHighscore(HighScore &);
  bool isBetter(HighScore &, HighScore &);
  void showHighscore(int focusitem = -1);
  QString getPlayerName();
  void lookupBackgroundPixmaps();

  virtual void saveProperties(KConfig *);
  virtual void readProperties(KConfig *);

public slots:
  void processEvent(int itemid);
  void slotScore();
  void slotGameEnded(int);
  void slotTurn(int);
  void slotStrength(int);
  void slotStatusChange(int);
  void slotIllegalMove();
  void slotBarChanged();

private:
  Board *b;
  KMenuBar *menu;
  KToolBar *tb;
  KStatusBar *sb;
  QArray<HighScore> highscore;
  QList<QFileInfo> backgroundPixmaps;
  QPixmap wpm, bpm;
  KLocale *locale;
};

#endif
