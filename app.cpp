/* Yo Emacs, this is -*- C++ -*-
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

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <qdir.h>
#include <qaccel.h>
#include <qpopupmenu.h>
#include <qpushbutton.h>
#include <qlistbox.h>
#include <qdatetime.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qregexp.h>
#include <qtimer.h>
#include <qlayout.h>

#include <kapplication.h>
#include <kdebug.h>
#include <kconfig.h>
#include <kcolordialog.h>
#include <kseparator.h>
#include <kmessagebox.h>
#include <kstandarddirs.h>
#include <kglobal.h>
#include <kiconloader.h>
#include <khelpmenu.h>
#include <kpopupmenu.h>

#include <config.h>

#include "Score.h"
#include "app.h"
#include "playsound.h"
#include "version.h"

#define APPDATA(x) KGlobal::dirs()->findResource("appdata", x)
#define PICDATA(x) KGlobal::dirs()->findResource("appdata", QString("pics/")+ x)

const int ID_FSAVE	= 101;
const int ID_FLOAD	= 102;
const int ID_FQUIT	= 104;

const int ID_GSTOP	= 200;
const int ID_GCONTINUE	= 201;
const int ID_GNEW	= 202;
const int ID_GUNDO	= 203;
const int ID_GSWITCH	= 204;
const int ID_GHIGHSCORES= 205;

const int ID_VZOOMIN	= 400;
const int ID_VZOOMOUT	= 401;
const int ID_VZOOMBASE	= 1000;
const int ID_VZOOM50	= 1050;
const int ID_VZOOM60	= 1060;
const int ID_VZOOM80	= 1080;
const int ID_VZOOM100	= 1100;
const int ID_VZOOM120	= 1120;
const int ID_VZOOM140	= 1140;
const int ID_VZOOM160	= 1160;
const int ID_VZOOM180	= 1180;
const int ID_VZOOM200	= 1200;

const int ID_OBASE	= 500;
const int ID_O2		= 502;
const int ID_O3		= 503;
const int ID_O4		= 504;
const int ID_O5		= 505;
const int ID_O6		= 506;
const int ID_O7		= 507;
const int ID_O8		= 508;
const int ID_OANIMATION = 509;
const int ID_OSPEED     = 510;
const int ID_COLOR	= 549;
const int ID_PIXMAP	= 550;
const int ID_OSOUND	= 580;
const int ID_OGSCALE	= 581;

const int ID_HCONTENTS	= 900;
const int ID_HABOUT	= 902;
const int ID_HHINT	= 903;
const int ID_HRULES	= 904;
const int ID_HSTRATEGY	= 905;
const int ID_HABOUTQT	= 906;
const int ID_HABOUTKDE	= 907;

const int SB_SCOREH	= 2;
const int SB_SCOREC	= 3;
const int SB_TURN       = 4;

App::App() : KMainWindow(0) {
  highscore.resize(0);
  readHighscore();
  setCaption("");
  gameOver = false;

  // create reversi board
  b = new Board(this);
  createMenuBar();
  createToolBar();
  createStatusBar();

  b->setFixedSize(b->sizeHint());
  setCentralWidget(b);

  connect(b, SIGNAL(score()), this, SLOT(slotScore()));
  connect(b, SIGNAL(gameWon(int)), this, SLOT(slotGameEnded(int)));
  connect(b, SIGNAL(turn(int)), this, SLOT(slotTurn(int)));
  connect(b, SIGNAL(strengthChanged(int)), this, SLOT(slotStrength(int)));
  connect(b, SIGNAL(statusChange(int)), this, SLOT(slotStatusChange(int)));
  connect(b, SIGNAL(illegalMove()), this, SLOT(slotIllegalMove()));

  connect(tb, SIGNAL(moved(BarPosition)), this, SLOT(slotBarChanged()));

  KConfig *conf = kapp->config();
  b->setColor(b->paletteBackgroundColor());
  if(conf->readNumEntry("Background", -1) != -1) {
    int i = conf->readNumEntry("Background");
    if(i == 1) {
      QColor s = conf->readColorEntry("BackgroundColor");
      b->setColor(s);
    } else if(i == 2) {
      QString s = locate("appdata", conf->readEntry("BackgroundPixmap"));
      if(!s.isEmpty()) {
        QPixmap bg(s);
        if(bg.width())
          b->setPixmap(bg);
      }
    }
  }
  else
  {
    QPixmap bg(locate("appdata", "pics/background/Light_Wood.xpm"));
    if (bg.width())
      b->setPixmap(bg);
  }

  if(conf->readNumEntry("Skill", -1) != -1)
    b->setStrength(conf->readNumEntry("Skill", -1));

  if(conf->readNumEntry("Zoom", -1) != -1) {
    b->setZoom(conf->readNumEntry("Zoom", -1));
    b->setFixedSize(b->sizeHint());
  }

  // set toolbar position
  int tbpos = conf->readNumEntry("Toolbar_1_Pos",
			   (int)(KToolBar::Top));
  tb->setBarPos((KToolBar::BarPosition)tbpos);

  if(conf->readNumEntry("AnimationSpeed", -1000) != -1000)
    b->setAnimationSpeed(conf->readNumEntry("AnimationSpeed", 0));

  if(conf->readNumEntry("Grayscale", 0)) {
    b->loadChips("chips_mono.png");
    menu->setItemChecked(ID_OGSCALE, TRUE);
  }
  else {
    b->loadChips("chips.png");
  }

#ifdef HAVE_MEDIATOOL
  if(conf->readNumEntry("Sound", 0) != 0) {
    initAudio();
    if(!audioOK()) {
      show();
      kapp->processEvents();
      KMessageBox::error(this,
			   i18n("A problem with the sound server occured!\n"
				"Cannot enable sound support."));
    }
  } else {
    doneAudio(); // just to be sure
  }
#endif

  if(kapp->isRestored())
    restore(1);

  b->start();
}


App::~App() {
}

void App::createMenuBar() {
// TODO: all of this should be done by KActions using KStdAction and KStdGameAction
  menu = menuBar();

  QPopupMenu *fm = new QPopupMenu;
  fm->insertItem(SmallIcon("filenew"),i18n("&New Game"), ID_GNEW);
  fm->insertItem(SmallIcon("fileopen"),i18n("&Load Game"), ID_FLOAD);
  fm->insertItem(SmallIcon("filesave"),i18n("&Save Game"), ID_FSAVE);
  fm->insertSeparator();
  fm->insertItem(SmallIcon("exit"),i18n("&Quit"), ID_FQUIT);

  QPopupMenu *gm = new QPopupMenu;
  gm->insertItem(i18n("Get &Hint"), ID_HHINT);
  gm->insertSeparator();
  gm->insertItem(i18n("&Stop Thinking"), ID_GSTOP);
  gm->insertItem(i18n("&Continue"), ID_GCONTINUE);
  gm->insertSeparator();
  gm->insertItem(i18n("&Undo Move"), ID_GUNDO);
  gm->insertItem(i18n("Switch Si&des"), ID_GSWITCH);
  gm->insertSeparator();
  gm->insertItem(i18n("Hall of &Fame"), ID_GHIGHSCORES);

  QPopupMenu *om = new QPopupMenu;
  om->setCheckable(TRUE);
  QPopupMenu *om_s = new QPopupMenu;
  om_s->setCheckable(TRUE);
  om_s->insertItem(i18n("Level 1 (Wimp)"), ID_O2);
  om_s->insertItem(i18n("Level 2 (Beginner)"), ID_O3);
  om_s->insertItem(i18n("Level 3 (Novice)"),  ID_O4);
  om_s->insertItem(i18n("Level 4 (Average)"),  ID_O5);
  om_s->insertItem(i18n("Level 5 (Good)"),  ID_O6);
  om_s->insertItem(i18n("Level 6 (Expert)"),  ID_O7);
  om_s->insertItem(i18n("Level 7 (Master)"),  ID_O8);
  om->insertItem(i18n("Skill"), om_s);
  om->insertSeparator();
  om->insertItem(i18n("&Shrink Board"), ID_VZOOMOUT);
  om->insertItem(i18n("&Enlarge Board"), ID_VZOOMIN);

  QPopupMenu *zm = new QPopupMenu;
  zm->setCheckable(TRUE);
  zm->insertItem(i18n("&Half Size"), ID_VZOOM50);
  zm->insertItem(i18n("60%"), ID_VZOOM60);
  zm->insertItem(i18n("80%"), ID_VZOOM80);
  zm->insertItem(i18n("D&efault Size"), ID_VZOOM100);
  zm->insertItem(i18n("120%"), ID_VZOOM120);
  zm->insertItem(i18n("140%"), ID_VZOOM140);
  zm->insertItem(i18n("160%"), ID_VZOOM160);
  zm->insertItem(i18n("180%"), ID_VZOOM180);
  zm->insertItem(i18n("&Double Size"), ID_VZOOM200);
  om->insertItem(i18n("Set Size"), zm);

  om->insertSeparator();
  om->insertItem(i18n("Select &Background Color..."), ID_COLOR);

  QPopupMenu *om_bg = new QPopupMenu;
  lookupBackgroundPixmaps();

  if(backgroundPixmaps.count() == 0)
    om_bg->insertItem(i18n("none"), ID_PIXMAP);
  else {
    QPopupMenu *current = om_bg;
    for(unsigned i = 0; i < backgroundPixmaps.count(); i++) {
      // since the filename may contain underscore, they
      // are replaced with spaces in the menu entry
      QString s(backgroundPixmaps[i]);
      int x = s.findRev('/');
      if (x != -1)
         s = s.mid(x+1);
      x = s.findRev('.');
         s = s.left(x);
      s = s.replace(QRegExp("_"), " ");

      // avoid too longish menus
      if(current->count() > 20) {
	QPopupMenu *newmenu = new QPopupMenu;
	current->insertItem(i18n("More..."), newmenu);
	current = newmenu;
      }

      current->insertItem(s, ID_PIXMAP + i);
    }
  }

  om->insertItem(i18n("Select Background Image"), om_bg);
  om->insertItem(i18n("&Grayscale"), ID_OGSCALE);
  om->insertSeparator();
  om->insertItem(i18n("&Animations"), ID_OANIMATION);

  QPopupMenu *om_sp = new QPopupMenu;
  om_sp->setCheckable(TRUE);
  om_sp->insertItem(i18n("1 (fastest)"), ID_OSPEED+1);
  for(int i = ID_OSPEED+2; i < ID_OSPEED + 10; i++) {
    QString txt;
    txt.setNum(i - ID_OSPEED);
    om_sp->insertItem(txt, i);
  }
  om_sp->insertItem(i18n("10 (slowest)"), ID_OSPEED+10);
  om->insertItem(i18n("Animation Speed"), om_sp, ID_OSPEED);
#ifdef HAVE_MEDIATOOL
  om->insertSeparator();
  om->insertItem(i18n("S&ound"), ID_OSOUND);
#endif

  QString s;
  s = i18n("Version ");
  s += KREVERSI_VERSION;
  s += i18n("\n(c) 1997 Mario Weilguni <mweilguni@sime.com>\n\n" \
    "This program is free software\npublished under the GNU General\n" \
    "Public License (take a look\ninto help for details)\n\n" \
    "Thanks to:\n" \
    "\tMats Luthman for the game engine\n" \
    "\t(I've ported it from his JAVA applet)\n\n" \
    "\tStephan Kulow\n\tfor comments "\
    "and bugfixes\n\n" \
    "\tArne Klaassen\n\t "\
    "for the raytraced chips");
  QPopupMenu *help = helpMenu(s);

  menu->insertItem(i18n("&File"), fm);
  menu->insertItem(i18n("&Game"), gm);
  menu->insertItem(i18n("&Settings"), om);
  menu->insertSeparator();
  menu->insertItem(i18n("&Help"), help);
  connect(menu, SIGNAL(activated(int)), this, SLOT(processEvent(int)));

  menu->setAccel(CTRL+Key_S, ID_FSAVE);
  menu->setAccel(CTRL+Key_L, ID_FLOAD);
  menu->setAccel(CTRL+Key_Q, ID_FQUIT);
  menu->setAccel(Key_H, ID_HHINT);
  menu->setAccel(Key_F1, ID_HCONTENTS);
  menu->setAccel(Key_Escape, ID_GSTOP);
  menu->setAccel(CTRL+Key_N, ID_GNEW);
  menu->setAccel(CTRL+Key_U, ID_GUNDO);
  menu->setAccel(CTRL+Key_H, ID_GHIGHSCORES);
  menu->setAccel(CTRL+Key_1, ID_O2);
  menu->setAccel(CTRL+Key_2, ID_O3);
  menu->setAccel(CTRL+Key_3, ID_O4);
  menu->setAccel(CTRL+Key_4, ID_O5);
  menu->setAccel(CTRL+Key_5, ID_O6);
  menu->setAccel(CTRL+Key_6, ID_O7);
  menu->setAccel(CTRL+Key_7, ID_O8);

  menu->setAccel(CTRL + Key_Minus, ID_VZOOMOUT);
  menu->setAccel(CTRL + Key_Plus, ID_VZOOMIN);

  // if no background pixmaps were found, disable
  // the "none" entry
  menu->setItemEnabled(ID_PIXMAP, backgroundPixmaps.count()!=0);
}

#ifdef ICON
#undef ICON
#endif

#define ICON(x) QPixmap(PICDATA(x))

void App::createToolBar() {
  tb = toolBar();

  tb->insertButton(ICON("stop.xpm"),
		   ID_GSTOP, TRUE, i18n("Stop thinking"));
  tb->insertButton(ICON("undo.xpm"),
		   ID_GUNDO, TRUE, i18n("Undo move"));
  tb->insertButton(ICON("zoomout.xpm"),
		   ID_VZOOMOUT, TRUE, i18n("Shrink board"));
  tb->insertButton(ICON("zoomin.xpm"),
		   ID_VZOOMIN, TRUE, i18n("Enlarge board"));
  tb->insertButton(ICON("hint.xpm"),
		   ID_HHINT, TRUE, i18n("Get hint"));
  tb->insertButton(ICON("help.xpm"),
		   ID_HCONTENTS, TRUE, i18n("Get help"));
  connect(tb, SIGNAL(clicked(int)), this, SLOT(processEvent(int)));
}


void App::createStatusBar() {
  sb = new KStatusBar(this);
  sb->insertItem(i18n("XXXXX's turn"), SB_TURN,1);
  sb->insertItem(i18n("You (XXXXX): 88"), SB_SCOREH,2);
  sb->insertItem(i18n("Computer (XXXXX): 88"), SB_SCOREC,2);
}


void App::lookupBackgroundPixmaps() {
  KGlobal::dirs()->findAllResources("appdata", "pics/background/*.xpm", false, true, backgroundPixmaps);
}

void App::processEvent(int itemid) {
  QString s;

  switch(itemid) {

  case 0:
      break;        // Built in help
  case 3:
      break;        // About KDE

  case ID_HCONTENTS:
  {
      KApplication::kApplication()->invokeHelp();
      break;
  }
  case ID_FSAVE:
    {
      KConfig *config = kapp->config();
      config->setGroup("Savegame");
      b->saveGame(config);
      KMessageBox::information(this,
                               i18n("Game saved."));
    }
    break;

  case ID_FLOAD:
    {
      KConfig *config = kapp->config();
      config->setGroup("Savegame");

      if(b->canLoad(config))
	b->loadGame(config);
    }
    break;

  case ID_FQUIT:
    b->interrupt();

    // interesting, without this kreversi seems to segfault
    delete this;
    kapp->quit();
    return;
    break;

  case ID_GNEW:
    gameOver = false;
    b->newGame();
    break;

  case ID_GCONTINUE:
    b->doContinue();
    break;

  case ID_GSTOP:
    b->interrupt();
    break;

  case ID_GUNDO:
    b->undo();
    break;

  case ID_GSWITCH:
    b->switchSides();
    break;

  case ID_GHIGHSCORES:
    showHighscore();
    break;

  case ID_VZOOMIN:
    b->zoomIn();
    b->setFixedSize(b->sizeHint());
    kapp->config()->writeEntry("Zoom", b->getZoom());
    break;

  case ID_VZOOMOUT:
    b->zoomOut();
    b->setFixedSize(b->sizeHint());
    kapp->config()->writeEntry("Zoom", b->getZoom());
    break;

  case ID_VZOOM50:
  case ID_VZOOM60:
  case ID_VZOOM80:
  case ID_VZOOM100:
  case ID_VZOOM120:
  case ID_VZOOM140:
  case ID_VZOOM160:
  case ID_VZOOM180:
  case ID_VZOOM200:
    b->setZoom(itemid - ID_VZOOMBASE);
    b->setFixedSize(b->sizeHint());
    kapp->config()->writeEntry("Zoom", b->getZoom());
    break;

  case ID_OANIMATION:
    b->setAnimationSpeed(-b->animationSpeed());
    kapp->config()->writeEntry("AnimationSpeed", b->animationSpeed());
    break;

  case ID_OGSCALE:
    {
      bool gs;

      if(b->chipsName() == "chips.png") {
	b->loadChips("chips_mono.png");
	gs = TRUE;
      } else {
	b->loadChips("chips.png");
	gs = FALSE;
      }
      menu->setItemChecked(ID_OGSCALE, gs);
      kapp->config()->writeEntry("Grayscale", gs);
    }

  case ID_O2:
  case ID_O3:
  case ID_O4:
  case ID_O5:
  case ID_O6:
  case ID_O7:
  case ID_O8:
    b->setStrength(itemid - ID_OBASE);
    kapp->config()->writeEntry("Skill", itemid - ID_OBASE);
    break;

  case ID_COLOR:
    {
      QColor c = b->eraseColor();
      if(KColorDialog::getColor(c,this)) {
	b->setColor(c);
	kapp->config()->writeEntry("Background", 1);
	s = QString("%1 %2 %3").arg(c.red()).arg(c.green()).arg(c.blue());
	kapp->config()->writeEntry("BackgroundColor", s);
      }
    };
    break;

#ifdef HAVE_MEDIATOOL
  case ID_OSOUND:
    if(!audioOK()) {
      initAudio();
      if(!audioOK()) {
	KMessageBox::error(this,
			   i18n("A problem with the sound server occured!\n"
				"Cannot enable sound support."));
	kapp->config()->writeEntry("Sound", 0);
      } else
	kapp->config()->writeEntry("Sound", 1);
    } else {
      doneAudio();
      kapp->config()->writeEntry("Sound", 0);
    }
    break;
#endif

  case ID_HHINT:
    b->hint();
    break;

  default:
    {
      if((itemid >= ID_PIXMAP) && (itemid < ID_PIXMAP + (int)backgroundPixmaps.count())) {
	QPixmap pm(locate("appdata", backgroundPixmaps[itemid - ID_PIXMAP]));
	b->setPixmap(pm);
	kapp->config()->writeEntry("Background", 2);
	kapp->config()->writeEntry("BackgroundPixmap",
				    backgroundPixmaps[itemid - ID_PIXMAP]);
      } else if((itemid >= ID_OSPEED) && (itemid <= ID_OSPEED + 10)) {
	b->setAnimationSpeed(itemid - ID_OSPEED);
	kapp->config()->writeEntry("AnimationSpeed", b->animationSpeed());
      } else {
/*	KMessageBox::sorry(this,
                                i18n("Sorry, not yet implemented!"));*/
      }
    }
  }
  enableItems();
}


