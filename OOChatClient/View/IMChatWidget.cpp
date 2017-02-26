 /*********************************************
File Name： IMChatWidget.h
Author： hanyan
Date： 2016.11.13
Description： 用于控制聊天界面的类
Changes：
********************************************/

#include "IMChatWidget.h"
#include <QLabel>
#include <QDebug>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QToolButton>
#include <QPixmap>
#include <QPushButton>
#include <QFrame>
#include <QFontComboBox>
#include <QComboBox>
#include <QTextBrowser>
#include <QDockWidget>
#include <QMessageBox>
#include <QCloseEvent>
#include <QColorDialog>
#include <QTextCharFormat>
#include <QFileDialog>
#include "../Control/IMClientFileCtrl.h"
#include "../Model/IMTcpSocket.h"
#include "IMInputTextEdit.h"
#include "IMClicklabel.h"
#include "IMFileSenderWidget.h"
#include "IMFileReceiverWidget.h"


//mark public: -------------------------------------------------------
IMChatWidget::IMChatWidget(const ChatInformation &chatInf, QWidget *parent)
    :QWidget(parent)
{
    m_chatInfo = chatInf;
    m_fileSender = new IMFileSenderWidget();

    initIMChatWidget();
    setLabelStatus();
    linkSignalWithSlot();
    setWindowTitle(tr("好友聊天窗口"));
}

IMChatWidget::~IMChatWidget()
{
  if(m_fileSender != NULL)
    {
      m_fileSender->close();
      m_fileSender->deleteLater();
      m_fileSender = NULL;
    }
}

/******************************************
 * function name: appendMessageShow
 * Description: 添加好友发来的信息
*****************************************/
void IMChatWidget::appendMessageShow(const TalkMessage &msg)
{
  QString name;
  if(m_chatInfo.m_friendRemark.isEmpty())
    {
      name = m_chatInfo.m_friendNickname;
    }
  else
    {
      name = m_chatInfo.m_friendRemark;
    }

  //将（收到的）信息显示在输出栏
  QString temp = QString("<font size=\"FONT_SIZE\" COLOR=BLUE>%1   %2: </font>%3")
      .arg(name, msg.m_dateTime.toString("yyyy-MM-dd hh:mm:ss"), msg.m_text);
  m_tbMessageShow->append(temp);
}


/*******************************************
 *function name: pendingFile
 *Description: 有接收文件请求需要处理
 * *****************************************/
void IMChatWidget::pendingFile(const TalkMessage &msg)
{
  int btn = QMessageBox::information(this, tr("接收文件"),
                                     tr("来自%1(%2)的文件：\n%3\n是否接收？")
                                     .arg(msg.m_senderID, msg.m_senderIP,
                                     msg.m_text), QMessageBox::Yes, QMessageBox::No);
  if(btn == QMessageBox::Yes)
    {
      QString name = QFileDialog::getSaveFileName(0, tr("保存文件"), msg.m_text);
      if(!name.isEmpty())
        {
          IMFileReceiverWidget *receiver = new IMFileReceiverWidget();
          receiver->setFileName(name);
          //qDebug() << "pengding file1";
          receiver->connectToHost(QHostAddress(msg.m_senderIP));
          qDebug() << "m_senderIP: " << QHostAddress(msg.m_senderIP);
          //qDebug() << "pengding file1";
          receiver->move(500, 200);
          receiver->show();
        }
    }
  else
    {
      m_msg.m_type = REFUSE_FILE;
      m_msg.m_receiverID = msg.m_senderID;
      m_msg.m_receiverIP = msg.m_senderIP;
      emit sendMessagesFromChat(m_msg);
    }
}


/*************************************************
 * function name: refuseSendFile
 * Description: 拒绝文件
 * ************************************************/
void IMChatWidget::refusedSendFile()
{
  if(m_fileSender != NULL)
    {
      m_fileSender->showRefused();
    }
}

/***********************************************
 * function name: pendingVideo
 * Description: 有接收视频请求需要处理
 * ********************************************/
