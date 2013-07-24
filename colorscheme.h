#ifndef COLORSCHEME_H
#define COLORSCHEME_H

#include <QDeclarativeItem>
#include <KColorScheme>
#include <QColor>

class ColorScheme : public QDeclarativeItem
{
    Q_OBJECT
    Q_PROPERTY(QColor background READ background)
    Q_PROPERTY(QColor foreground READ foreground)
    Q_PROPERTY(QColor border READ border)

public:
    ColorScheme(QDeclarativeItem *parent = 0);

    QColor background() const;
    QColor foreground() const;
    QColor border() const;
};

#endif // COLORSCHEME_H
