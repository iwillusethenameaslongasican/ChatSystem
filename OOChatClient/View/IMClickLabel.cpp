/*********************************************
File Name： IMClickLabel.cpp
Author： hanyan
Date： 2016.11.20
Description： 具有单击响应的标签类
Changes：
********************************************/

#include "IMClickLabel.h"
#include <QMouseEvent>

//mark: public:-----------------------------------------
IMClickLabel::IMClickLabel(QWidget *parent)
  :QLabel(parent)
{
  QPalette pa;
  pa.setColor(QPalette::WindowText, Qt::darkGreen);
  setPalette(pa);
}

IMClickLabel::IMClickLabel(const QString &text, QWidget *parent)
  : QLabel(parent)
{
  setText(text);
}

//mark:protected-------------------------------------------------------
void IMClickLabel::mouseReleaseEvent(QMouseEvent *event)
{
  if(event->button() == Qt::LeftButton)
    {
      emit clicked();
    }
}

void IMClickLabel::enterEvent(QEvent *)
{
  QPalette pa;
  pa.setColor(QPalette::WindowText, Qt::blue);
  setPalette(pa);
}

void IMClickLabel::leaveEvent(QEvent *)
{
  QPalette pa;
  pa.setColor(QPalette::WindowText, Qt::darkBlue);
  setPalette(pa);
}
