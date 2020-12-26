/*
    SPDX-FileCopyrightText: 2004 Nicolas HADACEK <hadacek@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "highscores.h"

#include <QDateTime>
#include <QVector>

#include <KLocalizedString>
#include <KConfigGroup>
#include <KConfig>
#include <KgDifficulty>

namespace KExtHighscore
{

ExtManager::ExtManager()
    : Manager(7)
{
    setShowMode(NeverShow);
    setShowStatistics(true);
    setShowDrawGamesStatistic(true);

    const uint       RANGE[6] = { 0, 32, 40, 48, 56, 64 };
    QVector<uint>  s;
    s.resize(6);
    std::copy(RANGE, RANGE+6, s.begin());
    setScoreHistogram(s, ScoreBound);

    QList< const KgDifficultyLevel * > diffList = Kg::difficulty()->levels();

    for (int i = 0; i < diffList.size(); i++)
        m_typeLabels << diffList.at(i)->title();
}


QString ExtManager::gameTypeLabel(uint gameType, LabelType type) const
{
    switch (type) {
    case Standard:
        return QString::number(gameType);
    case I18N:
        return m_typeLabels.at(gameType);
    case Icon:
        // FIXME dimsuz: implement
        break;
    case WW:
        break;
    }

    return QString();
}


// FIXME dimsuz: is this still needed?
/*
void ExtManager::convertLegacy(uint gameType)
{
  // Since there is no information about the skill level
  // in the legacy highscore list, consider they are
  // for beginner skill ...
  if ( gameType!=0 )
    return;

  KConfigGroup  cg(KSharedConfig::openConfig(), "High Score");

  for (uint i = 1; i <= 10; i++) {
    QString  key = "Pos" + QString::number(i);
    QString  name = cg.readEntry(key + "Name", QString());

    if ( name.isEmpty() )
      name = i18n("anonymous");

    uint  score = cg.readEntry(key + "NumChips", (uint)0);
    if ( score==0 )
      continue;

    QString    sdate = cg.readEntry(key + "Date", QString());
    QDateTime  date  = QDateTime::fromString(sdate);
    Score      s(Won);

    s.setScore(score);
    s.setData("name", name);
    if ( date.isValid() )
      s.setData("date", date);
    submitLegacyScore(s);
  }
}
 */


} // Namespace KExtHighscore