void App::slotScore() {
  int black, white;
  QString s1, s2;

  b->getScore(black, white);
  
  if(b->chipsName() == "chips.png") {
    if(b->humanIs() == Score::BLACK) {
      s1 = i18n("You (blue): %1").arg(black);
      s2 = i18n("Computer (red): %1").arg(white);
    } else {
      s2 = i18n("You (red): %1").arg(white);
      s1 = i18n("Computer (blue): %1").arg(black);
    }
  } else {
    if(b->humanIs() == Score::BLACK) {
      s1 = i18n("You (black): %1").arg(black);
      s2 = i18n("Computer (white): %1").arg(white);
    } else {
      s2 = i18n("You (white): %1").arg(white);
      s1 = i18n("Computer (black): %1").arg(black);
    }
  }

  sb->changeItem(s1, SB_SCOREH);
  sb->changeItem(s2, SB_SCOREC);
  enableItems();
}


void App::enableItems() {
  int i;

  menu->setItemEnabled(ID_VZOOMIN, b->canZoomIn());
  menu->setItemEnabled(ID_VZOOMOUT, b->canZoomOut());
  menu->setItemEnabled(ID_GSTOP, (bool)(b->getState() == Board::THINKING));
  tb->setItemEnabled(ID_VZOOMIN, b->canZoomIn());
  tb->setItemEnabled(ID_VZOOMOUT, b->canZoomOut());
  tb->setItemEnabled(ID_GSTOP, (bool)(b->getState() == Board::THINKING));
  menu->setItemEnabled(ID_GCONTINUE, b->interrupted());

  // check size entry
  for(i = ID_VZOOM50; i < ID_VZOOM200; i += 10)
    menu->setItemChecked(i, (b->getZoom() + ID_VZOOMBASE == i));

  // check animation speed
  menu->setItemChecked(ID_OANIMATION, (bool)(b->animationSpeed() > 0));
  for(i = ID_OSPEED+1; i <= ID_OSPEED + 10; i++)
    menu->setItemChecked(i, (bool)(b->animationSpeed() == i - ID_OSPEED));
  menu->setItemEnabled(ID_OSPEED, (bool)(b->animationSpeed() > 0));
  menu->setItemEnabled(ID_GUNDO, (bool)(b->getMoveNumber() != 0));
  menu->setItemEnabled(ID_GNEW, (bool)(b->getMoveNumber() != 0));
  tb->setItemEnabled(ID_GUNDO, (bool)(b->getMoveNumber() != 0));

  // check sound
#ifdef HAVE_MEDIATOOL
  menu->setItemChecked(ID_OSOUND, (bool)(audioOK()));
#endif
}


