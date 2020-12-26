/*
    SPDX-FileCopyrightText: 2001-2003 Nicolas Hadacek <hadacek@kde.org>

    SPDX-License-Identifier: LGPL-2.0-only
*/

#include "kexthighscore_item.h"

#include <QApplication>
#include <QHBoxLayout>
#include <QLabel>
#include <QLocale>
#include <QWidget>

#include <KHighscore>
#include <KPageDialog>
#include "kexthighscore_internal.h"
#include "kexthighscore_gui.h"


namespace KExtHighscore
{

//-----------------------------------------------------------------------------
Item::Item(const QVariant &def, const QString &label, Qt::AlignmentFlag alignment)
    : _default(def), _label(label), _alignment(alignment),
      _format(NoFormat), _special(NoSpecial)
{}

Item::~Item()
{}

QVariant Item::read(uint, const QVariant &value) const
{
    return value;
}

void Item::setPrettyFormat(Format format)
{
    bool buint = ( _default.type()==QVariant::UInt );
    bool bdouble = ( _default.type()==QVariant::Double );
    bool bnum = ( buint || bdouble || _default.type()==QVariant::Int );

    switch (format) {
    case OneDecimal:
    case Percentage:
        Q_ASSERT(bdouble);
        break;
    case MinuteTime:
        Q_ASSERT(bnum);
        break;
    case DateTime:
    	Q_ASSERT( _default.type()==QVariant::DateTime );
	break;
    case NoFormat:
        break;
    }

    _format = format;
}

void Item::setPrettySpecial(Special special)
{
    bool buint = ( _default.type()==QVariant::UInt );
    bool bnum = ( buint || _default.type()==QVariant::Double
                  || _default.type()==QVariant::Int );

    switch (special) {
    case ZeroNotDefined:
        Q_ASSERT(bnum);
        break;
    case NegativeNotDefined:
        Q_ASSERT(bnum && !buint);
        break;
    case DefaultNotDefined:
        break;
    case Anonymous:
        Q_ASSERT( _default.type()==QVariant::String );
        break;
    case NoSpecial:
        break;
    }

     _special = special;
}

QString Item::timeFormat(uint n)
{
    Q_ASSERT( n<=3600 && n!=0 );
    n = 3600 - n;
    return QString::number(n / 60).rightJustified(2, QLatin1Char( '0' )) + QLatin1Char( ':' )
        + QString::number(n % 60).rightJustified(2, QLatin1Char( '0' ));
}

QString Item::pretty(uint, const QVariant &value) const
{
    switch (_special) {
    case ZeroNotDefined:
        if ( value.toUInt()==0 ) return QStringLiteral( "--" );
        break;
    case NegativeNotDefined:
        if ( value.toInt()<0 ) return QStringLiteral( "--" );
        break;
    case DefaultNotDefined:
        if ( value==_default ) return QStringLiteral( "--" );
        break;
    case Anonymous:
        if ( value.toString()==QLatin1String( ItemContainer::ANONYMOUS ) )
            return i18n(ItemContainer::ANONYMOUS_LABEL);
        break;
    case NoSpecial:
        break;
    }

    switch (_format) {
    case OneDecimal:
        return QString::number(value.toDouble(), 'f', 1);
    case Percentage:
        return QString::number(value.toDouble(), 'f', 1) + QLatin1Char( '%' );
    case MinuteTime:
        return timeFormat(value.toUInt());
    case DateTime:
        if ( value.toDateTime().isNull() ) return QStringLiteral( "--" );
        return QLocale().toString(value.toDateTime());
    case NoFormat:
        break;
    }

    return value.toString();
}

//-----------------------------------------------------------------------------
Score::Score(ScoreType type)
    : _type(type)
{
    const ItemArray &items = internal->scoreInfos();
    for (int i=0; i<items.size(); i++)
        _data[items[i]->name()] = items[i]->item()->defaultValue();
}

Score::~Score()
{}

QVariant Score::data(const QString &name) const
{
    Q_ASSERT( _data.contains(name) );
    return _data[name];
}

void Score::setData(const QString &name, const QVariant &value)
{
    Q_ASSERT( _data.contains(name) );
    Q_ASSERT( _data[name].type()==value.type() );
    _data[name] = value;
}

bool Score::isTheWorst() const
{
    Score s;
    return score()==s.score();
}

bool Score::operator <(const Score &score) const
{
    return internal->manager.isStrictlyLess(*this, score);
}

QDataStream &operator <<(QDataStream &s, const Score &score)
{
    s << (quint8)score.type();
    s << score._data;
    return s;
}

QDataStream &operator >>(QDataStream &s, Score &score)
{
    quint8 type;
    s >> type;
    score._type = (ScoreType)type;
    s >> score._data;
    return s;
}

//-----------------------------------------------------------------------------
MultiplayerScores::MultiplayerScores()
{}

MultiplayerScores::~MultiplayerScores()
{}

void MultiplayerScores::clear()
{
    Score score;
    for (int i=0; i<_scores.size(); i++) {
        _nbGames[i] = 0;
        QVariant name = _scores[i].data(QStringLiteral( "name" ));
        _scores[i] = score;
        _scores[i].setData(QStringLiteral( "name" ), name);
        _scores[i]._data[QStringLiteral( "mean score" )] = double(0);
        _scores[i]._data[QStringLiteral( "nb won games" )] = uint(0);
    }
}

void MultiplayerScores::setPlayerCount(uint nb)
{
    _nbGames.resize(nb);
    _scores.resize(nb);
    clear();
}

void MultiplayerScores::setName(uint i, const QString &name)
{
    _scores[i].setData(QStringLiteral( "name" ), name);
}

void MultiplayerScores::addScore(uint i, const Score &score)
{
    QVariant name = _scores[i].data(QStringLiteral( "name" ));
    double mean = _scores[i].data(QStringLiteral( "mean score" )).toDouble();
    uint won = _scores[i].data(QStringLiteral( "nb won games" )).toUInt();
    _scores[i] = score;
    _scores[i].setData(QStringLiteral( "name" ), name);
    _nbGames[i]++;
    mean += (double(score.score()) - mean) / _nbGames[i];
    _scores[i]._data[QStringLiteral( "mean score" )] = mean;
    if ( score.type()==Won ) won++;
    _scores[i]._data[QStringLiteral( "nb won games" )] = won;
}

void MultiplayerScores::show(QWidget *parent)
{
    QLoggingCategory::setFilterRules(QStringLiteral("games.highscore.debug = true"));
    
    // check consistency
    if ( _nbGames.size()<2 ) 
	qCWarning(GAMES_EXTHIGHSCORE) << "less than 2 players";
    
    else {
        bool ok = true;
        uint nb = _nbGames[0];
        for (int i=1; i<_nbGames.size(); i++)
            if ( _nbGames[i]!=nb ) ok = false;
        if (!ok)
           qCWarning(GAMES_EXTHIGHSCORE) << "players have not same number of games";
    }

    // order the players according to the number of won games
    QVector<Score> ordered;
    for (int i=0; i<_scores.size(); i++) {
        uint won = _scores[i].data(QStringLiteral( "nb won games" )).toUInt();
        double mean = _scores[i].data(QStringLiteral( "mean score" )).toDouble();
        QVector<Score>::iterator it;
        for(it = ordered.begin(); it!=ordered.end(); ++it) {
            uint cwon = (*it).data(QStringLiteral( "nb won games" )).toUInt();
            double cmean = (*it).data(QStringLiteral( "mean score" )).toDouble();
            if ( won<cwon || (won==cwon && mean<cmean) ) {
                ordered.insert(it, _scores[i]);
                break;
            }
        }
        if ( it==ordered.end() ) ordered.push_back(_scores[i]);
    }

    // show the scores
    KPageDialog dialog(parent);
    dialog.setWindowTitle(i18n("Multiplayers Scores"));
// TODO    dialog.setButtons(KDialog::Close);
    dialog.setModal(true);
    dialog.setFaceType(KPageDialog::Plain);
    KPageWidgetItem *page = new KPageWidgetItem( new QLabel(QLatin1String( "" )), QLatin1String( "" ) );
    QHBoxLayout *hbox = new QHBoxLayout(page->widget());
    //hbox->setMargin(QApplication::style()->pixelMetric(QStyle::PM_DefaultLayoutSpacing));
    //hbox->setSpacing(QApplication::style()->pixelMetric(QStyle::PM_DefaultLayoutSpacing));

    QWidget *vbox = new QWidget(page->widget());
    hbox->addWidget(vbox);
    if ( _nbGames[0]==0 ) (void)new QLabel(i18n("No game played."), vbox);
    else {
        (void)new QLabel(i18n("Scores for last game:"), vbox);
        (void)new LastMultipleScoresList(ordered, vbox);
    }

    if ( _nbGames[0]>1 ) {
        vbox = new QWidget(page->widget());
        hbox->addWidget(vbox);
        (void)new QLabel(i18n("Scores for the last %1 games:",
                          _nbGames[0]), vbox);
        (void)new TotalMultipleScoresList(ordered, vbox);
    }

    //dialog.showButtonSeparator(false);
    dialog.addPage(page);
    dialog.exec();
}

QDataStream &operator <<(QDataStream &s, const MultiplayerScores &score)
{
    s << score._scores;
    s << score._nbGames;
    return s;
}

QDataStream &operator >>(QDataStream &s, MultiplayerScores &score)
{


    s >> score._scores;
    s >> score._nbGames;
    return s;
}

} // namespace
