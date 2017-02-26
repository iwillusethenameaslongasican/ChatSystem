#ifndef IMINFORMATIONWIDGET_H
#define IMINFORMATIONWIDGET_H

#include <QWidget>
/**********************************************
 * 个人信息窗口
 * 2016.11.14
 * ******************************************/

#include "../Model/IMContant.h"

class QPushButton;
class QLabel;
class QLineEdit;
class QCheckBox;
class QComboBox;
class QFrame;
class CustomLineEdit;
class IMMainWidget;

static const int MAX_NUM = 133; //总头像数

class IMInformationWidget : public QWidget
{
  Q_OBJECT
public:
  IMInformationWidget(IMMainWidget *mainWidget, QWidget *parent = 0);
  ~IMInformationWidget();

  void setReadOnly(bool isReadOnly, bool isEdit);
  void setUserInformation(const UserInformation &user);
  UserInformation getInformation();

protected:
  void closeEvent(QCloseEvent *event);

signals:

public slots:
  void onClickCancelButton();
  void onClickOKButton();
  void onClickPreButton();
  void onClickNextButton();

private:
  void initIMInformationWidget();

protected:
  QLabel *m_labelUserID; //用户帐号
  QLineEdit *m_leUserID;
  QLabel *m_labelNickname;  //用户昵称
  QLineEdit *m_leNickname;

  QFrame *m_frameHead;
  QLabel *m_labelHead;
  QPushButton *m_btnNext;
  QPushButton *m_btnPre;

  QLabel *m_labelConfirmPwd;  //确认密码
  QLineEdit *m_leConfirmPwd;

  QLabel *m_labelSex; //性别
  QComboBox *m_cbSex;

  QLabel *m_labelBirthday;  //生日
  CustomLineEdit *m_leBirthday;

  QLabel *m_labelName;  //真实姓名
  QLineEdit *m_leName;

  QLabel *m_labelPhone; //电话
  QLineEdit *m_lePhone;

  QLabel *m_labelAddress; //地址
  QLineEdit *m_leAddress;

  QPushButton *m_btnOK; //编辑/保存 按钮
  QPushButton *m_btnCancel; //取消/关闭 按钮

  int m_headNum;
  bool m_isReadOnly;
  bool m_isEdit;

  UserInformation m_userInf;
  IMMainWidget *m_mainWidget;


};


#endif // IMINFORMATIONWIDGET_H
