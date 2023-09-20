
#include <QPainter>
#include <stdio.h>

#include "window.h"

#define DEFAULT_A -10
#define DEFAULT_B 10
#define DEFAULT_C -10
#define DEFAULT_D 10
#define L2G(X,Y) (l2g ((X), (Y)))


bool Window::is_in_progress(){
  return in_progress_flag;
}


Window::Window (QWidget *parent)
  : QWidget (parent)
{
  a = DEFAULT_A;
  b = DEFAULT_B;
  c = DEFAULT_C;
  d = DEFAULT_D;
  func_id = 0;
  f_name = "f(x,y) = 1";
  f = f_0;
}

Window::~Window() {
  delete[] A;
  delete[] I;
  delete[] B;
  delete[] x;
  delete[] r;
  delete[] u;
  delete[] v;
  delete[] res;
  delete[] args;
}

void Window::timer (){
  if (is_in_progress()){
    QTimer::singleShot(500, this, &Window::timer);
  } else {

      r1 = res[0].r1;
      r2 = res[0].r2;
      r3 = res[0].r3;
      r4 = res[0].r4;
      it = res[0].it;
      t1 = res[0].alg_time;
      t2 = res[0].res_time;

      update();
  }
}

QSize Window::minimumSizeHint () const
{
  return QSize (100, 100);
}

QSize Window::sizeHint () const
{
  return QSize (1000, 1000);
}

int Window::parse_command_line (int argc, char *argv[])
{

    if(!(argc == 13 && sscanf(argv[1], "%lf", &a) == 1 &&
      sscanf(argv[2], "%lf", &b) == 1 && sscanf(argv[3], "%lf", &c) == 1 &&
      sscanf(argv[4], "%lf", &d) == 1 && sscanf(argv[5], "%d", &nx) == 1 &&
      sscanf(argv[6], "%d", &ny) == 1 && sscanf(argv[7], "%d", &mx) == 1 &&
      sscanf(argv[8], "%d", &my) == 1 && sscanf(argv[9], "%d", &func_id) == 1 &&
      sscanf(argv[10], "%lf", &eps) == 1 && sscanf(argv[11], "%d", &maxit) == 1 &&
      sscanf(argv[12], "%d", &p) == 1)){
        printf("Usage %s a b c d nx ny mx my k eps maxit p\n", argv[0]);
        return 1;
    }
    app_name = argv[0];
      lm = get_len_msr(nx, ny);
      N = (nx+1)*(ny+1);
      args = new Args[p];
      res = new Results[p];

      min_x = a;
      max_x = b;
      min_y = c;
      max_y = d;
      func_id--;
      change_func();
      set_minf_maxf();
  return 0;
}

/// change the displayed function
void Window::change_disp()
{
  
  disp = (disp + 1) % 3;
  update();
}

void Window::zoom_in()
{

  double t = (3*min_x + max_x)/4;
  max_x = (min_x + 3*max_x)/4;
  min_x = t;

  t = (3*min_y + max_y)/4;
  max_y = (min_y + 3*max_y)/4;
  min_y = t;
  update();
}


void Window::zoom_out()
{


  double t = (3*min_x - max_x)/2;
  max_x = (3*max_x - min_x)/2;
  min_x = t;

  t = (3*min_y - max_y)/2;
  max_y = (3*max_y - min_y)/2;
  min_y = t;

  update();
}

