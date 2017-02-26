#ifndef IMMAILINWIDGET_H
#define IMMAILINWIDGET_H

#include "IMMailListWidget.h"

class IMMailCtrl;

class IMMailInWidget: public IMMailListWidget
{
  Q_OBJECT
public:
  IMMailInWidget(IMMailCtrl *, QWidget *parent = 0);

private:
  IMMailCtrl *m_mailCtrl;
};

#endif // IMMAILINWIDGET_H