void App::slotGameEnded(int color) {
  QString s;
  int winner, loser;

  sb->changeItem(i18n("End of game"), SB_TURN);

  // get the score
  if(color == Score::BLACK)
    b->getScore(winner, loser);
  else
    b->getScore(loser, winner);

  if(color == Score::NOBODY) {
    playSound("reversi-drawn.wav");
    s = i18n("Game is drawn!\n\nYou     : %1\nComputer: %2").arg(winner).arg(loser);
    KMessageBox::information(this, s, i18n("Game Ended"));
  } else if(b->humanIs() == color) {
    // calculate score
    int  st = b->getStrength();
    int sum = winner + loser;
    float score= (float)winner / sum *
                 (st - (ID_O2 - ID_OBASE) + 1) / (ID_O8 - ID_O2 + 1) *
                 100.0;

    playSound("reversi-won.wav");
    s = i18n("Congratulations, you have won!\n\nYou     : %1\nComputer: %2\nYour rating %3%")
	      .arg(winner).arg(loser).arg(score,1);
    KMessageBox::information(this, s, i18n("Game Ended"));

    if (!gameOver)
    {
      // create highscore entry
      HighScore hs;
      QString name = getPlayerName();
      strncpy(hs.name, name.utf8(), sizeof(hs.name) - 1);
      hs.color = b->humanIs();
      hs.winner = winner;
      hs.loser = loser;
      hs.rating = score;
      hs.date = time((time_t*)0);

      int rank = insertHighscore(hs);
      if(rank != -1) {
        showHighscore(rank);
      }
    }
  } else {
    playSound("reversi-lost.wav");
    s = i18n("You have lost the game!\n\nYou     : %1\nComputer: %2")
	      .arg(loser).arg(winner);
    KMessageBox::information(this, s, i18n("Game Ended"));
  }
  gameOver = true;
}