void Window::incr_n()
{
  if(is_in_progress()){
    QMessageBox::warning(0, "Wait!", "In progress!");
    return ;
  }
  in_progress_flag = true;
  err_p = 0;
  nx *= 2;
  ny *= 2;

  if(A != NULL) delete[] A;
  if(I != NULL) delete[] I;
  if(B != NULL) delete[] B;
  if(x != NULL) delete[] x;
  if(r != NULL) delete[] r;
  if(u != NULL) delete[] u;
  if(v != NULL) delete[] v;
  lm = get_len_msr(nx, ny);
  N = (nx+1)*(ny+1);
  A = new double[N+1+lm];
  memset(A, 0, (N+1+lm) * sizeof(double));

  I = new int[N+1+lm];
  memset(I, 0, (N+1+lm) * sizeof(int));

  B = new double[N];
  memset(B, 0, N * sizeof(double));

  x = new double[N];
  memset(x, 0, N * sizeof(double));

  r = new double[N];
  memset(r, 0, N * sizeof(double));

  u = new double[N];
  memset(u, 0, N * sizeof(double));

  v = new double[N];
  memset(v, 0, N * sizeof(double));

  for(int k = 0; k < p; k++){
    args[k].res = res;
    args[k].a = a;
    args[k].b = b;
    args[k].c = c;
    args[k].d = d;
    args[k].nx = nx;
    args[k].ny = ny;
    args[k].eps = eps;
    args[k].maxit = maxit;
    args[k].func_id = func_id;
    args[k].set_func();
    args[k].k = k;
    args[k].p = p;
    args[k].err = err;
    args[k].err_p = err_p;
    args[k].in_progress_flag = &in_progress_flag;
    args[k].gresults.in_progress_flag = true;

    args[k].A = A;
    args[k].I = I;
    args[k].B = B;
    args[k].x = x;
    args[k].r = r;
    args[k].u = u;
    args[k].v = v;
  }

  for(int k = 0; k < p; k++){
    if(pthread_create(&args[k].tid, 0, thread_func, args+k)){
      printf("Cannot create thread %d\n", k);

        if(A != NULL) delete[] A;
        if(I != NULL) delete[] I;
        if(B != NULL) delete[] B;
        if(x != NULL) delete[] x;
        if(r != NULL) delete[] r;
        if(u != NULL) delete[] u;
        if(v != NULL) delete[] v;
      std::abort();
    }//if
  }//for

  QTimer::singleShot(250, this, &Window::timer);

if(!is_in_progress()){

  r1 = res[0].r1;
  r2 = res[0].r2;
  r3 = res[0].r3;
  r4 = res[0].r4;
  it = res[0].it;
  t1 = res[0].alg_time;
  t2 = res[0].res_time;

  update();
}
}

void Window::decr_n()
{
  if(nx <= 5) return;
  if(ny <= 5) return;

  if(is_in_progress()){
    QMessageBox::warning(0, "Wait!", "In progress!");
    return ;
  }

  in_progress_flag = true;

  err_p = 0;
  nx /= 2;
  ny /= 2;

  if(A != NULL) delete[] A;
  if(I != NULL) delete[] I;
  if(B != NULL) delete[] B;
  if(x != NULL) delete[] x;
  if(r != NULL) delete[] r;
  if(u != NULL) delete[] u;
  if(v != NULL) delete[] v;
  lm = get_len_msr(nx, ny);
  N  = (nx+1)*(ny+1);
  A = new double[N+1+lm];
  memset(A, 0, (N+1+lm) * sizeof(double));

  I = new int[N+1+lm];
  memset(I, 0, (N+1+lm) * sizeof(int));

  B = new double[N];
  memset(B, 0, N * sizeof(double));

  x = new double[N];
  memset(x, 0, N * sizeof(double));

  r = new double[N];
  memset(r, 0, N * sizeof(double));

  u = new double[N];
  memset(u, 0, N * sizeof(double));

  v = new double[N];
  memset(v, 0, N * sizeof(double));

  for(int k = 0; k < p; k++){
    args[k].res = res;
    args[k].a = a;
    args[k].b = b;
    args[k].c = c;
    args[k].d = d;
    args[k].nx = nx;
    args[k].ny = ny;
    args[k].eps = eps;
    args[k].maxit = maxit;
    args[k].func_id = func_id;
    args[k].set_func();
    args[k].k = k;
    args[k].p = p;
    args[k].err = err;
    args[k].err_p = err_p;
    args[k].in_progress_flag = &in_progress_flag;
    args[k].gresults.in_progress_flag = true;

    args[k].A = A;
    args[k].I = I;
    args[k].B = B;
    args[k].x = x;
    args[k].r = r;
    args[k].u = u;
    args[k].v = v;
  }


  for(int k = 0; k < p; k++){
    if(pthread_create(&args[k].tid, 0, thread_func, args+k)){
      printf("Cannot create thread %d\n", k);

        if(A != NULL) delete[] A;
        if(I != NULL) delete[] I;
        if(B != NULL) delete[] B;
        if(x != NULL) delete[] x;
        if(r != NULL) delete[] r;
        if(u != NULL) delete[] u;
        if(v != NULL) delete[] v;
      std::abort();
    }//if
  }//for

  QTimer::singleShot(250, this, &Window::timer);


  if(!is_in_progress()){

    r1 = res[0].r1;
    r2 = res[0].r2;
    r3 = res[0].r3;
    r4 = res[0].r4;
    it = res[0].it;
    t1 = res[0].alg_time;
    t2 = res[0].res_time;

    update();
  }
}

