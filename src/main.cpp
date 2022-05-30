#include <stdlib.h>
#include <string.h>
#include <spawn.h>
#include <wait.h>
#include <string>

#include <curses.h>
#include <menu.h>

#define ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]))
#define CTRLD 	4

char *choices[] = { "dwm", "kde", (char*) NULL};
extern char **environ;

void print_in_middle(WINDOW *win, int starty, int startx, int width, char *string, chtype color);
void startx(int choice);

int main() {
  ITEM **startx_items;
  int c;
  MENU *startx_menu;
  WINDOW *startx_menu_win;
  int n_choices, i;
  int win_height, win_width;

  /* Initialize curses */
  initscr();
  start_color();
  cbreak();
  noecho();
  keypad(stdscr, TRUE);
  init_pair(1, COLOR_RED, COLOR_BLACK);

  /* get current terminal height and width of the scrwin */
  getmaxyx(stdscr, win_height, win_width);

  /* Create items */
  n_choices = ARRAY_SIZE(choices);
  startx_items = (ITEM **)calloc(n_choices, sizeof(ITEM *));
  for(i = 0; i < n_choices; ++i) {
    startx_items[i] = new_item(choices[i], NULL);
  }

  /* Crate menu */
  startx_menu = new_menu((ITEM **) startx_items);

  /* Create the window to be associated with the menu */
  startx_menu_win = newwin(10, 40, win_height / 2 - 10, win_width / 2 - 15);

  keypad(startx_menu_win, TRUE);

  /* Set main window and sub window */
  set_menu_win(startx_menu, startx_menu_win);
  set_menu_sub(startx_menu, derwin(startx_menu_win, 6, 38, 3, 1));

  /* Set menu mark to the string " * " */
  set_menu_mark(startx_menu, " * ");

  /* Print a border around the main window and print a title */
  box(startx_menu_win, 0, 0);
  print_in_middle(startx_menu_win, 1, 0, 40, "Select WM/DE", COLOR_PAIR(1));
  mvwaddch(startx_menu_win, 2, 0, ACS_LTEE);
  mvwhline(startx_menu_win, 2, 1, ACS_HLINE, 38);
  mvwaddch(startx_menu_win, 2, 39, ACS_RTEE);
  mvprintw(LINES - 2, 0, "F1 to exit");
  refresh();

  /* Post the menu */
  post_menu(startx_menu);
  wrefresh(startx_menu_win);

  /* define choice {dwm, kde} corresponds to {0, 1} */
  int choice = 0;
  bool selected = false;

  while(!selected && (c = wgetch(startx_menu_win)) != KEY_F(1)) {
    switch(c) {
      case KEY_DOWN:
        choice = 1;
	    menu_driver(startx_menu, REQ_DOWN_ITEM);
		break;
	  case KEY_UP:
        choice = 0;
		menu_driver(startx_menu, REQ_UP_ITEM);
		break;
      case '\n':
        selected = true;
        break;
	}
    if (!selected)
      wrefresh(startx_menu_win);
  }

  /* Unpost and free all the memory taken up */
  unpost_menu(startx_menu);
  free_menu(startx_menu);
  for(i = 0; i < n_choices; ++i) {
    free_item(startx_items[i]);
  }
  free(startx_items);
  endwin();

  /* startx */
  startx(choice);
  return 0;
}

void print_in_middle(WINDOW *win, int starty, int startx, int width, char *string, chtype color) {
  int length, x, y;
  float temp;

  if(win == NULL)
    win = stdscr;

  getyx(win, y, x);

  if(startx != 0)
    x = startx;

  if(starty != 0)
    y = starty;

  if(width == 0)
    width = 80;

  length = strlen(string);
  temp = (width - length)/ 2;
  x = startx + (int)temp;
  wattron(win, color);
  mvwprintw(win, y, x, "%s", string);
  wattroff(win, color);
  refresh();
}

void startx(int choice) {
  std::string manager;

  switch(choice) {
    case 0: {
      manager = "dwm";
      char session[] = "DESKTOP_SESSION=dwm";
      putenv(session);
      break;
    }
    case 1: {
      manager = "kde";
      /* set environment variable */
      char session[] = "DESKTOP_SESSION=plasma";
      putenv(session);
      break;
    }
    default: {
      /* undefined manager */
      manager = "dwm";
      char session[] = "DESKTOP_SESSION=dwm";
      putenv(session);
      break;
    }
  }
  pid_t pid;
  std::string cmd_str = "exec startx ~/.xinitrc " + manager;
  char* cmd = strcpy(new char[cmd_str.length() + 1], cmd_str.c_str());
  char *argv[] = {cmd, (char *) NULL};
  int status;
  puts("Testing posix_spawn");
  fflush(NULL);
  status = posix_spawn(&pid, "/bin/startx", NULL, NULL, argv, environ);
  if (status == 0) {
    printf("Child id: %i\n", pid);
    fflush(NULL);
    if (waitpid(pid, &status, 0) != -1) {
      printf("Child exited with status %i\n", status);
    } else {
      perror("waitpid");
    }
  } else {
    printf("posix_spawn: %s\n", strerror(status));
  }
  delete[] cmd;
}
