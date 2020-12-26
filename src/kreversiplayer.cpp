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
