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

#include <kapplication.h>
#include <kstandarddirs.h>
#include <kglobal.h>
#include <kimageio.h>
#include <klocale.h>
#include <kcmdlineargs.h>
#include <kaboutdata.h>

#include "version.h"
#include "kreversi.h"

static const char *description = I18N_NOOP("KDE Board Game");

int main(int argc, char **argv)
{
  KAboutData aboutData( "kreversi", I18N_NOOP("KReversi"), 
    KREVERSI_VERSION, description, KAboutData::License_GPL, 
    "(c) 1997-2000, Mario Weilguni");
  aboutData.addAuthor("Mario Weilguni",0, "mweilguni@sime.com");
  aboutData.addAuthor("Benjamin Meyer",0, "ben@meyerhome.net");
  aboutData.addCredit("Mats Luthman", I18N_NOOP("Game engine, ported from his JAVA applet."), 0);
  aboutData.addCredit("Stephan Kulow", I18N_NOOP("Comments and bugfixes."), 0);
  aboutData.addCredit("Arne Klaassen", I18N_NOOP("Raytraced chips."), 0);

  KCmdLineArgs::init( argc, argv, &aboutData );

  KApplication a;
  KGlobal::locale()->insertCatalogue("libkdegames");

  // used for loading background pixmaps
  KImageIO::registerFormats();
  
  if(a.isRestored()){
     RESTORE(KReversi)
  }
  else {
    KReversi *kreversi = new KReversi();
    a.setMainWidget(kreversi);
    kreversi->show();
  }
  return a.exec();
}

