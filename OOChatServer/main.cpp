#include "mainwindow.h"
#include "View/IMServerWindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
  QApplication a(argc, argv);
  IMServerWidget w;;
  w.show();

  return a.exec();
}
