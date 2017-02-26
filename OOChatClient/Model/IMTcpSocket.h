/*********************************************
File Name： IMTcpSocket.h
Author： hy
Date： 2016.11.14
Description： 客户端tcpsocket类
Changes：
********************************************/

#ifndef IMTCPSOCKET_H
#define IMTCPSOCKET_H

#include <QTcpSocket>
#include "IMContant.h"

/*************************************************
Class Name： IMTcpSocket
Description: 客户端tcpsocket类
*************************************************/
class IMTcpSocket: public QTcpSocket
{
  Q_OBJECT
public:
  IMTcpSocket(QObject *parent = 0);
  //~IMTcpSocket();
  //请求连接
  void requestConnect();
  //是否连接
  bool isConnected();

  void setFlag(int);
  int flag();

  //获取ip
  static QString getIP();
  //获取当前的日期和时间
  static QString getCurrentDateTime();

  static QHostAddress s_hostAddress;
  static int s_hostPort;

signals:
  void showConnectionStatus(const QString &, bool isLogin = false);

private slots:
  //连接被客户端关闭
  void connectionClosed();
  //连接创建
  void connectionCreate();
  //显示错误
  void displayError(QAbstractSocket::SocketError);

private:
  //连接信号与槽
  void linkSignalWithSlot();

  bool m_isConnected;
  int m_flag;
};

#endif // IMTCPSOCKET_H
