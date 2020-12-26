/*
    SPDX-FileCopyrightText: 2002 Nicolas Hadacek <hadacek@kde.org>

    SPDX-License-Identifier: LGPL-2.0-only
*/

#ifndef KEXTHIGHSCORE_TAB_H
#define KEXTHIGHSCORE_TAB_H

#include <QComboBox>
#include <QVector>

class QLabel;
class QTreeWidget;


namespace KExtHighscore
{

//-----------------------------------------------------------------------------
class PlayersCombo : public QComboBox
{
 Q_OBJECT
 public:
    explicit PlayersCombo(QWidget *parent = nullptr);

    void load();

 Q_SIGNALS:
    void playerSelected(uint i);
    void allSelected();
    void noneSelected();

 private Q_SLOTS:
    void activatedSlot(int i);
};

//-----------------------------------------------------------------------------
class AdditionalTab : public QWidget
{
 Q_OBJECT
 public:
    explicit AdditionalTab(QWidget *parent);

    virtual void load();

 private Q_SLOTS:
    void playerSelected(uint i) { display(i) ; }
    void allSelected();

 protected:
    void init();
    static QString percent(uint n, uint total, bool withBraces = false);
    virtual void display(uint i) = 0;

 private:
    PlayersCombo *_combo;
};

//-----------------------------------------------------------------------------
class StatisticsTab : public AdditionalTab
{
 Q_OBJECT
 public:
    explicit StatisticsTab(QWidget *parent);

    void load() override;

 private:
    enum Count { Total = 0, Won, Lost, Draw, Nb_Counts };
    static const char *COUNT_LABELS[Nb_Counts];
    enum Trend { CurrentTrend = 0, WonTrend, LostTrend, Nb_Trends };
    static const char *TREND_LABELS[Nb_Trends];
    struct Data {
        uint count[Nb_Counts];
        double trend[Nb_Trends];
    };
    QVector<Data> _data;
    QLabel *_nbs[Nb_Counts], *_percents[Nb_Counts], *_trends[Nb_Trends];

    QString percent(const Data &, Count) const;
    void display(uint i) override;
};

//-----------------------------------------------------------------------------
class HistogramTab : public AdditionalTab
{
 Q_OBJECT
 public:
    explicit HistogramTab(QWidget *parent);

    void load() override;

 private:
    QVector<uint> _counts;
    QVector<uint> _data;
    QTreeWidget   *_list;

    void display(uint i) override;
};

} // namespace

#endif
