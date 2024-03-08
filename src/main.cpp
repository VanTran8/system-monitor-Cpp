#include "ncurses_display.h"
#include "system.h"

#define MAX_ROW 23

int main() {
  System system;
  NCursesDisplay::Display(system, MAX_ROW);
}