/*********************************************
File Name： IMLoginCtrl.cpp
Author： hanyan
Date： 2016.11.11
Description： 用于控制登录的类
Changes：
********************************************/

#include "IMLoginCtrl.h"
#include "../Model/IMContant.h"
#include <QMessageBox>
#include <QHostAddress>
#include <QDataStream>
#include <QDateTime>
#include <QtNetwork>
#include <QInputDialog>
#include "../Model/IMEncryption.h"

//public:-------------------------------------------------------------
IMLoginCtrl::IMLoginCtrl(QObject *parent)
  :QObject(parent)
{
  m_tcpSocket = new IMTcpSocket(this);
  m_tcpSocket->setFlag(1);

  connect(m_tcpSocket, &IMTcpSocket::showConnectionStatus,
          this, &IMLoginCtrl::changeLoginMessage);

  connect(m_tcpSocket, &IMTcpSocket::connected,
          this, &IMLoginCtrl::sendRequest);
  connect(m_tcpSocket, &IMTcpSocket::readyRead,
          this, &IMLoginCtrl::readMessage);
}

IMLoginCtrl::~IMLoginCtrl()
{

}

/*************************************************
Function Name： login()
Description: 发送登录信息
Input： const QString &id： 用户帐号, const QString &pwd：用户密码
*************************************************/
void IMLoginCtrl::login(const QString &id, const QString &pwd, const int status)
{
  m_kind = LOGIN;
  m_loginInfo.m_userID = id;
  m_loginInfo.m_password = pwd;
  m_loginInfo.m_status = status;

  if(m_tcpSocket->isConnected())
    {
      requestLogin();
    }
  else
    {
      m_blockSize = 0;
      m_tcpSocket->requestConnect();
    }
}

/*************************************************
Function Name： getQuestionAndAnswer()
Description: 获取密保信息
*************************************************/
void IMLoginCtrl::getQuestionAndAnswer(const QString &id)
{
  m_kind = GET_QUESTION_ANSWER;
  m_id = id;
  if(m_tcpSocket->isConnected())
    {
      requestGetQuestionAndAnswer();
    }
  else
    {
      m_blockSize = 0;
      m_tcpSocket->requestConnect();
    }
}

// mark: public slots:--------------------------------------------------------
/*************************************************
Function Name： changeLoginMessage()
Description: 改变登录信息
Input： const QString &mes： 信息, bool isLogin：状态
Output： NULL
Changes： NULL
*************************************************/
void IMLoginCtrl::changeLoginMessage(const QString &msg, bool isLogin)
{
  emit getLoginMessage(msg, isLogin);
}

// mark: private slots:--------------------------------------------------

/*************************************************
Function Name： requestLogin()
Description: 请求登录
Input： NULL
Output： NULL
Changes： NULL
*************************************************/
void IMLoginCtrl::requestLogin()
{
  if(NULL == m_tcpSocket)
    {
      return;
    }
  m_blockSize = 0;
  QByteArray block;
  QDataStream out(&block, QIODevice::WriteOnly);
  out.setVersion(QDataStream::Qt_4_8);
  out << quint16(0) << int(LOGIN) << m_loginInfo;
  out.device()->seek(0);
  out << quint16(block.size() - sizeof(quint16));
  qDebug() << "request login";
  m_tcpSocket->write(block);
}


/*************************************************
Function Name： sendRequest()
Description: 发送请求
*************************************************/
//如果login和getQuestionAndAnswer中
//IMTcpSocket::isConnected为false，会暂时不处理
//而是会去连接TCPsocket
//当tcpsocket连接上之后会调用此方法，接着处理
void IMLoginCtrl::sendRequest()
{
  switch(m_kind)
    {
    case LOGIN:
      {

        requestLogin();
        break;
      }
    case GET_QUESTION_ANSWER:
      {
        requestGetQuestionAndAnswer();
      }
    }
}

/*************************************************
Function Name： requestGetQuestionAndAnswer()
Description: 发送获取密保问题以及答案的请求
*************************************************/
void IMLoginCtrl::requestGetQuestionAndAnswer()
{
  if(NULL == m_tcpSocket)
    {
      return;
    }
  m_blockSize = 0;
  QByteArray block;
  QDataStream out(&block, QIODevice::WriteOnly);
  out.setVersion(QDataStream::Qt_4_8);
  out << quint16(0) << int(GET_QUESTION_ANSWER) << m_id;
  out.device()->seek(0);
  out << quint16(block.size() - sizeof(quint16));
  m_tcpSocket->write(block);
}

/*************************************************
Function Name： readMessage()
Description: 读取信息
Input： NULL
Output： NULL
Changes： NULL
*************************************************/
void IMLoginCtrl::readMessage()
{
  QDataStream in(m_tcpSocket);
  in.setVersion(QDataStream::Qt_4_8);
  if(m_blockSize == 0)
    {
      if(m_tcpSocket->bytesAvailable() < (int)sizeof(quint16))
        {
          return;
        }
      in >> m_blockSize;
    }
  if(m_tcpSocket->bytesAvailable() < m_blockSize)
    {
      return;
    }
  int type;

  in >> type;

  switch (type) {
    case LOGIN_SUCCESS:
      {
        in >> m_myself;
        emit getLoginMessage(tr("登录成功"), true, &m_myself);
        break;
      }
    case LOGIN_FAIL:
      {
        emit getLoginMessage(tr("登录失败，帐号或者密码错误"), false);
        break;
      }
    case HAVE_LOGINED:
      {
        emit getLoginMessage(tr("登录失败，该用户已登录"), false);
        break;
      }
    case GET_QUESTION_ANSWER_FAIL:
      {
        QMessageBox::critical(NULL, tr("找回密码"), tr("失败，帐号不存在！"));
        break;
      }
    case GET_QUESTION_ANSWER_SUCCESS:
      {
        in >> m_tempStr;
        emit getQuestionAndAnswerSuccess(m_tempStr);

        while(1)
          {
            bool isOK = false;
            QString str = QString(tr("密保问题：%1\n请输入问题答案:"))
                .arg(m_tempStr.m_two);
            QString answer = QInputDialog::getText(NULL, "找回密码",
                                                   str,
                                                   QLineEdit::Normal,
                                                   NULL,
                                                   &isOK);
            if(!isOK)
              {
                break;
              }
            if(answer != m_tempStr.m_three)
              {
                str = QString(tr("回答错误!"));
                QMessageBox::critical(NULL, tr("找回密码"), str);
                continue;
              }
            else
              {
                str = QString(tr("回答正确!\n您的帐号是:%1\n您的密码是:%2"))
                    .arg(m_id)
                    .arg(IMEncryption::getInstace()
                         .getXorEncryptDecrypt(m_tempStr.m_one, 10));
                QMessageBox::information(NULL, tr("找回密码"), str);
              }
            break;
          }
        break;
      }
   default:
      break;
    }
  QByteArray data = m_tcpSocket->readAll();
  qDebug() << "leave in socketL " << data.size();
  m_blockSize = 0;
}
