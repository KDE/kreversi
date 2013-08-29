#ifndef GAMESTARTINFORMATION_H
#define GAMESTARTINFORMATION_H

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
