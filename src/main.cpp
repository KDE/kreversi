/*
    SPDX-FileCopyrightText: 2006 Dmitry Suzdalev <dimsuz@gmail.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include <QApplication>
#include <QCommandLineOption>
#include <QCommandLineParser>

#include <KAboutData>
#include <KLocalizedString>
#include <KCrash>
#include <KDBusService>
#include <Kdelibs4ConfigMigrator>

#include "highscores.h"
#include "mainwindow.h"
#include "kreversi_version.h"

int main(int argc, char **argv)
{
    // Fixes blurry icons with fractional scaling
    QGuiApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
    QApplication application(argc, argv);
    Kdelibs4ConfigMigrator migrate(QStringLiteral("kreversi"));
    migrate.setConfigFiles(QStringList() << QStringLiteral("kreversirc"));
    migrate.setUiFiles(QStringList() << QStringLiteral("kreversiui.rc"));
    migrate.migrate();

    KLocalizedString::setApplicationDomain("kreversi");
    KAboutData aboutData(QStringLiteral("kreversi"), i18n("KReversi"),
                         QStringLiteral(KREVERSI_VERSION_STRING),
                         i18n("KDE Reversi Board Game"),
                         KAboutLicense::GPL,
                         i18n("(c) 1997-2000, Mario Weilguni\n(c) 2004-2006, Inge Wallin\n(c) 2006, Dmitry Suzdalev"),
                         QString(), i18n("https://kde.org/applications/games/org.kde.kreversi"));
    aboutData.addAuthor(i18n("Mario Weilguni"), i18n("Original author"), QStringLiteral("mweilguni@sime.com"));
    aboutData.addAuthor(i18n("Inge Wallin"), i18n("Original author"), QStringLiteral("inge@lysator.liu.se"));
    aboutData.addAuthor(i18n("Dmitry Suzdalev"), i18n("Game rewrite for KDE4. Current maintainer."), QStringLiteral("dimsuz@gmail.com"));
    aboutData.addCredit(i18n("Simon Hürlimann"), i18n("Action refactoring"));
    aboutData.addCredit(i18n("Mats Luthman"), i18n("Game engine, ported from his JAVA applet."));
    aboutData.addCredit(i18n("Arne Klaassen"), i18n("Original raytraced chips."));
    aboutData.addCredit(i18n("Mauricio Piacentini"), i18n("Vector chips and background for KDE4."));
    aboutData.addCredit(i18n("Brian Croom"), i18n("Port rendering code to KGameRenderer"), QStringLiteral("brian.s.croom@gmail.com"));
    aboutData.addCredit(i18n("Denis Kuplyakov"), i18n("Port rendering code to QML, redesign and a lot of improvements"), QStringLiteral("dener.kup@gmail.com"));

    KAboutData::setApplicationData(aboutData);

    QCommandLineParser parser;
    KCrash::initialize();
    parser.addOption(QCommandLineOption(QStringList() << QStringLiteral("demo"), i18n("Start with demo game playing")));

    aboutData.setupCommandLine(&parser);
    parser.process(application);
    aboutData.processCommandLine(&parser);

    KDBusService service;
    if (application.isSessionRestored()) {
        kRestoreMainWindows<KReversiMainWindow>();
    } else {
        KReversiMainWindow *mainWin = new KReversiMainWindow(nullptr, parser.isSet(QStringLiteral("demo")));
        mainWin->show();
    }

    KExtHighscore::ExtManager highscoresManager;

    application.setWindowIcon(QIcon::fromTheme(QStringLiteral("kreversi")));

    return application.exec();
}
