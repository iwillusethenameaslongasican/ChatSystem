#ifndef IMLINKFRIENDBUTTON_H
#define IMLINKFRIENDBUTTON_H

#include "IMLinkButton.h"
#include <QPushButton>
#include "../Model/IMContant.h"

class QLabel;
class IMFriendButton;

class IMLinkFriendButton: public IMLinkButton
{
    Q_OBJECT
public:
    IMLinkFriendButton(IMFriendButton *btn, QWidget *parent = 0);
    ~IMLinkFriendButton();

    //设置BUTTON显示信息
    void setButtonStatus();

public slots:
    //左键 双击
    void onDoubleClickButton();
    //忽略信息
    void ignoreMessage();

private:
    IMFriendButton *m_friendBtn;
    FriendInformation m_info;

};

#endif // IMLINKFRIENDBUTTON_H