void Window::incr_m()
{

  mx *=2;
  my *=2;
  update();
}

void Window::decr_m()
{

  if ((mx<=5) || (my<=5)) return ;
  mx /=2;
  my /=2;
  update();
}

void Window::add_error()
{
  if(is_in_progress()){
    QMessageBox::warning(0, "Wait!", "In progress!");
    return ;
  }

  in_progress_flag = true;

  err_p++;

  err = 0.1 * fmm;

  if(A != NULL) delete[] A;
  if(I != NULL) delete[] I;
  if(B != NULL) delete[] B;
  if(x != NULL) delete[] x;
  if(r != NULL) delete[] r;
  if(u != NULL) delete[] u;
  if(v != NULL) delete[] v;

  A = new double[N+1+lm];
  memset(A, 0, (N+1+lm) * sizeof(double));

  I = new int[N+1+lm];
  memset(I, 0, (N+1+lm) * sizeof(int));

  B = new double[N];
  memset(B, 0, N * sizeof(double));

  x = new double[N];
  memset(x, 0, N * sizeof(double));

  r = new double[N];
  memset(r, 0, N * sizeof(double));

  u = new double[N];
  memset(u, 0, N * sizeof(double));

  v = new double[N];
  memset(v, 0, N * sizeof(double));

  for(int k = 0; k < p; k++){
    args[k].res = res;
    args[k].a = a;
    args[k].b = b;
    args[k].c = c;
    args[k].d = d;
    args[k].nx = nx;
    args[k].ny = ny;
    args[k].eps = eps;
    args[k].maxit = maxit;
    args[k].func_id = func_id;
    args[k].set_func();
    args[k].k = k;
    args[k].p = p;
    args[k].err = err;
    args[k].err_p = err_p;
    args[k].in_progress_flag = &in_progress_flag;
    args[k].gresults.in_progress_flag = true;

    args[k].A = A;
    args[k].I = I;
    args[k].B = B;
    args[k].x = x;
    args[k].r = r;
    args[k].u = u;
    args[k].v = v;
  }


  for(int k = 0; k < p; k++){
    if(pthread_create(&args[k].tid, 0, thread_func, args+k)){
      printf("Cannot create thread %d\n", k);

        if(A != NULL) delete[] A;
        if(I != NULL) delete[] I;
        if(B != NULL) delete[] B;
        if(x != NULL) delete[] x;
        if(r != NULL) delete[] r;
        if(u != NULL) delete[] u;
        if(v != NULL) delete[] v;
        if(res != NULL) delete[] res;
        if(args != NULL)delete[] args;
      std::abort();
    }//if
  }//for
  QTimer::singleShot(250, this, &Window::timer);



  if(!is_in_progress()){

    r1 = res[0].r1;
    r2 = res[0].r2;
    r3 = res[0].r3;
    r4 = res[0].r4;
    it = res[0].it;
    t1 = res[0].alg_time;
    t2 = res[0].res_time;

    update();
  }
}

