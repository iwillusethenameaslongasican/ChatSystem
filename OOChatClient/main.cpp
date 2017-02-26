#include "mainwindow.h"
#include "View/IMLoginWidget.h"
#include <QApplication>
#include <QDebug>

int main(int argc, char *argv[])
{
  QApplication a(argc, argv);
  IMLoginWidget w;
  w.show();

  if(true == w.getIsAutoLogin())
    {
      qDebug() << "click";
      w.onClickBtnLogin();
    }

  return a.exec();
}
