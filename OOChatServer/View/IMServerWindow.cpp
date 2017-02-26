#include "IMServerWindow.h"
#include "../Control/IMServerMessageCtrl.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QListView>

IMServerWidget::IMServerWidget(QWidget *parent)
  : QWidget(parent)
{
  m_serverCtrl = new IMServerMessageCtrl(this);
  m_serverCtrl->runServer();

  QHBoxLayout *viewLayout = new QHBoxLayout;
  viewLayout->addWidget(m_serverCtrl->getIDListView());
  viewLayout->addWidget(m_serverCtrl->getAddressListView());

  QVBoxLayout *layout = new QVBoxLayout;
  QLabel *labelTitle = new QLabel(tr("连接服务器用户帐号列表: "));
  layout->addWidget(labelTitle);
  layout->addLayout(viewLayout);

  setLayout(layout);
}

IMServerWidget::~IMServerWidget()
{
  if(m_serverCtrl != NULL)
    {
      delete m_serverCtrl;
    }
}