void App::slotTurn(int color) {
  if (gameOver)
    return;

  QString s;

  if(b->humanIs() == Score::WHITE) {
    if(color == Score::WHITE)
      s = i18n("Your turn");
    else if(color == Score::BLACK)
      s = i18n("Computer's turn");
    else
      s = "";
  }
  else
  {
    if(color == Score::WHITE)
      s = i18n("Computer's turn");
    else if(color == Score::BLACK)
      s = i18n("Your turn");
    else
      s = "";
  }
  sb->changeItem(s, SB_TURN);
}


void App::slotStrength(int st) {
  for(int i = ID_O2; i <= ID_O8; i++)
    menu->setItemChecked(i, (bool)(i == ID_OBASE + st));
}


void App::slotStatusChange(int) {
  bool oldstate = (bool)(b->getState() == Board::THINKING);
  enableItems();
  bool newstate = (bool)(b->getState() == Board::THINKING);

  if(newstate != oldstate) {
    if(newstate) // we are thinking now
      kapp->setOverrideCursor(waitCursor);
    if(oldstate) // stopped thinking
      kapp->restoreOverrideCursor();
  }
}


void App::slotIllegalMove() {
  if(!audioOK())
    kapp->beep();
  else
    playSound("reversi-illegalmove.wav");
}


