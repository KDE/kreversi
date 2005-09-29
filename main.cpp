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
 * the Free Software Foundation, 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 *
 *******************************************************************
 */


#include <kapplication.h>
#include <kimageio.h>
#include <kcmdlineargs.h>
#include <kaboutdata.h>
#include <khighscore.h>

#include "version.h"
#include "kreversi.h"
#include "highscores.h"


static const char description[] = I18N_NOOP("KDE Board Game");

int main(int argc, char **argv)
{
  KHighscore::init("kreversi");

  KAboutData aboutData( "kreversi", I18N_NOOP("KReversi"),
    KREVERSI_VERSION, description, KAboutData::License_GPL,
    "(c) 1997-2000, Mario Weilguni");
  aboutData.addAuthor("Mario Weilguni",0, "mweilguni@sime.com");
  aboutData.addAuthor("Benjamin Meyer",0, "ben+kreversi@meyerhome.net");
  aboutData.addCredit("Mats Luthman", I18N_NOOP("Game engine, ported from his JAVA applet."), 0);
  aboutData.addCredit("Stephan Kulow", I18N_NOOP("Comments and bugfixes."), 0);
  aboutData.addCredit("Arne Klaassen", I18N_NOOP("Raytraced chips."), 0);
  aboutData.addCredit("Inge Wallin", I18N_NOOP("Cleaning, bugfixes, some enhancements."), 0);

  KCmdLineArgs::init( argc, argv, &aboutData );

  KApplication a;
  KGlobal::locale()->insertCatalogue("libkdegames");

  // used for loading background pixmaps
  KImageIO::registerFormats();
  KExtHighscore::ExtManager highscores;

  if (a.isRestored()){
     RESTORE(KReversi)
  }
  else {
    KReversi *kreversi = new KReversi;
    a.setMainWidget(kreversi);
    kreversi->show();
  }

  return a.exec();
}

