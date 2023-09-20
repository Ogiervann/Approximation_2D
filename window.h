#ifndef WINDOW_H
#define WINDOW_H

#include <QtWidgets/QtWidgets>
#include <pthread.h>
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <ctime>
#include "my_thread.h"

class Window : public QWidget
{
  Q_OBJECT

private:
  int func_id = 0;
  bool in_progress_flag = false;
  const char *app_name = nullptr;
  int disp = 0;
  const char *f_name = nullptr;
  char info[100];
  int task = 3;
  double a = -10;
  double min_x = -10;
  double b = 10;
  double max_x = 10;
  double c = -10;
  double min_y = -10;
  double d = 10;
  double max_y = 10;
  int nx = 10;
  int ny = 10;
  int mx = 10;
  int my = 10;
  double eps = 1e-14;
  int maxit = 10;
  double (*f) (double, double) = nullptr;
  int N = 0;
  int lm = 0;
  int err_p = 0;
  double err = 0;
  double *A = nullptr;
  int* I = nullptr;
  double *B = nullptr;
  double *x = nullptr;
  double *r = nullptr;
  double* u = nullptr;
  double* v = nullptr;

  double r1 = -1;
  double r2 = -1;
  double r3 = -1;
  double r4 = -1;
  double t1 = -1;
  double t2 = -1;
  int it = -1;

  double abmax = 0;
  double min_f = 0;
  double max_f = 0;
  double fmm = -1;

  GResults gresults;
public:
  Window (QWidget *parent);
  ~Window();
  Args* args = nullptr;
  Results *res = nullptr;
  int p = 0;

  QSize minimumSizeHint () const;
  QSize sizeHint () const;

  int parse_command_line (int argc, char *argv[]);
  QPointF l2g (double x_loc, double y_loc);

  double get_color(double z);
  void paint(QPainter &painter);
  void set_minf_maxf();
  bool is_in_progress();

public slots:
  void change_func ();
  void change_disp ();

  void zoom_in();
  void zoom_out();

  void incr_n();
  void decr_n();

  void incr_m();
  void decr_m();
  void timer ();
  void add_error();
  void substr_error();
  void draw_triangle (QPainter & painter, QPointF point1, QPointF point2,
    QPointF point3, QColor color);

protected:
  void paintEvent (QPaintEvent *event);
};

#endif