bool App::isBetter(HighScore &hs, HighScore &than) {
  if(hs.rating > than.rating)
    return TRUE;
  else if(hs.rating < than.rating)
    return FALSE;
  // better if less pieces are set
  else if(hs.winner + hs.loser < than.winner + than.loser)
    return TRUE;
  else
    return FALSE;
}


int App::insertHighscore(HighScore &hs) {
  int i;

  if(highscore.size() == 0) {
    highscore.resize(1);
    highscore[0] = hs;
    writeHighscore();
    return 0;
  } else {
    HighScore last = highscore[highscore.size() - 1];
    if(isBetter(hs, last) || ((int)highscore.size() < HIGHSCORE_MAX)) {
      if((int)highscore.size() == HIGHSCORE_MAX)
	highscore[HIGHSCORE_MAX - 1] = hs;
      else {
	highscore.resize(highscore.size()+1);
	highscore[highscore.size() - 1] = hs;
      }

      // sort in new entry
      int bestsofar = highscore.size() - 1;
      for(i = highscore.size() - 1; i > 0; i--)
	if(highscore[i-1].rating < highscore[i].rating) {
	  // swap entries
	  HighScore temp = highscore[i-1];
	  highscore[i-1] = highscore[i];
	  highscore[i] = temp;
	  bestsofar = i - 1;
	}

      writeHighscore();
      return bestsofar;
    }
  }
  return -1;
}