void IMChatWidget::pendingVideo(const TalkMessage &msg)
{
  int btn = QMessageBox::information(NULL, tr("视频请求"),
                                     tr("来自%1(%2)的视频，是否接受？")
                                     .arg(m_chatInfo.m_friendNickname)
                                     .arg(msg.m_senderID),
                                     QMessageBox::Yes, QMessageBox::No);
  if(btn == QMessageBox::Yes)
    {
      m_toolBtnVideo->setEnabled(false);
      m_msg.m_type = AGREE_VOICE;
      m_msg.m_receiverID = msg.m_senderID;
      m_msg.m_receiverIP = msg.m_senderIP;
      m_msg.m_text = "port";

      qDebug() << "创建视频窗口";
      //showCamera(msg);
      //发送视频 接收视频

    }
  else
    {
      m_msg.m_type = REFUSE_VIDEO;
      m_msg.m_receiverID = msg.m_senderID;
      m_msg.m_receiverIP = msg.m_senderIP;
    }
  emit sendMessagesFromChat(m_msg);

}

/***************************************************
 * function name: showVideoWidget
 * Description: 显示视频窗口
 * **************************************************/
void IMChatWidget::showVideoWidget(const TalkMessage &msg)
{
  qDebug() << "show video widget 还没做，以后补上";
}

/*****************************************************
 * function name: showCamera
 * Description: 显示采集视频窗口
 * ***************************************************/
void IMChatWidget::showCamera(const TalkMessage &msg)
{
  //还没做，以后补上
  qDebug() << "showCamera() 还没做，以后补上";
}

/*****************************************************
 * function name: closeVideoWidget
 * Description: 关闭视频窗口
 * ***************************************************/
void IMChatWidget::closeVideoWidget()
{
  qDebug() << "closeVideoWidget 还没做，以后补上";
}

/***************************************************
 * function name: setLabelStatus
 * Description: 设置聊天窗口显示信息
 * *************************************************/
void IMChatWidget::setLabelStatus()
{
  QString str = QString(":/resource/image/head/%1.bmp")
      .arg(QString::number(m_chatInfo.m_headPortrait));

  QString name;
  if(m_chatInfo.m_friendRemark.isEmpty())
    {
      name = m_chatInfo.m_friendNickname;
    }
  else
    {
      name = m_chatInfo.m_friendRemark;
    }

  QPixmap pixmap;
  QIcon icon(str);
  pixmap = icon.pixmap(QSize(HEAD_BIG_SIZE, HEAD_BIG_SIZE));
  m_labelHead->setPixmap(pixmap);
  m_labelHead->setText(name);
  m_labelMark->setText(m_chatInfo.m_friendMark);

}

/**********************************************************
 * function name: setChatInformation
 * Description: 设置对方信息
 * ****************************************************/
void IMChatWidget::setChatInformation(const ChatInformation &chatInfo)
{
  if(chatInfo.m_friendID != m_chatInfo.m_friendID)
    {
      return;
    }
  m_chatInfo.m_headPortrait = chatInfo.m_headPortrait;
  m_chatInfo.m_friendNickname = chatInfo.m_friendNickname;
  m_chatInfo.m_friendRemark = chatInfo.m_friendRemark;
  m_chatInfo.m_friendMark = chatInfo.m_friendMark;
  m_chatInfo.m_friendIP = chatInfo.m_friendIP;
  m_chatInfo.m_friendStatus = chatInfo.m_friendStatus;
}

/*************************************************************
 * function name: setFriendIP
 * Description: 设置对方ip
 * **********************************************************/
void IMChatWidget::setFriendIP(const QString &ip)
{
  m_chatInfo.m_friendIP = ip;
  m_labelFriendIP->setText(QString(tr("好友IP： %1")).arg(ip));
}

//mark protected:********************************************
/************************************************************
 * function name: closeEvent
 * Description: 关闭重载事件
 * *********************************************************/
