#ifndef IMMAILWIDGET_H
#define IMMAILWIDGET_H

#include <QWidget>
#include <QMap>
#include "../Model/IMContant.h"

class QPushButton;
class QTabWidget;
class IMMailButton;
class IMMailOutWidget;
class IMMailInWidget;
class IMMailWriteWidget;
class QLabel;
class IMMailCtrl;
class IMMailInformationWidget;

class IMMailWidget: public QWidget
{
  Q_OBJECT
public:
  IMMailWidget(const QString &myID, QWidget *parent = 0);
  ~IMMailWidget();

  IMMailCtrl *getMailCtrl();
  QString getMyID();
  void showMailInformationWidget(const MailInformation &mail);
  void showMailInWidget();
  void showMailWriteWidget(const QString &address);

private slots:
  //设置收信箱
  void setInMailsWidget(const QVector<MailInformation> &);
  //设置发信箱
  void setOutMailsWidget(const QVector<MailInformation> &);

  //添加发件箱按钮
  bool addInMailButton(const MailInformation &);
  //添加发信箱按钮
  bool addOutMailButton(const MailInformation &);
  //删除邮件按钮
  void deleteMail(const int &);

  //添加新邮件到发信箱
  bool addNewOutMailButton(const MailInformation &);
  //添加新邮件到收信箱
  bool addNewInMailButton(const MailInformation &);

private:
  void initIMMailWidget();
  void linkSignalWithSlot();
  void clearContainers();

private:
  QString m_myID;

  QLabel *m_labelInformation;
  QTabWidget *m_tabWidget;
  IMMailOutWidget *m_outWidget;
  IMMailInWidget *m_inWidget;
  IMMailWriteWidget *m_writeWidget;
  IMMailInformationWidget *m_informationWidget;

  IMMailCtrl *m_mailCtrl;

  QMap<int, IMMailButton *> m_inMailMap;
  QMap<int, IMMailButton *> m_outMailMap;
};


#endif // IMMAILWIDGET_H
