/*********************************************
File Name： IMLoginWidget.cpp
Author： hanyan
Date： 2016.11.15
Description： 客户端登录界面
Changes：
********************************************/

#include "IMLoginWidget.h"
#include <QPushButton>
#include <QLineEdit>
#include <QLabel>
#include <QCheckBox>
#include <QComboBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QDockWidget>
#include <QTime>
#include <QTimerEvent>
#include <QHostAddress>
#include <QInputDialog>

#include "IMMainWidget.h"
#include "../View/IMClickLabel.h"
#include "IMRegisterWidget.h"
#include "IMForgotWidget.h"
#include "../Control/IMClientFileCtrl.h"
#include "../Model/IMTcpSocket.h"
#include "../Control/IMLoginCtrl.h"
#include "../Model/IMEncryption.h"

static const QString AUTO_LOGIN_FILE_NAME = ":/resource/config/auto_login.im";

//MARK public: ............................................................
IMLoginWidget::IMLoginWidget(QWidget *parent)
  :QWidget(parent)
{
  initIMLoginWidget();
  linkSignalWithSlot();
  setWindowTitle(tr("登录OO"));
}


IMLoginWidget::~IMLoginWidget()
{
  if(m_loginCtrl != NULL)
    {
      delete m_loginCtrl;
      m_loginCtrl = NULL;
    }
  if(m_timerID != 0)
    {
      killTimer(m_timerID);
      m_timerID = 0;
    }
}

// mark: public slots:-----------------------------------------------------
/*************************************************
Function Name： showMainWidget()
Description: 显示主界面
Input： NULL
Output： NULL
Changes： NULL
*************************************************/
void IMLoginWidget::showMainWidget()
{
}

/*************************************************
Function Name： setLabelStatus()
Description: 设置状态标签
***********************************************/
void IMLoginWidget::setLabelStatus(const QString &msg, bool isLogin, const UserInformation *me)
{
    closeTimer();
    //qDebug() << "setLabelStatus";
    if(isLogin == true)
      {
        //qDebug() << "setLabelStatus";
        m_btnLogin->setEnabled(false);
        m_btnLogin->setText(tr("取消"));
        m_labelStatus->setText(tr("%1").arg(msg));

       if(true == m_cbKeepPwd->isChecked())
          {
            if(!IMClientFileCtrl::writeAutoLogin(
                 "D:/Code/QT/OOChatClient/resource/config/auto_login.im",
                 m_leUserID->text(),
                 m_leUserPwd->text(), m_cbAutoLogin->isChecked()))
              {
                qDebug() << "fail to write autoLogin file1.";
              }

          }
        else
          {
            if(!IMClientFileCtrl::remove(AUTO_LOGIN_FILE_NAME))
              {
                qDebug() << "fail to write autologin file2.";
              }
          }

        IMMainWidget *mainWidget = new IMMainWidget(*me);

        qDebug() << "show IMMainWidget";

        mainWidget->getFriendsInformation();

        mainWidget->show();

        close();
      }
    else
      {
        m_isLogin = true;
        m_btnLogin->setText(tr("登录"));
        m_labelStatus->setText(msg);
        m_leUserID->setReadOnly(false);
        m_leUserPwd->setReadOnly(false);
      }
}

/*************************************************
Function Name： getIsAutoLogin()
Description: 获取m_isAutoLogin
Input：NULL
Output： bool
Changes： NULL
*************************************************/
bool IMLoginWidget::getIsAutoLogin()
{
  return m_isAutoLogin;
}