void IMChatWidget::closeEvent(QCloseEvent *event)
{
  if(m_fileSender != NULL)
    {
      if(!m_fileSender->isStopped())
        {
          QString temp = tr("文件还在发送中，您确定想要关闭聊天窗口?");
          QMessageBox::StandardButton btn = QMessageBox::question(
                NULL, tr("关闭窗口"), temp,
                QMessageBox::Yes | QMessageBox::No,
                QMessageBox::No);
          if(btn == QMessageBox::No)
            {
              event->ignore();
              return;
            }
        }
    }
  //向主界面发送退出信号
  emit roomQuitSignal();
  saveHistoryMessage();

  event->accept();
}

//mark: private -----------------------------------------------

/**************************************************************
 * function name: initIMChatWidget
 * Description: 初始化主界面
 * *********************************************************/
void IMChatWidget::initIMChatWidget()
{
  setWidgetToolBtn();

  /**********好友信息以及交互信息栏************************/
  QHBoxLayout *hLayoutFriend = new QHBoxLayout;
  QVBoxLayout *vLayoutFriend = new QVBoxLayout;
  QVBoxLayout *vLayoutFriendInfo = new QVBoxLayout;
  QHBoxLayout *hLayoutFriendInter = new QHBoxLayout;

  m_labelHead = new IMClickLabel;
  m_labelName = new QLabel;
  m_labelMark = new QLabel;

  vLayoutFriendInfo->addWidget(m_labelName);
  vLayoutFriendInfo->addWidget(m_labelMark);

  hLayoutFriendInter->addWidget(m_toolBtnVideo);
  hLayoutFriendInter->addWidget(m_toolBtnVoice);
  hLayoutFriendInter->addWidget(m_toolBtnFile);
  hLayoutFriendInter->addWidget(m_toolBtnSpace);
  hLayoutFriendInter->addStretch();

  hLayoutFriend->addWidget(m_labelHead);
  hLayoutFriend->addLayout(vLayoutFriendInfo);
  hLayoutFriend->addStretch();

  vLayoutFriend->addLayout(hLayoutFriend);
  vLayoutFriend->addLayout(hLayoutFriendInter);

  /*******输入编辑框工具栏**************************/
  QHBoxLayout *hLayoutInputTool = new QHBoxLayout;

  hLayoutInputTool->addWidget(m_cbFont);
  hLayoutInputTool->addWidget(m_cbSize);
  hLayoutInputTool->addStretch();

  hLayoutInputTool->addWidget(m_toolBtnBold);
  hLayoutInputTool->addWidget(m_toolBtnItalic);
  hLayoutInputTool->addWidget(m_toolBtnUnderline);
  hLayoutInputTool->addWidget(m_toolBtnColor);
  hLayoutInputTool->addWidget(m_toolBtnClear);
  hLayoutInputTool->addWidget(m_toolBtnHistory);

  /****************底部ip显示以及按钮***************************/
  QHBoxLayout *hLayoutBtm = new QHBoxLayout;
  m_labelFriendIP = new QLabel(tr("IP"));
  m_btnClose = new QPushButton(tr("关闭"));
  m_btnSend = new QPushButton(tr("发送"));
  m_btnSend->setDefault(true);

  hLayoutBtm->addWidget(m_labelFriendIP);
  hLayoutBtm->addStretch();
  hLayoutBtm->addWidget(m_btnClose);
  hLayoutBtm->addWidget(m_btnSend);

  /****************左侧布局************************************/
  m_tbMessageShow = new QTextBrowser;
  m_tbMessageShow->setMinimumSize(400, 280);
  m_textInput = new IMInputTextEdit;
  m_textInput->setMinimumWidth(100);
  m_textInput->setMaximumHeight(170);
  QVBoxLayout *vLayoutLeft = new QVBoxLayout;
  vLayoutLeft->addWidget(m_tbMessageShow);
  vLayoutLeft->addLayout(hLayoutInputTool);
  vLayoutLeft->addWidget(m_textInput);
  vLayoutLeft->addLayout(hLayoutBtm);

  /*****************中间整体布局**********************************/
  QHBoxLayout *hLayoutMid = new QHBoxLayout;

  m_dockHistory = new QWidget;
  m_dockHistory->hide();
  m_dockHistory->setFixedWidth(250);
  //QTextBrowser是只读的QEditText 而且QTextBrowser可以添加链接文本
  m_tbHistoryShow = new QTextBrowser(m_dockHistory);
  QLabel *labelHistory = new QLabel(tr("消息记录"));
  QVBoxLayout *vLayoutHistory = new QVBoxLayout;
  vLayoutHistory->addWidget(labelHistory);
  vLayoutHistory->addWidget(m_tbHistoryShow);
  vLayoutHistory->setContentsMargins(0, 0, 0, 0);
  m_dockHistory->setLayout(vLayoutHistory);

  hLayoutMid->addLayout(vLayoutLeft);
  hLayoutMid->addWidget(m_dockHistory);

  /******************整体布局***************************************/
  QVBoxLayout *vLayoutMain = new QVBoxLayout;

  vLayoutMain->addLayout(vLayoutFriend);
  vLayoutMain->addLayout(hLayoutMid);
  setLayout(vLayoutMain);

  m_textInput->setFocus();

}

