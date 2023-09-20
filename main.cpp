
#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QAction>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QMessageBox>

#include "window.h"


#include <cstdio>
#include <cstring>



int main(int argc, char* argv[]){
    QApplication app (argc, argv);

  QMainWindow *window = new QMainWindow;
  QMenuBar *tool_bar = new QMenuBar (window);
  Window *graph_area = new Window (window);
  QAction *action;

  if (graph_area->parse_command_line (argc, argv))
    {
      QMessageBox::warning (0, "Wrong input arguments!",
                            "Usage ... a b c d nx ny mx my k eps maxit p");
      return -1;
    }

  action = tool_bar->addAction ("Change function [&0]", graph_area, SLOT (change_func ()));
  action->setShortcut (QString ("0"));

  action = tool_bar->addAction ("Change disp [&1]", graph_area, SLOT (change_disp ()));
  action->setShortcut (QString ("1"));

  action = tool_bar->addAction ("Zoom in [&2]", graph_area, SLOT (zoom_in ()));
  action->setShortcut (QString ("2"));
  action = tool_bar->addAction ("Zoom out [&3]", graph_area, SLOT (zoom_out ()));
  action->setShortcut (QString ("3"));

  action = tool_bar->addAction ("Increase n [&4]", graph_area, SLOT (incr_n ()));
  action->setShortcut (QString ("4"));
  action = tool_bar->addAction ("Decrease n [&5]", graph_area, SLOT (decr_n ()));
  action->setShortcut (QString ("5"));

  action = tool_bar->addAction ("Add error [&6]", graph_area, SLOT (add_error ()));
  action->setShortcut (QString ("6"));
  action = tool_bar->addAction ("Substract error [&7]", graph_area, SLOT (substr_error ()));
  action->setShortcut (QString ("7"));

  action = tool_bar->addAction ("Increase m [&8]", graph_area, SLOT (incr_m ()));
  action->setShortcut (QString ("8"));
  action = tool_bar->addAction ("Decrease m [&9]", graph_area, SLOT (decr_m ()));
  action->setShortcut (QString ("9"));

  action = tool_bar->addAction ("E&xit", window, SLOT (close ()));
  action->setShortcut (QString ("Ctrl+X"));

  tool_bar->setMaximumHeight (30);

  window->setMenuBar (tool_bar);
  window->setCentralWidget (graph_area);
  window->setWindowTitle ("Graph");

  window->show ();
  app.exec ();
  Args* args = graph_area->args;
  int p = args->p;
  for (int q = 0;q<p;q++){
    pthread_join(args[q].tid,0);
  }
  delete tool_bar;
  delete graph_area;
  delete window;
  return 0;
}
