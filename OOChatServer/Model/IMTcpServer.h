#ifndef IMTCPSERVER_H
#define IMTCPSERVER_H

#include <QTcpServer>

#include "../Control/IMClientSocketCtrl.h"
#include "IMConstant.h"
#include "../Control/IMDatabaseCtrl.h"


/*************************************************
Class Name： IMTcpServer
Description: 服务器
*************************************************/
class IMTcpServer : public QTcpServer
{
  Q_OBJECT
public:
  IMTcpServer(QObject *parent = 0);
  ~IMTcpServer();

public slots:
  //客户端断开链接
  void clientDisconnected(const QString &id);
  //要求clientSocket发送消息
  void sendMessage(const SaveTmpInformation &temp);

signals:
  void haveNewID(const QString &id);
  void haveNewAddress(const QString &address);
  void haveLeftID(const QString &id);
  void haveLeftAddress(const QString &address);

private:
  //当有新的连接时，会调用这个函数。用于创建新的线程去控制Socket通信
  //incomingConnection是QTcpServer的虚函数
  void incomingConnection(int socketDescriptor);

  //处理好友请求
  void friendRequest(const SaveTmpInformation &save);
  //处理群请求
  void flockRequest(const SaveTmpInformation &save);
  //处理讨论组请求
  void discussionRequest(const SaveTmpInformation &save);
  //处理对话请求
  void talkRequest(const SaveTmpInformation &save);

  //通知好友，改变用户状态
  void tellFriendsStatusChanged(const QString &id, int status);
  //通知好友，改变用户信息
  void tellFriendsInformationChanged(const QString &id);
  //通知群成员,群被解散
  void tellMembersFlockDroped(const QString &flockID);
  //通知讨论组成员，讨论组被解散
  void tellMembersDiscussionDrop(const QString &discussionID);
  //通知群成员，群信息被改变
  void tellMembersFlockChanged(const FlockInformation &flock);
  //通知讨论组成员，讨论组信息被修改
  void tellMembersDiscussionChanged(const DiscussionInformation &flock);

  //通知群成员，有新的成员
  void tellFlockMembersHaveNewMember(const QString &flockID);
  //通知群成员，有成员退出
  void tellFlockMembersHaveLeaveMember(const QString &flockID);

  //通知讨论组成员，有新的成员
  void tellDiscussionMembersHaveNewMember(const QString &discussionID);
  //通知讨论组成员，有成员退出
  void tellDiscussionMembersHaveLeaveMember(const QString &discussionID);

private:
  //临时信息
  SaveTmpInformation m_save;
  //操作数据库
  IMDatabaseCtrl m_database;
  //在线用户列表
  QMap<QString, IMClientSocketCtrl *> m_userMap;
  //在线用户状态
  QMap<QString, int> m_userStatus;
  //邮箱在线用户
  QMap<QString, IMClientSocketCtrl *> m_userMailMap;

};

#endif // IMTCPSERVER_H
