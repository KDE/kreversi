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

#include <kapp.h>
#include <kstddirs.h>
#include <kglobal.h>
#include <kimgio.h>
#include <klocale.h>
#include <kcmdlineargs.h>
#include <kaboutdata.h>

#include "misc.h"
#include "version.h"
#include "app.h"


static const char *description = I18N_NOOP("KDE Board Game");

int main(int argc, char **argv)
{
  KAboutData aboutData( "kreversi", I18N_NOOP("KReversi"), 
    KREVERSI_VERSION, description, KAboutData::GPL, 
    "(c) 1997-2000, Mario Weilguni");
  aboutData.addAuthor("Mario Weilguni",0, "mweilguni@sime.com");
  KCmdLineArgs::init( argc, argv, &aboutData );

  KApplication a;

  kimgioRegister();
  
  App *app = new App;

  app->show();
  a.setMainWidget(app);
  a.setTopWidget(app);

  return a.exec();
}