/**************************************************
 * function name: linkSignalWithSlot
 * Description: 连接信号与槽
 * *********************************************/
void IMChatWidget::linkSignalWithSlot()
{
  connect(m_textInput, &IMInputTextEdit::sendMessage,
          this, &IMChatWidget::onClickBtnSend);
  connect(m_btnSend, &QPushButton::clicked,
          this, &IMChatWidget::onClickBtnSend);
  connect(m_btnClose, &QPushButton::clicked,
          this, &IMChatWidget::onClickBtnClose);

  connect(m_toolBtnVideo, &QToolButton::clicked,
          this, &IMChatWidget::onClickBtnVideo);
  connect(m_toolBtnVoice, &QToolButton::clicked,
          this, &IMChatWidget::onClickBtnVoice);
  connect(m_toolBtnFile, &QToolButton::clicked,
          this, &IMChatWidget::onClickBtnFile);
  connect(m_toolBtnSpace, &QToolButton::clicked,
          this, &IMChatWidget::onClickBtnSpace);

  connect(m_cbFont, &QFontComboBox::currentFontChanged,
          this, &IMChatWidget::setInputTextFont);
  connect(m_cbSize, SIGNAL(currentIndexChanged(QString)),
          this, SLOT(setInputTextSize(QString)));
  connect(m_toolBtnBold, &QToolButton::clicked,
          this, &IMChatWidget::onClickBtnBlod);
  connect(m_toolBtnItalic, &QToolButton::clicked,
          this, &IMChatWidget::onClickBtnItalic);
  connect(m_toolBtnUnderline, &QToolButton::clicked,
          this, &IMChatWidget::onClickBtnUnderline);

  connect(m_toolBtnColor, &QToolButton::clicked,
          this, &IMChatWidget::onClickBtnColor);
  connect(m_toolBtnClear, &QToolButton::clicked,
          this, &IMChatWidget::onClickBtnClear);

  connect(m_toolBtnHistory, &QToolButton::clicked,
          this, &IMChatWidget::onClickBtnHistory);
  //currentCharFormatChanged是个信号，cursor移动而且字符格式改变时会被调用
  connect(m_textInput, &IMInputTextEdit::currentCharFormatChanged,
          this, &IMChatWidget::currentFormatChanged);
}

/***************************************************
 * function name: setWidgetToolBtn
 * Description: 设置面板图标
 * *************************************************/
