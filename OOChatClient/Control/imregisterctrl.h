/*********************************************
File Name： IMRegisterCtrl.h
Author： hanyan
Date： 2016.11.4
Description： 用于控制注册的类
Changes：
********************************************/

#ifndef IMREGISTERCTRL_H
#define IMREGISTERCTRL_H

#include <QObject>
#include "../Model/IMContant.h"

class IMTcpSocket;

/*************************************************
Class Name： IMRegisterCtrl
Description: 用于控制注册的类
*************************************************/
class IMRegisterCtrl: public QObject
{
  Q_OBJECT
public:
  explicit IMRegisterCtrl(QObject *parent = 0);
  ~IMRegisterCtrl();

  //注册
  void registerAccout(const UserInformation &);

signals:
  void signalsResetBtnRegister(bool);
public slots:

private slots:
  //请求注册
  void requestRegister();
  //读取信息
  void readMessage();
  //重置按钮
  void resetBtnRegister();

private:
  UserInformation m_user;
  quint16 m_blockSize;
  IMTcpSocket *m_tcpSocket;
};

#endif // IMREGISTERCTRL_H
