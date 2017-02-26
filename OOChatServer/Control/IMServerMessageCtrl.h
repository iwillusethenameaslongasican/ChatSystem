#ifndef IMSERVERMESSAGECTRL_H
#define IMSERVERMESSAGECTRL_H

#include <QObject>
#include "IMDatabaseCtrl.h"

class IMClientSocketCtrl; //暂时没用到
class IMTcpServer;

class QStringListModel;
class QListView;


/*************************************************
Class Name： IMServerMessageCtrl
Description: 控制服务器
*************************************************/

class IMServerMessageCtrl : public QObject
{
  Q_OBJECT
public:
  IMServerMessageCtrl(QObject *parent);
  ~IMServerMessageCtrl();
  void runServer(); //用于启动服务器
  QListView *getIDListView();
  QListView *getAddressListView();

private slots:
  void insertIDData(const QString &id);
  void deleteIDData(const QString &id);

  void insertAddressData(const QString &address);
  void deleteAddressData(const QString &address);

private:
  IMTcpServer *m_tcpServer;
  IMDatabaseCtrl m_databaseCtrl;

  QStringList m_listID;
  QStringListModel *m_modelID;
  QListView *m_listViewID;

  QStringList m_listAddress;
  QStringListModel *m_modelAddress;
  QListView *m_listViewAddress;
};

#endif // IMSERVERMESSAGECTRL_H