void IMChatWidget::setWidgetToolBtn()
{
  m_toolBtnVideo = new QToolButton;
  m_toolBtnVideo->setIcon(QIcon(":/resource/image/camera.png"));
  m_toolBtnVideo->setAutoRaise(true);
  m_toolBtnVideo->setToolTip(tr("视频"));
  m_toolBtnVideo->setStatusTip(tr("打开视频"));
  m_toolBtnVideo->setIconSize(QSize(CHAT_WIDGET_TOOL_SIZE,CHAT_WIDGET_TOOL_SIZE));

  m_toolBtnVoice = new QToolButton;
  m_toolBtnVoice->setIcon(QIcon(":/resource/image/microphone.png"));
  m_toolBtnVoice->setAutoRaise(true);
  m_toolBtnVoice->setToolTip(tr("语音"));
  m_toolBtnVoice->setStatusTip(tr("打开语音"));
  m_toolBtnVoice->setIconSize(QSize(CHAT_WIDGET_TOOL_SIZE,CHAT_WIDGET_TOOL_SIZE));

  m_toolBtnFile = new QToolButton;
  m_toolBtnFile->setIcon(QIcon(":/resource/image/send_file.png"));
  m_toolBtnFile->setAutoRaise(true);
  m_toolBtnFile->setToolTip(tr("发送文件"));
  m_toolBtnFile->setStatusTip(tr("发送文件"));
  m_toolBtnFile->setIconSize(QSize(CHAT_WIDGET_TOOL_SIZE,CHAT_WIDGET_TOOL_SIZE));

  m_toolBtnSpace = new QToolButton;
  m_toolBtnSpace->setIcon(QIcon(":/resource/image/space.png"));
  m_toolBtnSpace->setAutoRaise(true);
  m_toolBtnSpace->setToolTip(tr("空间"));
  m_toolBtnSpace->setStatusTip(tr("打开空间"));
  m_toolBtnSpace->setIconSize(QSize(CHAT_WIDGET_TOOL_SIZE,CHAT_WIDGET_TOOL_SIZE));
//---------------------------------------------------------------------------

  m_cbFont = new QFontComboBox;
  m_cbFont->setToolTip(tr("字体"));
  m_cbFont->setStatusTip(tr("设置字体"));
  m_cbSize = new QComboBox;
  m_cbSize->setToolTip(tr("大小"));
  m_cbSize->setStatusTip(tr("设置字体大小"));
  for (int i=9;  i<=22; ++i)
  {
      m_cbSize->addItem(QString::number(i));
  }
  m_cbSize->setCurrentIndex(0);

  m_cbSize->setMaxVisibleItems(10);
  m_cbFont->setMaxVisibleItems(10);

  m_toolBtnBold = new QToolButton;
  m_toolBtnBold->setCheckable(true);
  m_toolBtnBold->setIcon(QIcon(":/resource/image/font_bold.png"));
  m_toolBtnBold->setAutoRaise(true);
  m_toolBtnBold->setIconSize(QSize(CHAT_WIDGET_TOOL_SMALL_SIZE,CHAT_WIDGET_TOOL_SMALL_SIZE));
  m_toolBtnBold->setToolTip(tr("加粗"));
  m_toolBtnBold->setStatusTip(tr("是否加粗"));

  m_toolBtnItalic = new QToolButton;
  m_toolBtnItalic->setCheckable(true);
  m_toolBtnItalic->setIcon(QIcon(":/resource/image/font_italic.png"));
  m_toolBtnItalic->setAutoRaise(true);
  m_toolBtnItalic->setIconSize(QSize(CHAT_WIDGET_TOOL_SMALL_SIZE,CHAT_WIDGET_TOOL_SMALL_SIZE));
  m_toolBtnItalic->setToolTip(tr("倾斜"));
  m_toolBtnItalic->setStatusTip(tr("是否倾斜"));

  m_toolBtnUnderline = new QToolButton;
  m_toolBtnUnderline->setCheckable(true);
  m_toolBtnUnderline->setIcon(QIcon(":/resource/image/font_underline.png"));
  m_toolBtnUnderline->setAutoRaise(true);
  m_toolBtnUnderline->setIconSize(QSize(CHAT_WIDGET_TOOL_SMALL_SIZE,CHAT_WIDGET_TOOL_SMALL_SIZE));
  m_toolBtnUnderline->setToolTip(tr("下划线"));
  m_toolBtnUnderline->setStatusTip(tr("是否添加下划线"));

  m_toolBtnColor = new QToolButton;
  m_toolBtnColor->setIcon(QIcon(":/resource/image/colour.png"));
  m_toolBtnColor->setAutoRaise(true);
  m_toolBtnColor->setIconSize(QSize(CHAT_WIDGET_TOOL_SMALL_SIZE,CHAT_WIDGET_TOOL_SMALL_SIZE));
  m_toolBtnColor->setToolTip(tr("颜色"));
  m_toolBtnColor->setStatusTip(tr("设置颜色"));

  m_toolBtnClear = new QToolButton;
  m_toolBtnClear->setIcon(QIcon(":/resource/image/clear.png"));
  m_toolBtnClear->setAutoRaise(true);
  m_toolBtnClear->setIconSize(QSize(CHAT_WIDGET_TOOL_SMALL_SIZE,CHAT_WIDGET_TOOL_SMALL_SIZE));
  m_toolBtnClear->setToolTip(tr("清空"));
  m_toolBtnClear->setStatusTip(tr("清空消息显示"));

  m_toolBtnHistory = new QToolButton;
  m_toolBtnHistory->setIcon(QIcon(":/resource/image/message.png"));
  m_toolBtnHistory->setAutoRaise(true);
  m_toolBtnHistory->setCheckable(true);
  m_toolBtnHistory->setIconSize(QSize(CHAT_WIDGET_TOOL_SMALL_SIZE,CHAT_WIDGET_TOOL_SMALL_SIZE));
  m_toolBtnHistory->setToolTip(tr("消息记录"));
  m_toolBtnHistory->setStatusTip(tr("打开消息记录"));
}

