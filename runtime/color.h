// https://gist.github.com/dafrancis/1365246
#ifndef __color_h__
#define __color_h__

/*
 * I got some of the colour codes (as well as having the idea of putting them in a macro) from here:
 * http://stackoverflow.com/questions/3506504/c-code-changes-terminal-text-color-how-to-restore-defaults-linux
 */

#define RED    "\e[31m"
#define GREEN  "\e[32m"
#define YELLOW "\e[33m"
#define WHITE  "\e[1m"

/*
 * COLOR_X resets the colour. Yes I'm inconsistent with how I spell colour.
 */
#define COLOR_X "\e[m"

#endif
