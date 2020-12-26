/*
    SPDX-FileCopyrightText: 2013 Denis Kuplyakov <dener.kup@gmail.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

/* TODO: make this two constant below accessible through c++ to use them
 everywhere instead of hardcoded 8 x 8 */

/**
  * Number of columns on board
  */
var COLUMN_COUNT = 8;

/**
  * Number of rows on board
  */
var ROW_COUNT = 8;

/*
    TODO: this is theme specific constants below. It will be good to connect
    them with theme
*/

/**
  * X-coordinate offset in percents of playable area from beginning of board's image
  */
var GRID_OFFSET_X_PERCENT = 0.1;
/**
  * Y-coordinate offset in percents of playable area from beginning of board's image
  */
var GRID_OFFSET_Y_PERCENT = 0.1;
/**
  * Playable area's width part of board's image's width
  */
var GRID_WIDTH_PERCENT = 0.8;
/**
  * Playable area's height part of board's image's height
  */
var GRID_HEIGHT_PERCENT = 0.8;
/**
  * Number of chip's animation framesCount
  */
var CHIP_ANIMATION_FRAMES_COUNT = 12
