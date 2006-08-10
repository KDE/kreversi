#ifndef KREVERSI_MAIN_WINDOW_H
#define KREVERSI_MAIN_WINDOW_H

#include <kmainwindow.h>

class KReversiMainWindow : public KMainWindow
{
public:
    KReversiMainWindow(QWidget* parent=0);
private:
    void setupActions();
};
#endif
