/*
    SPDX-FileCopyrightText: 2002 Nicolas Hadacek <hadacek@kde.org>

    SPDX-License-Identifier: LGPL-2.0-only
*/

#include "kexthighscore_tab.h"


#include <QApplication>
#include <QGroupBox>
#include <QHeaderView>
#include <QLayout>
#include <QLabel>
#include <QPixmap>
#include <QTreeWidget>


#include "kexthighscore.h"
#include "kexthighscore_internal.h"


namespace KExtHighscore
{

//-----------------------------------------------------------------------------
PlayersCombo::PlayersCombo(QWidget *parent)
    : QComboBox(parent)
{
    const PlayerInfos &p = internal->playerInfos();
    for (uint i = 0; i<p.nbEntries(); i++)
        addItem(p.prettyName(i));
    addItem(QStringLiteral("<") + i18n("all") + QLatin1Char( '>' ));
    connect(this, static_cast<void (PlayersCombo::*)(int)>(&PlayersCombo::activated), this, &PlayersCombo::activatedSlot);
}

void PlayersCombo::activatedSlot(int i)
{
    const PlayerInfos &p = internal->playerInfos();
    if ( i==(int)p.nbEntries() ) Q_EMIT allSelected();
    else if ( i==(int)p.nbEntries()+1 ) Q_EMIT noneSelected();
    else Q_EMIT playerSelected(i);
}

void PlayersCombo::load()
{
    const PlayerInfos &p = internal->playerInfos();
    for (uint i = 0; i<p.nbEntries(); i++)
        setItemText(i, p.prettyName(i));
}

//-----------------------------------------------------------------------------
AdditionalTab::AdditionalTab(QWidget *parent)
    : QWidget(parent)
{
    QVBoxLayout *top = new QVBoxLayout(this);
    //top->setMargin( QApplication::style()->pixelMetric(QStyle::PM_DefaultChildMargin) );
    //top->setSpacing( QApplication::style()->pixelMetric(QStyle::PM_DefaultLayoutSpacing) );

    QHBoxLayout *hbox = new QHBoxLayout;
    top->addLayout(hbox);
    QLabel *label = new QLabel(i18n("Select player:"), this);
    hbox->addWidget(label);
    _combo = new PlayersCombo(this);
    connect(_combo, &PlayersCombo::playerSelected, this, &AdditionalTab::playerSelected);
    connect(_combo, &PlayersCombo::allSelected, this, &AdditionalTab::allSelected);
    hbox->addWidget(_combo);
    hbox->addStretch(1);
}

void AdditionalTab::init()
{
    uint id = internal->playerInfos().id();
    _combo->setCurrentIndex(id);
    playerSelected(id);
}

void AdditionalTab::allSelected()
{
    display(internal->playerInfos().nbEntries());
}

QString AdditionalTab::percent(uint n, uint total, bool withBraces)
{
    if ( n==0 || total==0 ) return QString();
    QString s =  QStringLiteral( "%1%").arg(100.0 * n / total, 0, 'f', 1);
    return (withBraces ? QLatin1Char('(') + s + QLatin1Char( ')' ) : s);
}

void AdditionalTab::load()
{
    _combo->load();
}


//-----------------------------------------------------------------------------
const char *StatisticsTab::COUNT_LABELS[Nb_Counts] = {
    I18N_NOOP("Total:"), I18N_NOOP("Won:"), I18N_NOOP("Lost:"),
    I18N_NOOP("Draw:")
};
const char *StatisticsTab::TREND_LABELS[Nb_Trends] = {
    I18N_NOOP("Current:"), I18N_NOOP("Max won:"), I18N_NOOP("Max lost:")
};

StatisticsTab::StatisticsTab(QWidget *parent)
    : AdditionalTab(parent)
{
    setObjectName( QStringLiteral("statistics_tab" ));
    // construct GUI
    QVBoxLayout *top = static_cast<QVBoxLayout *>(layout());

    QHBoxLayout *hbox = new QHBoxLayout;
    QVBoxLayout *vbox = new QVBoxLayout;
    hbox->addLayout(vbox);
    top->addLayout(hbox);

    QGroupBox *group = new QGroupBox(i18n("Game Counts"), this);
    vbox->addWidget(group);
    QGridLayout *gridLay = new QGridLayout(group);
    //gridLay->setSpacing(QApplication::style()->pixelMetric(QStyle::PM_DefaultLayoutSpacing));
    for (uint k=0; k<Nb_Counts; k++) {
        if ( Count(k)==Draw && !internal->showDrawGames ) continue;
        gridLay->addWidget(new QLabel(i18n(COUNT_LABELS[k]), group), k, 0);
        _nbs[k] = new QLabel(group);
        gridLay->addWidget(_nbs[k], k, 1);
        _percents[k] = new QLabel(group);
        gridLay->addWidget(_percents[k], k, 2);
    }

    group = new QGroupBox(i18n("Trends"), this);
    vbox->addWidget(group);
    gridLay = new QGridLayout(group);
    //gridLay->setSpacing(QApplication::style()->pixelMetric(QStyle::PM_DefaultLayoutSpacing));
    for (uint k=0; k<Nb_Trends; k++) {
        gridLay->addWidget(new QLabel(i18n(TREND_LABELS[k]), group), k, 0);
        _trends[k] = new QLabel(group);
        gridLay->addWidget(_trends[k], k, 1);
    }

    hbox->addStretch(1);
    top->addStretch(1);
}

void StatisticsTab::load()
{
    AdditionalTab::load();
    const PlayerInfos &pi = internal->playerInfos();
    uint nb = pi.nbEntries();
    _data.resize(nb+1);
    for (int i=0; i<_data.size()-1; i++) {
        _data[i].count[Total] = pi.item(QStringLiteral( "nb games" ))->read(i).toUInt();
        _data[i].count[Lost] = pi.item(QStringLiteral( "nb lost games" ))->read(i).toUInt()
                       + pi.item(QStringLiteral( "nb black marks" ))->read(i).toUInt(); // legacy
        _data[i].count[Draw] = pi.item(QStringLiteral( "nb draw games" ))->read(i).toUInt();
        _data[i].count[Won] = _data[i].count[Total] - _data[i].count[Lost]
                              - _data[i].count[Draw];
        _data[i].trend[CurrentTrend] =
            pi.item(QStringLiteral( "current trend" ))->read(i).toInt();
        _data[i].trend[WonTrend] = pi.item(QStringLiteral( "max won trend" ))->read(i).toUInt();
        _data[i].trend[LostTrend] =
            -(int)pi.item(QStringLiteral( "max lost trend" ))->read(i).toUInt();
    }

    for (int k=0; k<Nb_Counts; k++) _data[nb].count[k] = 0;
    for (int k=0; k<Nb_Trends; k++) _data[nb].trend[k] = 0;
    for (int i=0; i<_data.size()-1; i++) {
        for (uint k=0; k<Nb_Counts; k++)
            _data[nb].count[k] += _data[i].count[k];
        for (uint k=0; k<Nb_Trends; k++)
            _data[nb].trend[k] += _data[i].trend[k];
    }
    for (uint k=0; k<Nb_Trends; k++)
        _data[nb].trend[k] /= (_data.size()-1);

    init();
}

QString StatisticsTab::percent(const Data &d, Count count) const
{
    if ( count==Total ) return QString();
    return AdditionalTab::percent(d.count[count], d.count[Total], true);
}

void StatisticsTab::display(uint i)
{
    const Data &d = _data[i];
    for (uint k=0; k<Nb_Counts; k++) {
        if ( Count(k) && !internal->showDrawGames ) continue;
        _nbs[k]->setText(QString::number(d.count[k]));
        _percents[k]->setText(percent(d, Count(k)));
    }
    for (uint k=0; k<Nb_Trends; k++) {
        QString s;
        if ( d.trend[k]>0 ) s = QLatin1Char( '+' );
        int prec = (i==internal->playerInfos().nbEntries() ? 1 : 0);
        _trends[k]->setText(s + QString::number(d.trend[k], 'f', prec));
    }
}

//-----------------------------------------------------------------------------
HistogramTab::HistogramTab(QWidget *parent)
    : AdditionalTab(parent)
{
    setObjectName( QStringLiteral("histogram_tab" ));
    // construct GUI
    QVBoxLayout *top = static_cast<QVBoxLayout *>(layout());

    _list = new QTreeWidget(this);
    _list->setSelectionMode(QAbstractItemView::NoSelection);
/// @todo to port or no more necessary ?
//     _list->setItemMargin(3);
    _list->setAllColumnsShowFocus(true);
    _list->setSortingEnabled(false);
    _list->header()->setSectionsClickable(false);
    _list->header()->setSectionsMovable(false);
    top->addWidget(_list);

    _list->headerItem()->setText(0,i18n("From"));
    _list->headerItem()->setText(1,i18n("To"));
    _list->headerItem()->setText(2,i18n("Count"));
    _list->headerItem()->setText(3,i18n("Percent"));
    for (int i=0; i<4; i++) _list->headerItem()->setTextAlignment(i, Qt::AlignRight);
    _list->headerItem()->setText(4,QString());

    const Item *sitem = internal->scoreInfos().item(QStringLiteral( "score" ))->item();
    const PlayerInfos &pi = internal->playerInfos();
    const QVector<uint> &sh = pi.histogram();
    for (int k=1; k<( int )pi.histoSize(); k++) {
        QString s1 = sitem->pretty(0, sh[k-1]);
        QString s2;
        if ( k==sh.size() ) s2 = QStringLiteral( "..." );
        else if ( sh[k]!=sh[k-1]+1 ) s2 = sitem->pretty(0, sh[k]);
	QStringList items; items << s1 << s2;
        (void)new QTreeWidgetItem(_list, items);
    }
}

void HistogramTab::load()
{
    AdditionalTab::load();
    const PlayerInfos &pi = internal->playerInfos();
    uint n = pi.nbEntries();
    uint s = pi.histoSize() - 1;
    _counts.resize((n+1) * s);
    _data.fill(0, n+1);
    for (uint k=0; k<s; k++) {
        _counts[n*s + k] = 0;
        for (uint i=0; i<n; i++) {
            uint nb = pi.item(pi.histoName(k+1))->read(i).toUInt();
            _counts[i*s + k] = nb;
            _counts[n*s + k] += nb;
            _data[i] += nb;
            _data[n] += nb;
        }
    }

    init();
}

void HistogramTab::display(uint i)
{
    const PlayerInfos &pi = internal->playerInfos();
    uint itemNum = 0;
    QTreeWidgetItem *item = _list->topLevelItem(itemNum);
    uint s = pi.histoSize() - 1;
    for (int k=s-1; k>=0; k--) {
        uint nb = _counts[i*s + k];
        item->setText(2, QString::number(nb));
        item->setText(3, percent(nb, _data[i]));
        uint width = (_data[i]==0 ? 0 : qRound(150.0 * nb / _data[i]));
        QPixmap pixmap(width, 10);
        pixmap.fill(Qt::blue);
        item->setData(4, Qt::DecorationRole, pixmap);
	itemNum++;
        item = _list->topLevelItem(itemNum);
    }
}

} // namespace