void Window::substr_error()
{
  if(in_progress_flag){
    QMessageBox::warning(0, "Wait!", "In progress!");
    return ;
  }

    err_p--;

    err = 0.1 * fmm;

    if(A != NULL) delete[] A;
    if(I != NULL) delete[] I;
    if(B != NULL) delete[] B;
    if(x != NULL) delete[] x;
    if(r != NULL) delete[] r;
    if(u != NULL) delete[] u;
    if(v != NULL) delete[] v;

    A = new double[N+1+lm];
    memset(A, 0, (N+1+lm) * sizeof(double));

    I = new int[N+1+lm];
    memset(I, 0, (N+1+lm) * sizeof(int));

    B = new double[N];
    memset(B, 0, N * sizeof(double));

    x = new double[N];
    memset(x, 0, N * sizeof(double));

    r = new double[N];
    memset(r, 0, N * sizeof(double));

    u = new double[N];
    memset(u, 0, N * sizeof(double));

    v = new double[N];
    memset(v, 0, N * sizeof(double));

    for(int k = 0; k < p; k++){
      args[k].res = res;
      args[k].a = a;
      args[k].b = b;
      args[k].c = c;
      args[k].d = d;
      args[k].nx = nx;
      args[k].ny = ny;
      args[k].eps = eps;
      args[k].maxit = maxit;
      args[k].func_id = func_id;
      args[k].set_func();
      args[k].k = k;
      args[k].p = p;
      args[k].err = err;
      args[k].err_p = err_p;
      args[k].in_progress_flag = &in_progress_flag;
      args[k].gresults.in_progress_flag = true;

      args[k].A = A;
      args[k].I = I;
      args[k].B = B;
      args[k].x = x;
      args[k].r = r;
      args[k].u = u;
      args[k].v = v;
    }



    for(int k = 0; k < p; k++){
      if(pthread_create(&args[k].tid, 0, thread_func, args+k)){
        printf("Cannot create thread %d\n", k);

          if(A != NULL) delete[] A;
          if(I != NULL) delete[] I;
          if(B != NULL) delete[] B;
          if(x != NULL) delete[] x;
          if(r != NULL) delete[] r;
          if(u != NULL) delete[] u;
          if(v != NULL) delete[] v;
          if(res != NULL) delete[] res;
          if(args != NULL)delete[] args;
        std::abort();
      }//if
    }//for
    QTimer::singleShot(250, this, &Window::timer);



    if(!is_in_progress()){

      r1 = res[0].r1;
      r2 = res[0].r2;
      r3 = res[0].r3;
      r4 = res[0].r4;
      it = res[0].it;
      t1 = res[0].alg_time;
      t2 = res[0].res_time;

      update();
    }

}



/// change current function for drawing
void Window::change_func ()
{
  if(is_in_progress()){
    QMessageBox::warning(0, "Wait!", "In progress!");
    return ;
  }

  in_progress_flag = true;


  func_id = (func_id + 1) % 8;

  switch (func_id)
  {
    case 0:
    f_name = "k=0 f(x,y)=1";
    f = f_0;
    break;
    case 1:
    f_name = "k=1 f(x,y)=x";
    f = f_1;
    break;
    case 2:
    f_name = "k=2 f(x,y)=y";
    f = f_2;
    break;
    case 3:
    f_name = "k=3 f(x,y)=x+y";
    f = f_3;
    break;
    case 4:
    f_name = "k=4 f(x,y)=sqrt(x^2+y^2)";
    f = f_4;
    break;
    case 5:
    f_name = "k=5 f(x,y)=x^2+y^2";
    f = f_5;
    break;
    case 6:
    f_name = "k=6 f(x)=exp(x^2-y^2)";
    f = f_6;
    break;
    case 7:
    f_name = "k=7 f(x)=1/(25*(x^2+y^2)+1)";
    f = f_7;
    break;
  }


    if(A != NULL) delete[] A;
    if(I != NULL) delete[] I;
    if(B != NULL) delete[] B;
    if(x != NULL) delete[] x;
    if(r != NULL) delete[] r;
    if(u != NULL) delete[] u;
    if(v != NULL) delete[] v;

    A = new double[N+1+lm];
    memset(A, 0, (N+1+lm) * sizeof(double));

    I = new int[N+1+lm];
    memset(I, 0, (N+1+lm) * sizeof(int));

    B = new double[N];
    memset(B, 0, N * sizeof(double));

    x = new double[N];
    memset(x, 0, N * sizeof(double));

    r = new double[N];
    memset(r, 0, N * sizeof(double));

    u = new double[N];
    memset(u, 0, N * sizeof(double));

    v = new double[N];
    memset(v, 0, N * sizeof(double));

    err_p = 0;

    for(int k = 0; k < p; k++){
      args[k].res = res;
      args[k].a = a;
      args[k].b = b;
      args[k].c = c;
      args[k].d = d;
      args[k].nx = nx;
      args[k].ny = ny;
      args[k].eps = eps;
      args[k].maxit = maxit;
      args[k].func_id = func_id;
      args[k].set_func();
      args[k].k = k;
      args[k].p = p;
      args[k].err = err;
      args[k].err_p = err_p;
      args[k].in_progress_flag = &in_progress_flag;
      args[k].gresults.in_progress_flag = true;

      args[k].A = A;
      args[k].I = I;
      args[k].B = B;
      args[k].x = x;
      args[k].r = r;
      args[k].u = u;
      args[k].v = v;
    }

    for(int k = 0; k < p; k++){
      if(pthread_create(&args[k].tid, 0, thread_func, args+k)){
        printf("Cannot create thread %d\n", k);

          if(A != NULL) delete[] A;
          if(I != NULL) delete[] I;
          if(B != NULL) delete[] B;
          if(x != NULL) delete[] x;
          if(r != NULL) delete[] r;
          if(u != NULL) delete[] u;
          if(v != NULL) delete[] v;
          if(res != NULL) delete[] res;
          if(args != NULL)delete[] args;
        std::abort();
      }//if
    }//for

    QTimer::singleShot(250, this, &Window::timer);


if(!is_in_progress()){

    r1 = res[0].r1;
    r2 = res[0].r2;
    r3 = res[0].r3;
    r4 = res[0].r4;
    it = res[0].it;
    t1 = res[0].alg_time;
    t2 = res[0].res_time;

  update ();
}


}