/*************************************************
Function Name： onClickbtnLogin()
Description: 单击“登录”按钮
Input： NULL
Output： NULL
Changes： NULL
*************************************************/
void IMLoginWidget::onClickBtnLogin()
{
  setServer();

  m_leUserID->setReadOnly(true);
  m_leUserPwd->setReadOnly(true);
  qDebug() << "on clickLogin";
  if(true == m_isLogin)
    {
      m_isLogin = false;
      m_btnLogin->setText(tr("取消"));

      m_closeTimer = false;
     //间隔1000ms
      m_timerID = startTimer(1000);

      m_labelStatus->setText(tr("登陆中"));

      if(NULL == m_loginCtrl)
        {
          m_loginCtrl = new IMLoginCtrl(this);
          connect(m_loginCtrl, &IMLoginCtrl::getLoginMessage,
                  this, &IMLoginWidget::setLabelStatus);
        }

      switch(m_cbxStatus->currentIndex())
        {
        case 0:
          {
            m_status = ONLINE;
            break;
          }
        case 1:
          {
            //隐身
            m_status = INVISIBLE;
            break;
          }
        case 2:
          {
            m_status = BUSY;
            break;
          }
        case 3:
          {
            m_status = LEAVE;
            break;
          }
        case 4:
          {
            m_status = NOTDISTURB;
            break;
          }
        default:
          {
            m_status = ONLINE;
            break;
          }

        }
      qDebug() << "status: " << m_status;
      QString pwd = IMEncryption::getInstace()
          .getXorEncryptDecrypt(m_leUserPwd->text(), 10);
      //qDebug() << "pwd:" << pwd;
      m_loginCtrl->login(m_leUserID->text(), pwd, m_status);
    }
  else
    {
      m_isLogin = true;
      m_btnLogin->setText(tr("登录"));
      m_labelStatus->setText(tr("你好，请登录系统"));

      if(m_loginCtrl != NULL)
        {
          delete m_loginCtrl;
          m_loginCtrl = NULL;
        }

      closeTimer();
    }

}

// mark: protected:---------------------------------------------------------
/*************************************************
Function Name： timerEvent()
Description: 处理定时器事件
Input： QTimerEvent *event：定时器事件
Output： NULL
Changes： NULL
*************************************************/
void IMLoginWidget::timerEvent(QTimerEvent *event)
{
  if(event->timerId() == m_timerID)
    {
      ++ m_counter;
      switch(m_counter % 4)
        {
        case 0:
          {
            m_labelStatus->setText(tr("登陆中"));
            break;
          }
        case 1:
          {
            m_labelStatus->setText(tr("登录中.."));
            break;
          }
        case 2:
          {
            m_labelStatus->setText(tr("登录中......"));
            break;
          }
        case 3:
          {
            m_labelStatus->setText(tr("登陆中.........."));
            break;
          }
        }
    }
}

// mark: private slots:-----------------------------------------------------

/*************************************************
Function Name： onClickLabelRegister()
Description: 单击"注册帐号"标签
Input： NULL
Output： NULL
Changes： NULL
*************************************************/
void IMLoginWidget::onClickLabelRegister()
{
  setServer();
  IMRegisterWidget *registerWidget = new IMRegisterWidget;
  registerWidget->show();

}

/*************************************************
Function Name： onClickLabelForgotPwd()
Description: 单击“忘记密码"标签
Input： NULL
Output： NULL
Changes： NULL
*************************************************/
void IMLoginWidget::onClickLabelForgotPwd()
{
  setServer();

  bool isOK = false;
  QString userID = QInputDialog::getText(NULL, "找回密码",
                                    "请输入帐号ID",
                                    QLineEdit::Normal,
                                    NULL,
                                    &isOK);
  if(!isOK)
    {
      return;
    }

  m_loginCtrl->getQuestionAndAnswer(userID);
}

/*************************************************
Function Name： onClickCbAutoLogin
Description: 自动登录选择框改变
Input： state
Output： NULL
Changes： NULL
*************************************************/
void IMLoginWidget::onClickCbAutoLogin(int state)
{
  if(state == Qt::Checked)
    {
      m_cbKeepPwd->setChecked(true);
    }
  else
    {
      m_cbKeepPwd->setChecked(false);
    }
}

