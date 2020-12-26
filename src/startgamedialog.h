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

#include "gamestartinformation.h"
#include "commondefs.h"

#include <QDialog>
#include <KUser>
#include <KgThemeProvider>
class QAbstractButton;
namespace Ui
{
class StartGameDialog;
}

/**
 * Dialog used to gather information about new game.
 * @see GameStartInformation
 */
class StartGameDialog : public QDialog
{
    Q_OBJECT

public:
    explicit StartGameDialog(QWidget *parent, KgThemeProvider *provider);
    ~StartGameDialog();

    /**
     * @return collected GameStartInformation
     */
    GameStartInformation createGameStartInformation() const;
    /**
     * Sets chips color according to @p prefix
     */
    void setChipsPrefix(ChipsPrefix prefix);

Q_SIGNALS:
    /**
     * Emitted when user has finished entering information
     */
    void startGame();

private Q_SLOTS:
    /**
     * Handles "User changed player type for black player" event
     */
    void slotUpdateBlack(QAbstractButton *button);

    /**
     * Handles "User changed player type for white player" event
     */
    void slotUpdateWhite(QAbstractButton *button);

    void slotAccepted();

private:
    /**
     * Updates chip images
     */
    void loadChipImages();
    /**
     * Encapsulates UI
     */
    Ui::StartGameDialog *ui;
    /**
     * Main dialog widget
     */
    QWidget *m_contents;
    /**
     * Needed to get username
     */
    KUser m_user;
    /**
     * Used to draw chip
     */
    KgThemeProvider *m_provider;
    /**
     * Sets chip's color to use
     */
    ChipsPrefix m_chipsPrefix;
};

#endif // STARTGAMEDIALOG_H
