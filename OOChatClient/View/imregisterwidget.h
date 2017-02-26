/*********************************************
File Name： IMRegisterWidget.h
Author： hanyan
Date： 2016.11.04
Description： 客户端注册界面
Changes：
********************************************/

#ifndef IMREGISTERWIDGET_H
#define IMREGISTERWIDGET_H

#include <QWidget>
#include "../Model/IMContant.h"

class QPushButton;
class QLineEdit;
class QLabel;
class QCheckBox;
class QComboBox;
class IMRegisterCtrl;
class CustomLineEdit;


/*************************************************
Class Name： IMRegisterWidget
Description: 客户端注册帐号界面类
*************************************************/
class IMRegisterWidget : public QWidget
{
  Q_OBJECT
public:
  explicit IMRegisterWidget(QWidget *parent = 0);
  ~IMRegisterWidget();

signals:

public slots:
  //重置注册按钮
  void resetBtnRegister(bool );

protected:
  //窗口移动事件
  void moveEvent(QMoveEvent *);

private slots:
  //点击注册按钮
  void onClickBtnRegister();
  //点击取消按钮
  void onClickBtnCancel();

private:
  //初始化注册界面
  void initIMRegisterWidget();
  //连接信号与槽
  void linkSignalWithSlot();

private:
  QLabel *m_labelHead;

  //用户昵称
  QLabel *m_labelNickName;
  QLineEdit *m_leNickName;
  //呢称描述
  QLabel *m_labelDesNickName;

  //用户密码
  QLabel *m_labelPassword;
  QLineEdit *m_lePassword;
  QLabel *m_labelDesPassword;

  // 确认密码
  QLabel *m_labelConfirmPwd;
  QLineEdit *m_leConfirmPwd;
  QLabel *m_labelDesConfirmPwd;

  // 性别
  QLabel *m_labelSex;
  QComboBox *m_cbxSex;
  QLabel *m_labelDesSex;

  //生日
  QLabel *m_labelBirthday;
  //自定义的CustomLineEdit
  CustomLineEdit *m_leBirthday;
  QLabel *m_labelDesBirthday;

  //密保问题
  QLabel *m_labelQuestion;
  QLineEdit *m_leQuestion;
  QLabel *m_labelDesQuestion;

  //问题答案
  QLabel *m_labelAnswer;
  QLineEdit *m_leAnswer;
  QLabel *m_labelDesAnswer;

  //真实姓名
  QLabel *m_labelName;
  QLineEdit *m_leName;
  QLabel *m_labelDesName;

  //电话
  QLabel *m_labelPhone;
  QLineEdit *m_lePhone;
  QLabel *m_labelDesPhone;

  //地址
  QLabel *m_labelAddress;
  QLineEdit *m_leAddress;
  QLabel *m_labelDesAddress;

  // "注册" 按钮
  QPushButton *m_btnRegister;
  // "取消" 按钮
  QPushButton *m_btnCancel;

  IMRegisterCtrl * m_regCtrl;

  UserInformation m_userInf;
};

#endif // IMREGISTERWIDGET_H
