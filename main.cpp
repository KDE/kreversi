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

static const char description[] = I18N_NOOP("KDE Reversi Board Game");

int main(int argc, char **argv)
{
    KAboutData aboutData( "kreversi", 0, ki18n("KReversi"),
                          "2.0", ki18n(description), KAboutData::License_GPL,
                          ki18n("(c) 1997-2000, Mario Weilguni\n(c) 2004-2006, Inge Wallin\n(c) 2006, Dmitry Suzdalev"),
                          KLocalizedString(), "http://games.kde.org/kreversi" );
    aboutData.addAuthor(ki18n("Mario Weilguni"),ki18n("Original author"), "mweilguni@sime.com");
    aboutData.addAuthor(ki18n("Inge Wallin"),ki18n("Original author"), "inge@lysator.liu.se");
    aboutData.addAuthor(ki18n("Dmitry Suzdalev"), ki18n("Game rewrite for KDE4. Current maintainer."), "dimsuz@gmail.com");
    aboutData.addCredit(ki18n("Simon Hürlimann"), ki18n("Action refactoring"));
    aboutData.addCredit(ki18n("Mats Luthman"), ki18n("Game engine, ported from his JAVA applet."));
    aboutData.addCredit(ki18n("Arne Klaassen"), ki18n("Original raytraced chips."));
    aboutData.addCredit(ki18n("Mauricio Piacentini"), ki18n("Vector chips and background for KDE4."));
    aboutData.addCredit(ki18n("Brian Croom"), ki18n("Port rendering code to KGameRenderer"), "brian.s.croom@gmail.com");

    KCmdLineArgs::init( argc, argv, &aboutData );

    KCmdLineOptions options;
    options.add("demo", ki18n( "Start with demo game playing" ));
    KCmdLineArgs::addCmdLineOptions( options );

    KApplication application;
    KGlobal::locale()->insertCatalog( QLatin1String( "libkdegames" ));

    if( application.isSessionRestored() )
    {
        RESTORE(KReversiMainWindow)
    }
    else
    {
        KCmdLineArgs *args = KCmdLineArgs::parsedArgs();
        KReversiMainWindow *mainWin = new KReversiMainWindow( 0, args->isSet( "demo" ) );
	args->clear();
        mainWin->show();
    }

    KExtHighscore::ExtManager highscoresManager;

    return application.exec();
}

