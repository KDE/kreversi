/*******************************************************************
 *
 * Copyright 2006 Dmitry Suzdalev <dimsuz@gmail.com>
 *
 * This file is part of the KDE project "KReversi"
 *
 * KReversi is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * KReversi is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with KReversi; see the file COPYING.  If not, write to
 * the Free Software Foundation, 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 *
 *******************************************************************
 */


#include <kapplication.h>
#include <klocale.h>
#include <kcmdlineargs.h>
#include <kaboutdata.h>

#include "highscores.h"
#include "mainwindow.h"
#include "version.h"

static const char description[] = I18N_NOOP("KDE Reversi Board Game");

static const KCmdLineOptions options[] =
{
    {"demo", I18N_NOOP( "Start with demo game playing" ),  0 },
    KCmdLineLastOption
};

int main(int argc, char **argv)
{
    KAboutData aboutData( "kreversi", I18N_NOOP("KReversi"),
                          KREVERSI_VERSION, description, KAboutData::License_GPL,
                          "(c) 1997-2000, Mario Weilguni\n(c) 2004-2006, Inge Wallin\n(c) 2006, Dmitry Suzdalev");
    aboutData.addAuthor("Mario Weilguni",I18N_NOOP("Original author"), "mweilguni@sime.com");
    aboutData.addAuthor("Inge Wallin",I18N_NOOP("Original author"), "inge@lysator.liu.se");
    aboutData.addAuthor("Dmitry Suzdalev", I18N_NOOP("Game rewrite for KDE4. Current maintainer."), "dimsuz@gmail.com");
    aboutData.addCredit("Mats Luthman", I18N_NOOP("Game engine, ported from his JAVA applet."), 0);
    aboutData.addCredit("Arne Klaassen", I18N_NOOP("Original raytraced chips."), 0);
    aboutData.addCredit("Mauricio Piacentini", I18N_NOOP("Vector chips and background for KDE4."), 0);

    KCmdLineArgs::init( argc, argv, &aboutData );
    KCmdLineArgs::addCmdLineOptions( options );

    KApplication application;
    if( application.isSessionRestored() )
    {
        RESTORE(KReversiMainWindow)
    }
    else
    {
        KCmdLineArgs *args = KCmdLineArgs::parsedArgs();
        KReversiMainWindow *mainWin = new KReversiMainWindow( 0, args->isSet( "demo" ) );
        mainWin->show();
    }

    KExtHighscore::ExtManager highscoresManager;

    return application.exec();
}

