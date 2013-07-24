#include "colorscheme.h"

ColorScheme::ColorScheme(QDeclarativeItem *parent) :
    QDeclarativeItem(parent)
{
}

QColor ColorScheme::background() const
{
    return KStatefulBrush(KColorScheme::Tooltip,
                          KColorScheme::NormalBackground)
            .brush(QPalette::Active).color();
}

QColor ColorScheme::foreground() const
{
    return KStatefulBrush(KColorScheme::Tooltip,
                          KColorScheme::NormalText)
            .brush(QPalette::Active).color();
}

QColor ColorScheme::border() const
{
    return KStatefulBrush(KColorScheme::View,
                          KColorScheme::NormalText)
            .brush(QPalette::Active).color();
}
