#ifndef KFINDWIDGET_H
#define KFINDWIDGET_H

#include <QLineEdit>
#include <QPushButton>
#include <QListWidget>
#include <QVBoxLayout>
#include "kmap.h"

class KFindWidget: public QWidget
{
  Q_OBJECT

  enum Mode
  {
    Input,
    History,
    Categories
  };
  Mode mode = Input;

  QStringList history;
  QStringList categories;

  QVBoxLayout root_layout;
  QHBoxLayout button_layout;
  QLineEdit   edit;
  QPushButton history_button;
  QPushButton categories_button;
  QListWidget list;

signals:
  QStringList find(const QString& str);
  void        showObject(const QString& str);
  void        showCategory(const QString& str);
  KCategories getCategories();

public:
  KFindWidget();
  void changedFindString(const QString&);
  void onItemClicked(QListWidgetItem*);
  void showHistory();
  void showCategories();
};

#endif  // KFINDWIDGET_H