QPointF Window::l2g (double x_loc, double y_loc)
{
  double x_gl = (x_loc - min_x) / (max_x - min_x) * (width ()-1);
  double y_gl = (max_y - y_loc) / (max_y - min_y) * (height ()-1);
  return QPointF (x_gl, y_gl);
}

double Window::get_color(double z){
  if(z > max_f) return 1;
  if(z < min_f) return 0;
  return (z - min_f) / (max_f-min_f);

}

void Window::draw_triangle (QPainter & painter, QPointF point1, QPointF point2,
  QPointF point3, QColor color)
{
	QPolygonF polygon;
  painter.setBrush(color);
	polygon << point1 << point2 << point3;
	painter.drawPolygon(polygon);
}

void Window::paint(QPainter &painter){
  int i = 0, j = 0;
  double x1, x2, y1, y2;
  double hx = (b-a)/nx;
  double hy = (d-c)/ny;
  double disp_hx = (max_x-min_x)/mx;
  double disp_hy = (max_y-min_y)/my;
  double z = 0;
   int red = 0;
   int green = 0;
   int blue = 0;
   double color;
   for(i = 0; i < mx; i++){
     for(j = 0; j < my; j++){
       x1 = min_x+i*disp_hx;
       x2 = min_x+(i+1)*disp_hx;
       y1 = min_y + j*disp_hy;
       y2 = min_y + (j+1)*disp_hy;
       if(disp == 0){

         z = f(x1 +2*disp_hx/3, y1+disp_hy/3);
         color = 30 + 150 * get_color(z);
         red = color;
         green = 50;
         blue = 50;

         draw_triangle (painter, L2G(x1, y1), L2G(x2, y1), L2G(x2, y2),
          QColor(red, green, blue));

         z = f(x1 +disp_hx/3, y1+2*disp_hy/3);
         color = 30 + 150 * get_color(z);
         red = color;
         green = 50;
         blue = 50;

         draw_triangle (painter, L2G(x1, y1), L2G(x1, y2), L2G(x2, y2),
          QColor(red, green, blue));

       }else if(disp == 1){
         z = Pf(x, x1 +2*disp_hx/3, y1+disp_hy/3,
           nx, ny, a, c, hx, hy);
           color = 30 + 150 * get_color(z);
           red = color;
           green = 50;
           blue = 50;

           draw_triangle (painter, L2G(x1, y1), L2G(x2, y1), L2G(x2, y2),
            QColor(red, green, blue));



         z = Pf(x, x1 +disp_hx/3, y1+2*disp_hy/3,
           nx, ny, a, c, hx, hy);
           color = 30 + 150 * get_color(z);
           red = color;
           green = 50;
           blue = 50;

           draw_triangle (painter, L2G(x1, y1), L2G(x1, y2), L2G(x2, y2),
            QColor(red, green, blue));

       }else{
         z = fabs(f(x1 +2*disp_hx/3, y1+disp_hy/3) -
         Pf(x, x1 +2*disp_hx/3, y1+disp_hy/3,
           nx, ny, a, c, hx, hy));
           color = 30 + 150 * get_color(z);
           red = color;
           green = 50;
           blue = 50;

           draw_triangle (painter, L2G(x1, y1), L2G(x2, y1), L2G(x2, y2),
            QColor(red, green, blue));


         z = fabs(f(x1 +disp_hx/3, y1+2*disp_hy/3) -
         Pf(x, x1 +disp_hx/3, y1+2*disp_hy/3,
           nx, ny, a, c, hx, hy));
           color = 30 + 150 * get_color(z);

           red = color;
           green = 50;
           blue = 50;

           draw_triangle (painter, L2G(x1, y1), L2G(x1, y2), L2G(x2, y2),
            QColor(red, green, blue));


       }
     }
   }
}

