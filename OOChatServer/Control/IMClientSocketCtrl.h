#ifndef IMCLIENTSOCKETCTRL_H
#define IMCLIENTSOCKETCTRL_H

#include <QTcpSocket>
#include "IMDatabaseCtrl.h"
#include "../Model/IMConstant.h"

class QDateTime;
class IMUser;

/*************************************************
Class Name： IMClientSocketCtrl
Description: 来自客户端socket，用于处理各种请求
*************************************************/
class IMClientSocketCtrl : public QTcpSocket
{
  Q_OBJECT
public:
  IMClientSocketCtrl(QObject *parent);
  ~IMClientSocketCtrl();

signals:
  //请求server处理
  void sendSignal(const SaveTmpInformation &temp);
  //用户登录(在IMClientSocketCtrl中未使用)
  void userLoginSignal(const UserInformation &user);
  //用户断开连接
  void deleteSignal(const QString &acc);

public slots:
  //接收信息
  void receiveMessage();
  //发送信息
  void sendMessage(const SaveTmpInformation &temp);
  //删除scoket
  void deleteScoket();

private:
  quint16 m_blockSize;
  SaveTmpInformation m_save;
  IMDatabaseCtrl m_database;
};



#endif // IMCLIENTSOCKETCTRL_H