void App::readHighscore() {
  int i;
  QString s, e, grp;
  KConfig *conf = kapp->config();

  highscore.resize(0);
  i = 0;
  bool eol = FALSE;
  grp = conf->group();
  conf->setGroup("Hall of Fame");
  while ((i < HIGHSCORE_MAX) && !eol) {
    s = QString("Highscore_%1").arg(i);
    if(conf->hasKey(s)) {
      e = conf->readEntry(s);
      highscore.resize(i+1);

      HighScore hs;
      sscanf( e.utf8(), "%31s %d %d %d %f %ld",
	     hs.name, &hs.color, &hs.winner,
	     &hs.loser, &hs.rating, &hs.date);
      highscore[i] = hs;
    } else
      eol = TRUE;
    i++;
  }

  // restore old group
  conf->setGroup(grp);
}


void App::writeHighscore() {
  int i;
  QString s, e, grp;
  KConfig *conf = kapp->config();

  grp = conf->group();
  conf->setGroup("Hall of Fame");
  for(i = 0; i < (int)highscore.size(); i++) {
    s = QString("Highscore_%1").arg(i);
    HighScore hs = highscore[i];
    e = QString("%1 %2 %3 %4 %5 %6")
	      .arg(QString::fromUtf8(hs.name)).arg(hs.color).arg(hs.winner)
	      .arg(hs.loser).arg(hs.rating).arg(hs.date);
    conf->writeEntry(s, e);
  }

  // restore old group
  conf->setGroup(grp);
}