void Window::set_minf_maxf(){
  int i,j;
  double hx = (b-a)/nx;
  double hy = (d-c)/ny;
  double disp_hx = (max_x-min_x)/mx;
  double disp_hy = (max_y-min_y)/my;
  bool start = true;
  min_f = 0;
  max_f = 0;
  double z1 = 0, z2 = 0;
  for(i = 0; i < mx; i++){
    for(j = 0; j < my; j++){
      if(disp == 0){
        z1 = f(min_x + i*disp_hx +2*disp_hx/3, min_y+j*disp_hy+disp_hy/3);
        z2 = f(min_x + i*disp_hx +disp_hx/3, min_y+j*disp_hy+2*disp_hy/3);
      }else if(disp == 1){
        z1 = Pf(x, min_x + i*disp_hx +2*disp_hx/3, min_y+j*disp_hy+disp_hy/3,
          nx, ny, a, c, hx, hy);
        z2 = Pf(x, min_x + i*disp_hx +disp_hx/3, min_y+j*disp_hy+2*disp_hy/3,
          nx, ny, a, c, hx, hy);
      }else{
        z1 = fabs(f(min_x + i*disp_hx +2*disp_hx/3, min_y+j*disp_hy+disp_hy/3) -
        Pf(x, min_x + i*disp_hx +2*disp_hx/3, min_y+j*disp_hy+disp_hy/3,
          nx, ny, a, c, hx, hy));
        z2 = fabs(f(min_x + i*disp_hx +disp_hx/3, min_y+j*disp_hy+2*disp_hy/3) -
        Pf(x, min_x + i*disp_hx +disp_hx/3, min_y+j*disp_hy+2*disp_hy/3,
          nx, ny, a, c, hx, hy));
      }
      if(start){
        min_f = z1;
        max_f = z1;
        start = false;
      }
      if(z1 < min_f) min_f = z1;
      if(z1 > max_f) max_f = z1;
      if(z2 < min_f) min_f = z2;
      if(z2 > max_f) max_f = z2;
    }
  }

  fmm = (fabs(max_f) > fabs(min_f)? fabs(max_f): fabs(min_f));

  if(fabs(max_f - min_f) < 1e-16){
    max_f += 1e-14;
    min_f -= 1e-14;
  }
}

/// render graph
void Window::paintEvent (QPaintEvent * /* event */)
{
  QPainter painter (this);
  QPen pen_black(Qt::black, 0, Qt::SolidLine);
  QPen pen_red(Qt::red, 0, Qt::SolidLine);
  QPen pen_blue(Qt::blue, 0, Qt::SolidLine);
  QPen pen_green(Qt::green, 0, Qt::SolidLine);

  painter.setPen (pen_black);
  printf (
  "%s : Task = %d R1 = %e R2 = %e R3 = %e R4 = %e T1 = %.2f T2 = %.2f \
It = %d E = %e K = %d Nx = %d Ny = %d P = %d\n",
  app_name, task, r1, r2, r3, r4, t1, t2, it, eps, func_id, nx, ny, p);

  set_minf_maxf();
  paint(painter);
  printf("max{|F_min|, |F_max|} = %e\n", fmm);


  // render function name
  painter.setPen ("white");
  painter.drawText (0, 20, f_name);
  if (disp == 0) {
    painter.drawText(0,40, "disp 0 : f(x,y) ");
  }
  if (disp == 1) {
    painter.drawText(0,40, "disp 1 : Pf(x,y)");
  }
  if (disp == 2) {
    painter.drawText(0,40, "disp 2: f(x,y)-Pf(x,y)");
  }

  sprintf(info, "nx = %d, ny = %d\n", nx, ny);
  painter.drawText (0, 60, info);

  sprintf(info, "mx = %d, my = %d\n", mx, my);
  painter.drawText (0, 80, info);

  sprintf(info, "real:a = %lf, b = %lf, c = %lf, d = %lf,\n", a,b,c,d);
  painter.drawText (0, 100, info);

  sprintf(info, "disp:a = %lf, b = %lf, c = %lf, d = %lf,\n",
  min_x, max_x, min_y, max_y);
  painter.drawText (0, 120, info);

  sprintf(info, "err_p = %d, |f|_max = %e\n", err_p, fmm);
  painter.drawText (0, 140, info);

}
