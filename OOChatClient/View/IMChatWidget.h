/*********************************************
File Name： IMChatWidget.h
Author： hanyan
Date： 2016.11.12
Description： 用于控制聊天界面的类
Changes：
********************************************/

#ifndef IMCHATWIDGET_H
#define IMCHATWIDGET_H

#include <QWidget>
#include "../Model/IMContant.h"

class QComboBox;
class QFontComboBox;
class IMInputTextEdit;
class QDockWidget;
class QPixmap;
class QFrame;
class QLabel;
class QPushButton;
class QToolButton;
class QTextBrowser;
class IMClickLabel;
class QTextCharFormat;
class IMFileSenderWidget;

class IMChatWidget: public QWidget
{
  Q_OBJECT
public:
  IMChatWidget(const ChatInformation &chatInf, QWidget *parent = 0);

  ~IMChatWidget();
  //添加好友发来的信息
  void appendMessageShow(const TalkMessage &msg);
  //有接收文件请求需要处理
  void pendingFile(const TalkMessage &msg);
  //对方拒绝文件发送
  void refusedSendFile();

  //有接收视频请求需要处理
  void pendingVideo(const TalkMessage &msg);
  //显示接收视频窗口
  void showVideoWidget(const TalkMessage &msg);
  //显示采集视频窗口
  void showCamera(const TalkMessage &msg);
  //开始视频
  void startCapture();
  //关闭视频窗口
  void closeVideoWidget();

  //设置聊天窗口信息
  void setLabelStatus();
  //设置对方信息
  void setChatInformation(const ChatInformation &chatInfo);
  //设置对方ip
  void setFriendIP(const QString &ip);

signals:
  void sendMessagesFromChat(TalkMessage &);
  //退出信号
  void roomQuitSignal();
public slots:

protected:
  //重载关闭事件
  void closeEvent(QCloseEvent *event);

private slots:

  //视频
  void onClickBtnVideo();

  //语音
  void onClickBtnVoice();

  //发送文件
  void onClickBtnFile();
  //空间
  void onClickBtnSpace();

  //字体样式
  void setInputTextFont(const QFont &font);
  //字体大小
  void setInputTextSize(const QString &size);
  //加粗
  void onClickBtnBlod(bool checked);
  //斜体
  void onClickBtnItalic(bool checked);
  //下划线
  void onClickBtnUnderline(bool checked);
  //颜色
  void onClickBtnColor();
  //清空
  void onClickBtnClear();
  //聊天历史记录
  void onClickBtnHistory(bool checked);
  //当编辑器的字体格式改变时，我们让部件状态也随之改变
  void currentFormatChanged(const QTextCharFormat &format);
  //关闭
  void onClickBtnClose();
  //发送聊天信息
  void onClickBtnSend();

private:
  void initIMChatWidget();
  void linkSignalWithSlot();
  void setWidgetToolBtn();
  void saveHistoryMessage();
  void readHistoryMessage();

private:
  IMClickLabel *m_labelHead;
  QLabel *m_labelName;  //好友的昵称 以及 备注名
  QLabel *m_labelMark;  //好友的签名
  QToolButton *m_toolBtnVideo;  //视频按钮
  QToolButton *m_toolBtnVoice;  //语音按钮
  QToolButton *m_toolBtnFile;   //文件按钮
  QToolButton *m_toolBtnSpace;  //空间按钮
  QTextBrowser *m_tbMessageShow; //显示聊天信息

  QFontComboBox *m_cbFont;      //字体样式选择框
  QComboBox *m_cbSize;          //字体大小选择框
  QToolButton *m_toolBtnBold;   //加粗
  QToolButton *m_toolBtnItalic; //斜体
  QToolButton *m_toolBtnUnderline;//下划线
  QToolButton *m_toolBtnColor;  //颜色
  QToolButton *m_toolBtnClear;  //清空
  QToolButton *m_toolBtnHistory;//历史记录

  IMInputTextEdit *m_textInput; //信息输入框
  QLabel *m_labelFriendIP;      //好友ip
  QPushButton *m_btnClose;      //关闭按钮
  QPushButton *m_btnSend;       //发送按钮

  QWidget *m_dockHistory;
  QTextBrowser *m_tbHistoryShow; //显示聊天历史记录

  ChatInformation m_chatInfo;
  TalkMessage m_msg;

  IMFileSenderWidget *m_fileSender;
};

#endif // IMCHATWIDGET_H
