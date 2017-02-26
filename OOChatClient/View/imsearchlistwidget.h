#ifndef IMSEARCHLISTWIDGET_H
#define IMSEARCHLISTWIDGET_H

#include <QWidget>

class QVBoxLayout;
class QScrollArea;
class QMenu;
class IMMainWidget;

class IMSearchListWidget: public QWidget
{
  Q_OBJECT
public:
  IMSearchListWidget(QWidget *parent = 0);
  void addItem(QWidget *item);
  void removeItem(QWidget *item);
  int size();
  void clearAll();

private:
  IMMainWidget *m_mainWidget;
  QWidget *m_contentsWidget;
  QScrollArea *m_scrollArea;
  QVBoxLayout *m_layout;

  int m_msgCounter;
};

#endif // IMSEARCHLISTWIDGET_H