/*******************************************************
 * function name: saveHistoryMessage
 * Description: 保存历史记录
 * ****************************************************/
void IMChatWidget::saveHistoryMessage()
{
  QString filePath = LOCAL_HISTORY_MESSAGE_PATH;
  filePath.append(m_chatInfo.m_myID);
  QString fileName = QString("%1_%2_friend.imres")
      .arg(m_chatInfo.m_myID, m_chatInfo.m_friendID);

  IMClientFileCtrl::saveTalkHistory(filePath, fileName, m_tbMessageShow);
}

/*******************************************************
 * function naem: readHistoryMessage
 * Description: 读取历史信息
 * ****************************************************/
void IMChatWidget::readHistoryMessage()
{
  QString filePath = LOCAL_HISTORY_MESSAGE_PATH;
  filePath.append(m_chatInfo.m_myID);
  QString fileName = QString("%1_%2_friend.imres")
      .arg(m_chatInfo.m_myID, m_chatInfo.m_friendID);

  IMClientFileCtrl::readTalkHistory(filePath, fileName, m_tbHistoryShow);
}

// private slots:--------------------------------------------------------------

/*******************************************************
 * function name: onClickBtnVideo
 * Description: 点击“视频”按钮
 * ******************************************************/
void IMChatWidget::onClickBtnVideo()
{
  //暂时没做
}

/*********************************************************
 * function name: onClickBtnVoice
 * Description: 点击“语音”按钮
 * ******************************************************/
void IMChatWidget::onClickBtnVoice()
{
  //暂时没做
}

/**********************************************************
 * function name: onClickBtnFile
 * Description: 发送文件
 * ********************************************************/
void IMChatWidget::onClickBtnFile()
{
  if(m_fileSender->isVisible())
    {
      return;
    }
  if(OFFLINE == m_chatInfo.m_friendStatus)
    {
      QMessageBox::critical(NULL, tr("发送文件"), tr("好友不在线，暂时无法发送文件"));
      return;
    }
  QString fileName = QFileDialog::getOpenFileName(this);
  if(fileName.isEmpty())
    {
      QMessageBox::information(NULL, tr("警告"), tr("请选择文件"));
      return;
    }
  m_fileSender->initSender();
  if(!m_fileSender->startSending(fileName))
    {
      qDebug() << "!m_fileSender->startSending(fileName)";
      return;
    }
  m_fileSender->move(500, 200);
  m_fileSender->show();

  m_msg.m_type = REQUEST_FILE;
  m_msg.m_receiverID = m_chatInfo.m_friendID;
  m_msg.m_text = fileName;
  qDebug() << "emit sendMessagesFromChat";
  emit sendMessagesFromChat(m_msg);

}

/*************************************************
Function Name： onClickBtnSpace()
Description:  空间
*************************************************/
void IMChatWidget::onClickBtnSpace()
{
}