// mark private:------------------------------------------------------------
/*************************************************
Function Name： initIMLoginWidget()
Description: 初始化登录界面
Input： NULL
Output： NULL
Changes： NULL
*************************************************/
void IMLoginWidget::initIMLoginWidget()
{
  m_isLogin = true;
  m_loginCtrl = new IMLoginCtrl(this);
  connect(m_loginCtrl, &IMLoginCtrl::getLoginMessage,
          this, &IMLoginWidget::setLabelStatus);
  m_counter = 0;
  m_closeTimer = true;
  m_timerID = 0;

  QVBoxLayout *vLayoutMidTop = new QVBoxLayout;
  QVBoxLayout *vLayoutRightTop = new QVBoxLayout;
  QHBoxLayout *hLayoutTop = new QHBoxLayout;

  QHBoxLayout *hLayoutMid = new QHBoxLayout;
  QHBoxLayout *hLayoutBtm = new QHBoxLayout;
  QVBoxLayout *vLayoutMain = new QVBoxLayout;

  QLabel *labelHeadLine = new QLabel(tr("登录OO"));
  labelHeadLine->setAlignment(Qt::AlignCenter);
  //QPalette是用于调色的类
  QPalette pa;
  //windowText()前景色
  pa.setColor(QPalette::WindowText, Qt::black);
  labelHeadLine->setPalette(pa);
  QFont ft;
  ft.setPointSize(15);
  labelHeadLine->setFont(ft);

  m_leUserID = new QLineEdit;
  m_leUserID->setPlaceholderText(tr("帐号"));
  m_leUserPwd = new QLineEdit;
  m_leUserPwd->setPlaceholderText(tr("密码"));
  m_leUserPwd->setEchoMode(QLineEdit::Password);
  vLayoutMidTop->addWidget(m_leUserID);
  vLayoutMidTop->addWidget(m_leUserPwd);

  m_labelRegister = new IMClickLabel;

  m_labelRegister->setText(tr("注册帐号"));
  m_labelForgotPwd = new IMClickLabel;

  m_labelForgotPwd->setText(tr("忘记密码"));
  vLayoutRightTop->addWidget(m_labelRegister);
  vLayoutRightTop->addWidget(m_labelForgotPwd);

  hLayoutTop->addLayout(vLayoutMidTop);
  hLayoutTop->addLayout(vLayoutRightTop);
  hLayoutTop->setMargin(10);

  //mid
  m_cbKeepPwd = new QCheckBox(tr("记住密码"));
  m_cbAutoLogin = new QCheckBox(tr("自动登录"));
  hLayoutMid->addWidget(m_cbKeepPwd);
  hLayoutMid->addWidget(m_cbAutoLogin);
  //end mid


  //bottom
  m_moreButton = new QPushButton(tr("设置"));
  m_moreButton->setCheckable(true);
  m_moreButton->setAutoDefault(false);
  m_cbxStatus = new QComboBox();
  m_cbxStatus->addItem(tr("在线"));
  m_cbxStatus->addItem((tr("隐身")));
  m_cbxStatus->addItem(tr("忙碌"));
  m_cbxStatus->addItem(tr("离开"));
  m_cbxStatus->addItem(tr("请勿打扰"));
  m_btnLogin = new QPushButton(tr("登录"));
  m_btnLogin->setDefault(true);

  hLayoutBtm->addWidget(m_moreButton);
  hLayoutBtm->addWidget(m_cbxStatus);
  hLayoutBtm->addWidget(m_btnLogin);
  //end bottom

  //hide
  m_hostAddress = new QLineEdit;
  m_hostAddress->setPlaceholderText(tr("服务器IP地址"));
  m_hostPort = new QLineEdit;
  m_hostPort->setFixedWidth(60);
  m_hostPort->setPlaceholderText(tr("端口号"));
  m_extendedWidget = new QWidget;
  QHBoxLayout *hLayoutExtended = new QHBoxLayout;
  hLayoutExtended->addWidget(m_hostAddress);
  hLayoutExtended->addWidget(m_hostPort);
  m_extendedWidget->setLayout(hLayoutExtended);
  //end hide

  m_labelStatus = new QLabel;
  m_labelStatus->setText(tr("你好，请登录系统."));
  QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
  m_labelStatus->setSizePolicy(sizePolicy);

  vLayoutMain->setSizeConstraint(QLayout::SetFixedSize);
  vLayoutMain->addWidget(labelHeadLine);
  vLayoutMain->addLayout(hLayoutTop);
  vLayoutMain->addLayout(hLayoutMid);
  vLayoutMain->addLayout(hLayoutBtm);
  vLayoutMain->addWidget(m_extendedWidget);
  vLayoutMain->addWidget(m_labelStatus);
  vLayoutMain->setMargin(5);

  setLayout(vLayoutMain);

  m_extendedWidget->hide();

  //读取自动配置文件
  readAutoLoginFile(AUTO_LOGIN_FILE_NAME);
}

