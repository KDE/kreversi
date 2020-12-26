/*
    SPDX-FileCopyrightText: 2004 Nicolas HADACEK (hadacek@kde.org)

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef HIGHSCORES_H
#define HIGHSCORES_H

#include <QStringList>
#include "kexthighscore.h"

namespace KExtHighscore
{

class ExtManager : public Manager
{
public:
    ExtManager();

private:
    QString gameTypeLabel(uint gameTye, LabelType) const override;
    QStringList m_typeLabels;
};

}

#endif
