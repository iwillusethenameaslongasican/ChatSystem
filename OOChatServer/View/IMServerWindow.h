#ifndef IMSERVERWINDOW_H
#define IMSERVERWINDOW_H

#include <QWidget>

class IMServerMessageCtrl;

/*************************************************
Class Name： IMServerWindow
Description: 服务器主界面类
*************************************************/

class IMServerWidget: public QWidget
{
  Q_OBJECT
public:
  IMServerWidget(QWidget *parent = 0);
  ~IMServerWidget();

private:
  IMServerMessageCtrl *m_serverCtrl;
};

#endif // IMSERVERWINDOW_H