void App::showHighscore(int focusitem) {
  // this may look a little bit confusing...
  QDialog *dlg = new QDialog(this, "hall_of_fame", TRUE);
  dlg->setCaption(i18n("KReversi: Hall of Fame"));

  QVBoxLayout *tl = new QVBoxLayout(dlg, 10);

  QLabel *l = new QLabel(i18n("KReversi: Hall of Fame"), dlg);
  QFont f = font();
  f.setPointSize(24);
  f.setBold(TRUE);
  l->setFont(f);
  l->setFixedSize(l->sizeHint());
  l->setFixedWidth(l->width() + 32);
  l->setAlignment(AlignCenter);
  tl->addWidget(l);

  // insert highscores in a gridlayout
  QGridLayout *table = new QGridLayout(12, 5, 5);
  tl->addLayout(table, 1);

  // add a separator line
  KSeparator *sep = new KSeparator(dlg);
  table->addMultiCellWidget(sep, 1, 1, 0, 4);

  // add titles
  f = font();
  f.setBold(TRUE);
  l = new QLabel(i18n("Rank"), dlg);
  l->setFont(f);
  l->setMinimumSize(l->sizeHint());
  table->addWidget(l, 0, 0);
  l = new QLabel(i18n("Name"), dlg);
  l->setFont(f);
  l->setMinimumSize(l->sizeHint());
  table->addWidget(l, 0, 1);
  l = new QLabel(i18n("Color"), dlg);
  l->setFont(f);
  l->setMinimumSize(l->sizeHint());
  table->addWidget(l, 0, 2);
  l = new QLabel(i18n("Score"), dlg);
  l->setFont(f);
  l->setMinimumSize(l->sizeHint());
  table->addWidget(l, 0, 3);
  l = new QLabel(i18n("Rating"), dlg);
  l->setFont(f);
  l->setMinimumSize(l->sizeHint());
  table->addWidget(l, 0, 4);

  QString s;
  QLabel *e[10][5];
  unsigned i, j;

  for(i = 0; i < 10; i++) {
    QString color;
    HighScore hs;
    if(i < highscore.size()) {
      hs = highscore[i];
      if(hs.color == Score::BLACK)
	color = i18n("blue");
      else
	color = i18n("red");
    }

    // insert rank
    s.setNum(i+1);
    e[i][0] = new QLabel(s, dlg);

    // insert name
    if(i < highscore.size())
      e[i][1] = new QLabel(hs.name, dlg);
    else
      e[i][1] = new QLabel("", dlg);

    // insert color
    if(i < highscore.size())
      e[i][2] = new QLabel(color, dlg);
    else
      e[i][2] = new QLabel("", dlg);

    // insert score
    if(i < highscore.size())
      s = QString("%1/%2").arg(hs.winner).arg(hs.loser);
    else
      s = "";
    e[i][3] = new QLabel(s, dlg);

    // insert rating
    if(i < highscore.size())
      s.sprintf("%3.0f", hs.rating);
    else
      s = "";
    e[i][4] = new QLabel(s, dlg);
  }

  f = font();
  f.setBold(TRUE);
  f.setItalic(TRUE);
  for(i = 0; i < 10; i++)
    for(j = 0; j < 5; j++) {
      e[i][j]->setMinimumSize(e[i][j]->sizeHint());
      if((int)i == focusitem)
	e[i][j]->setFont(f);
      table->addWidget(e[i][j], i+2, j, AlignCenter);
    }

  QPushButton *b = new QPushButton(i18n("&Close"), dlg);
#if QT_VERSION < 300
  if(style().guiStyle() == MotifStyle)
    b->setFixedSize(b->sizeHint().width() + 10,
		    b->sizeHint().height() + 10);
  else
#endif
    b->setFixedSize(b->sizeHint());

  // connect the "Close"-button to done
  connect(b, SIGNAL(clicked()),
	  dlg, SLOT(accept()));
  b->setDefault(TRUE);
  b->setFocus();

  // make layout
  tl->addSpacing(10);
  tl->addWidget(b);
  tl->activate();
  tl->freeze();

  playSound("reversi-hof.wav");
  dlg->exec();
  delete dlg;
}


