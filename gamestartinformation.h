#ifndef GAMESTARTINFORMATION_H
#define GAMESTARTINFORMATION_H

#include <QString>

class GameStartInformation {

public:
    enum PlayerType {
        Human,
        AI
    };

    PlayerType type[2];
    QString name[2];
    int skill[2];
};

#endif // GAMESTARTINFORMATION_H