/*************************************************
Function Name： linkSignalWithSlot()
Description: 连接信号与槽
Input： NULL
Output： NULL
Changes： NULL
*************************************************/
void IMLoginWidget::linkSignalWithSlot()
{
  connect(m_btnLogin, &QPushButton::clicked,
          this, &IMLoginWidget::onClickBtnLogin);

  connect(m_moreButton, &QPushButton::clicked,
          m_extendedWidget, &QWidget::setVisible);

  connect(m_labelRegister, &IMClickLabel::clicked,
         this, &IMLoginWidget::onClickLabelRegister);


  connect(m_labelForgotPwd, &IMClickLabel::clicked,
         this, &IMLoginWidget::onClickLabelForgotPwd);

  connect(m_cbAutoLogin, &QCheckBox::stateChanged,
          this, &IMLoginWidget::onClickCbAutoLogin);
}

/*************************************************
Function Name： closeTimer()
Description: 关闭定时器
Input： NULL
Output： NULL
Changes： NULL
*************************************************/
void IMLoginWidget::closeTimer()
{
  if(m_timerID != 0)
    {
      m_closeTimer = true;
      killTimer(m_timerID);
      m_timerID = 0;
      m_counter = 0;
    }
}

/*************************************************
Function Name： setServer()
Description: 设置服务器
Input：  ip地址，端口号
Output： NULL
Changes： NULL
*************************************************/
void IMLoginWidget::setServer(const QString &ip, int port)
{
  QHostAddress serverAddress;
  int serverPort;
  if(m_hostAddress->text().isEmpty())
    {
      serverAddress = ip;
    }
  else
    {
      serverAddress = m_hostAddress->text();
    }
  if(m_hostPort->text().isEmpty())
    {
      serverPort = port;
    }
  else
    {
      serverPort = m_hostPort->text().toInt();
    }
  IMTcpSocket::s_hostAddress = serverAddress;
  IMTcpSocket::s_hostPort = serverPort;
}

/*************************************************
Function Name： 读取自动登录配置文件()
Description: 设置服务器
Input：  ip地址，端口号
Output： NULL
Changes： NULL
*************************************************/
bool IMLoginWidget::readAutoLoginFile(const QString &filename)
{
  QString tmpID;
  QString tmpPwd;
  bool isAuto = false;
 if(!IMClientFileCtrl::readAutoLogin(filename, tmpID, tmpPwd, isAuto))
    {
      m_isAutoLogin = false;
      qDebug() << "fail to read autoLogin file.";
      return false;

    }
  else
    {
      m_leUserID->setText(tmpID);
      m_leUserPwd->setText(tmpPwd);
      m_cbAutoLogin->setChecked(isAuto);
      m_isAutoLogin = isAuto;
      m_cbKeepPwd->setChecked(true);
    }
  return true;
}
