/*******************************************************************
 *
 * Copyright 2013 Denis Kuplyakov <dener.kup@gmail.com>
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
 ********************************************************************/

#ifndef STARTGAMEDIALOG_H
#define STARTGAMEDIALOG_H

#include <gamestartinformation.h>
#include <commondefs.h>

#include <KDialog>
#include <KUser>
#include <KgThemeProvider>

namespace Ui
{
class StartGameDialog;
}

class StartGameDialog : public KDialog
{
    Q_OBJECT

public:
    explicit StartGameDialog(QWidget *parent, KgThemeProvider *provider);
    ~StartGameDialog();

    GameStartInformation createGameStartInformation() const;
    void setChipsPrefix(ChipsPrefix prefix);

signals:
    void startGame();

private slots:
    void slotUpdateBlack(int clickedId);
    void slotUpdateWhite(int clickedId);

private:
    void slotButtonClicked(int button);
    Ui::StartGameDialog *ui;
    QWidget *m_contents;
    KUser m_user;
    KgThemeProvider *m_provider;
    ChipsPrefix m_chipsPrefix;
    void loadChipImages();
};

#endif // STARTGAMEDIALOG_H
