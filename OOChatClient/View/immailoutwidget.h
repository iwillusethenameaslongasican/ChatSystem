#ifndef IMMAILOUTWIDGET_H
#define IMMAILOUTWIDGET_H

#include "IMMailListWidget.h"

class IMMailCtrl;

class IMMailOutWidget: public IMMailListWidget
{
  Q_OBJECT
public:
  IMMailOutWidget(IMMailCtrl *ctrl, QWidget *parent = 0);

private:
  IMMailCtrl *m_mailCtrl;
};

#endif // IMMAILOUTWIDGET_H