/*************************************************
Function Name： setInputTextFont()
Description:  字体样式
*************************************************/
void IMChatWidget::setInputTextFont(const QFont &font)
{
  m_textInput->setCurrentFont(font);
  m_textInput->setFocus();
}

/*************************************************
Function Name： setInputTextSize()
Description:  字体大小
*************************************************/
void IMChatWidget::setInputTextSize(const QString &size)
{
  m_textInput->setFontPointSize(size.toDouble());
  m_textInput->setFocus();
}


/*************************************************
Function Name： onClickBtnBold()
Description:  加粗
*************************************************/
void IMChatWidget::onClickBtnBlod(bool checked)
{
  if(checked)
    {
      m_textInput->setFontWeight(QFont::Bold);
    }
  else
    {
      m_textInput->setFontWeight(QFont::Normal);
    }
  m_textInput->setFocus();
}


/*************************************************
Function Name： onClickBtnItalic()
Description:  斜体
*************************************************/
void IMChatWidget::onClickBtnItalic(bool checked)
{
    m_textInput->setFontItalic(checked);
    m_textInput->setFocus();
}


/*************************************************
Function Name： onClickBtnUnderline()
Description:  下划线
*************************************************/
void IMChatWidget::onClickBtnUnderline(bool checked)
{
    m_textInput->setFontUnderline(checked);
    m_textInput->setFocus();
}

/*************************************************
Function Name： onClickBtnColor()
Description:  颜色
*************************************************/
void IMChatWidget::onClickBtnColor()
{
  QColor color = QColorDialog::getColor(color, this);
  if(color.isValid())
    {
      m_textInput->setTextColor(color);
      m_textInput->setFocus();
    }
}


/*************************************************
Function Name： onClickBtnClear()
Description:  清空
*************************************************/
void IMChatWidget::onClickBtnClear()
{
  saveHistoryMessage();
  m_tbMessageShow->clear();
}


/*************************************************
Function Name： onClickBtnHistory()
Description:  聊天历史记录
*************************************************/
void IMChatWidget::onClickBtnHistory(bool checked)
{
  m_dockHistory->setHidden(!checked);
  if(checked)
    {
      readHistoryMessage();
    }
}


/*************************************************
Function Name： currentFormatChanged()
Description:  部件状态
*************************************************/
void IMChatWidget::currentFormatChanged(const QTextCharFormat &format)
{
  //当编辑器的字体格式改变时，我们让部件状态也随之改变
  m_cbFont->setCurrentFont(format.font());

  if(format.fontPointSize() < 9)  //如果字体大小出错，因为我们最小的字体为9
    {
      m_cbSize->setCurrentIndex(3); //即显示12
    }
  else
    {
      m_cbSize->setCurrentIndex(
            m_cbSize->findText(QString::number(format.fontPointSize())));
    }

  m_toolBtnBold->setChecked(format.font().bold());
  m_toolBtnItalic->setChecked(format.font().italic());
  m_toolBtnUnderline->setChecked(format.font().underline());

}


/*************************************************
Function Name： onClickBtnClocse()
Description:  点击“关闭”按钮
*************************************************/
void IMChatWidget::onClickBtnClose()
{
    this->close();
}


/*************************************************
Function Name： onClickBtnSend()
Description:  点击“发送”按钮
*************************************************/
void IMChatWidget::onClickBtnSend()
{
  if(m_textInput->toPlainText().isEmpty())
    {
      return;
    }
  //获取信息输入框的信息，并且更新信息输入框
  QString sendString = m_textInput->toHtml();
  m_textInput->clear();

  QDateTime dateTime = QDateTime::currentDateTime();
  QString temp = QString("<font size=\"FONT_SIEZE\" color=green>%1    %2: </font>%3")
      .arg("我").arg(dateTime.toString("yyyy-MM-dd hh:mm:ss")).arg(sendString);
  m_tbMessageShow->append(temp);

  m_msg.m_text = sendString;
  m_msg.m_type = TALK_MESSAGE;
  m_msg.m_receiverID = m_chatInfo.m_friendID;
  m_msg.m_dateTime = dateTime;

  emit sendMessagesFromChat(m_msg);
}
