import QtQuick 1.1

Item {
    property color color
    property Gradient gradient

    /* Transformation in original is accessed through QTransform */
    property Scale scale
    property Rotation rotation
    property Translate translate

    /* In original there are texture access through QPixmap and QImage, what the
      difference? */
    property Image texture;
}
