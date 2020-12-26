/*
    SPDX-FileCopyrightText: 2013 Denis Kuplyakov <dener.kup@gmail.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "kreversiplayer.h"

KReversiPlayer::KReversiPlayer(ChipColor color, const QString &name,
                               bool hintAllowed, bool undoAllowed):
    m_state(UNKNOWN), m_color(color), m_name(name),
    m_hintAllowed(hintAllowed), m_hintCount(0), m_undoAllowed(undoAllowed),
    m_undoCount(0)
{
}

ChipColor KReversiPlayer::getColor() const
{
    return m_color;
}

QString KReversiPlayer::getName() const
{
    return m_name;
}

bool KReversiPlayer::isHintAllowed() const
{
    return m_hintAllowed;
}

void KReversiPlayer::hintUsed()
{
    m_hintCount++;
}

int KReversiPlayer::getHintsCount()
{
    return m_hintCount;
}

bool KReversiPlayer::isUndoAllowed() const
{
    return m_undoAllowed;
}

void KReversiPlayer::undoUsed()
{
    m_undoCount++;
}

int KReversiPlayer::getUndoCount()
{
    return m_undoCount;
}
