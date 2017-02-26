#ifndef IMMESSAGEMANAGEWIDGET_H
#define IMMESSAGEMANAGEWIDGET_H

#include <QWidget>
#include "../Model/IMContant.h"

class IMMainWidget;
class IMLocalMessageWidget;
class IMNetworkMessageWidget;
class QTabWidget;

class IMMessageManageWidget: public QWidget
{
  Q_OBJECT
public:
  IMMessageManageWidget(IMMainWidget *mainWidget, QWidget *parent = 0);
  ~IMMessageManageWidget();

  void setLocalMessageWidget(const QVector<TalkMessage> &msgVec);
  void setNetworkMessageWidget(const QVector<TalkMessage> &msgVec);

  void setListModelGroup();

private:
  IMMainWidget *m_mainWidget;
  QTabWidget *m_tabWidget;
  IMLocalMessageWidget *m_localWidget;
  IMNetworkMessageWidget *m_networkWidget;
};


#endif // IMMESSAGEMANAGEWIDGET_H