QString App::getPlayerName() {
  QDialog *dlg = new QDialog(this, "Hall of Fame", TRUE);

  QLabel  *l1  = new QLabel(i18n("You've made it into the \"Hall Of Fame\". Type in\nyour name so mankind will always remember\nyour cool rating."), dlg);
  l1->setFixedSize(l1->sizeHint());

  QLabel *l2 = new QLabel(i18n("Your name:"), dlg);
  l2->setFixedSize(l2->sizeHint());

  QLineEdit *e = new QLineEdit(dlg);
  e->setText("XXXXXXXXXXXXXXXX");
  e->setMinimumWidth(e->sizeHint().width());
  e->setFixedHeight(e->sizeHint().height());
  e->setText("");
  e->setFocus();

  QPushButton *b = new QPushButton(i18n("OK"), dlg);
  b->setDefault(TRUE);
#if QT_VERSION < 300
  if(style().guiStyle() == MotifStyle)
    b->setFixedSize(b->sizeHint().width() + 10,
		    b->sizeHint().height() +10);
  else
#endif
    b->setFixedSize(b->sizeHint());
  connect(b, SIGNAL(released()), dlg, SLOT(accept()));
  connect(e, SIGNAL(returnPressed()),
	  dlg, SLOT(accept()));

  // create layout
  QVBoxLayout *tl = new QVBoxLayout(dlg, 10);
  QHBoxLayout *tl1 = new QHBoxLayout();
  tl->addWidget(l1);
  tl->addSpacing(5);
  tl->addLayout(tl1);
  tl1->addWidget(l2);
  tl1->addWidget(e);
  tl->addSpacing(5);
  tl->addWidget(b);
  tl->activate();
  tl->freeze();

  dlg->exec();

  QString s = e->text();
  delete dlg;

  if(s.length() == 0)
    s = " ";
  return s;
}

void App::slotBarChanged() {
  kdDebug() << "void App::slotBarChanged()" << endl;

  KConfig *conf = kapp->config();
  if(conf) {
    conf->writeEntry("Toolbar_1_Pos", (int)(tb->barPos()));
  }
}

void App::saveProperties(KConfig *c) {
  // make sure options are written
  kapp->config()->sync();
  b->saveGame(c);
}

void App::readProperties(KConfig *c) {
  b->loadGame(c);
}

#include "app.moc"
