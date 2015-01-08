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

#include <QApplication>
#include <KLocalizedString>
#include <KAboutData>
#include <QCommandLineParser>

#include <highscores.h>
#include <mainwindow.h>
#include <QCommandLineOption>

static const char description[] = I18N_NOOP("KDE Reversi Board Game");

int main(int argc, char **argv)
{
    QApplication application(argc, argv);

    KAboutData aboutData(i18n("kreversi"), i18n("KReversi"),
                         "2.0", i18n(description), KAboutLicense::GPL,
                         i18n("(c) 1997-2000, Mario Weilguni\n(c) 2004-2006, Inge Wallin\n(c) 2006, Dmitry Suzdalev"),
                         QString(), i18n("http://games.kde.org/kreversi"));
    aboutData.addAuthor(i18n("Mario Weilguni"), i18n("Original author"), "mweilguni@sime.com");
    aboutData.addAuthor(i18n("Inge Wallin"), i18n("Original author"), "inge@lysator.liu.se");
    aboutData.addAuthor(i18n("Dmitry Suzdalev"), i18n("Game rewrite for KDE4. Current maintainer."), "dimsuz@gmail.com");
    aboutData.addCredit(i18n("Simon Hürlimann"), i18n("Action refactoring"));
    aboutData.addCredit(i18n("Mats Luthman"), i18n("Game engine, ported from his JAVA applet."));
    aboutData.addCredit(i18n("Arne Klaassen"), i18n("Original raytraced chips."));
    aboutData.addCredit(i18n("Mauricio Piacentini"), i18n("Vector chips and background for KDE4."));
    aboutData.addCredit(i18n("Brian Croom"), i18n("Port rendering code to KGameRenderer"), "brian.s.croom@gmail.com");
    aboutData.addCredit(i18n("Denis Kuplyakov"), i18n("Port rendering code to QML, redesign and a lot of improvements"), "dener.kup@gmail.com");

    KAboutData::setApplicationData(aboutData);

    QCommandLineParser parser;
    QApplication app(argc, argv);
    KAboutData::setApplicationData(aboutData);
    parser.addVersionOption();
    parser.addHelpOption();
    parser.addOption(QCommandLineOption(QStringList() << QLatin1String("demo"), i18n("Start with demo game playing")));

    aboutData.setupCommandLine(&parser);
    parser.process(app);
    aboutData.processCommandLine(&parser);

    if (application.isSessionRestored()) {
        RESTORE(KReversiMainWindow)
    } else {
        KReversiMainWindow *mainWin = new KReversiMainWindow(0, parser.isSet("demo"));
        mainWin->show();
    }

    KExtHighscore::ExtManager highscoresManager;

    return application.exec();
}
