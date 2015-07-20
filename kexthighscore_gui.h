/*
    This file is part of the KDE games library
    Copyright (C) 2001-02 Nicolas Hadacek (hadacek@kde.org)

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License version 2 as published by the Free Software Foundation.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/

#ifndef KEXTHIGHSCORE_GUI_H
#define KEXTHIGHSCORE_GUI_H

#include <QCheckBox>
#include <QLineEdit>
#include <QTreeWidget>
#include <QDialogButtonBox>
#include <QDialog>
#include <kpagedialog.h>

#include <QtCore/QList>

#include "kexthighscore.h"

class QTabWidget;
class KLineEdit;
class KUrlLabel;

namespace KExtHighscore
{

class ItemContainer;
class ItemArray;
class Score;
class AdditionalTab;

//-----------------------------------------------------------------------------
class ShowItem : public QTreeWidgetItem
{
 public:
    ShowItem(QTreeWidget *, bool highlight);

 private:
    bool _highlight;
};

class ScoresList : public QTreeWidget
{
 Q_OBJECT
 public:
    ScoresList(QWidget *parent);

    void addHeader(const ItemArray &);

 protected:
    QTreeWidgetItem *addLine(const ItemArray &, uint index, bool highlight);
    virtual QString itemText(const ItemContainer &, uint row) const = 0;

 private:
    virtual void addLineItem(const ItemArray &, uint index,
                             QTreeWidgetItem *item);
};

//-----------------------------------------------------------------------------
class HighscoresList : public ScoresList
{
 Q_OBJECT
 public:
    explicit HighscoresList(QWidget *parent);

    void load(const ItemArray &, int highlight);

 protected:
    QString itemText(const ItemContainer &, uint row) const;
};

class HighscoresWidget : public QWidget
{
 Q_OBJECT
 public:
    explicit HighscoresWidget(QWidget *parent);

    void load(int rank);

 signals:
    void tabChanged(int i);

 public slots:
    void changeTab(int i);

 private slots:
    void showURL(const QString &);
    void tabChanged() { emit tabChanged(_tw->currentIndex()); }

 private:
    QTabWidget     *_tw;
    HighscoresList *_scoresList, *_playersList;
    KUrlLabel      *_scoresUrl, *_playersUrl;
    AdditionalTab  *_statsTab, *_histoTab;
};

class HighscoresDialog : public KPageDialog
{
 Q_OBJECT
 public:
    HighscoresDialog(int rank, QWidget *parent);

 private slots:
    void slotUser1();
    void slotUser2();
    void tabChanged(int i) { _tab = i; }
    void highscorePageChanged(KPageWidgetItem *newpage, KPageWidgetItem *before);

 private:
    int _rank, _tab;
    QWidget *_current;
    QList<KPageWidgetItem*> _pages;
};

//-----------------------------------------------------------------------------
class LastMultipleScoresList : public ScoresList
{
    Q_OBJECT
public:
    LastMultipleScoresList(const QVector<Score> &, QWidget *parent);

private:
    void addLineItem(const ItemArray &, uint index, QTreeWidgetItem *line);
    QString itemText(const ItemContainer &, uint row) const;

private:
    const QVector<Score> &_scores;
};

class TotalMultipleScoresList : public ScoresList
{
    Q_OBJECT
public:
    TotalMultipleScoresList(const QVector<Score> &, QWidget *parent);

private:
    void addLineItem(const ItemArray &, uint index, QTreeWidgetItem *line);
    QString itemText(const ItemContainer &, uint row) const;

private:
    const QVector<Score> &_scores;
};

//-----------------------------------------------------------------------------
class ConfigDialog : public QDialog
{
 Q_OBJECT
 public:
    explicit ConfigDialog(QWidget *parent);

    bool hasBeenSaved() const { return _saved; }

 private slots:
    void modifiedSlot();
    void removeSlot();
    void accept();
    void slotApply() { save(); }
    void nickNameChanged(const QString &);

 private:
    bool         _saved;
    QCheckBox   *_WWHEnabled;
    QLineEdit   *_nickname, *_comment;
    KLineEdit   *_key, *_registeredName;
    QPushButton *_removeButton;
    QDialogButtonBox *buttonBox;

    void load();
    bool save();
};

//-----------------------------------------------------------------------------
class AskNameDialog : public QDialog
{
 Q_OBJECT
 public:
    explicit AskNameDialog(QWidget *parent);

    QString name() const { return _edit->text(); }
    bool dontAskAgain() const { return _checkbox->isChecked(); }

 private slots:
    void nameChanged(QDialogButtonBox *box);

 private:
    QLineEdit *_edit;
    QCheckBox *_checkbox;
};

} // namespace

#endif
