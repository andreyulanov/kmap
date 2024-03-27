#include "kfindwidget.h"

KFindWidget::KFindWidget()
{
  setLayout(&root_layout);
  root_layout.setAlignment(Qt::AlignCenter);

  root_layout.addWidget(&edit);
  history_button.setIcon(QIcon(":/labels/history.png"));
  button_layout.addWidget(&history_button);
  categories_button.setIcon(QIcon(":/labels/category.png"));
  button_layout.addWidget(&categories_button);
  root_layout.addLayout(&button_layout);

  root_layout.addWidget(&list);
  connect(&edit, &QLineEdit::textChanged, this,
          &KFindWidget::changedFindString);
  connect(&list, &QListWidget::itemClicked, this,
          &KFindWidget::onItemClicked);
  connect(&history_button, &QPushButton::clicked, this,
          &KFindWidget::showHistory);
  connect(&categories_button, &QPushButton::clicked, this,
          &KFindWidget::showCategories);
}

void KFindWidget::showHistory()
{
  list.clear();
  list.addItems(history);
  mode = History;
}

void KFindWidget::showCategories()
{
  list.clear();
  auto cats = getCategories();
  for (auto cat: cats)
  {
    QListWidgetItem* item = new QListWidgetItem(
        QIcon(QPixmap::fromImage(cat.image)), cat.name);
    list.addItem(item);
  }
  mode = Categories;
}

void KFindWidget::onItemClicked(QListWidgetItem* item)
{
  if (mode == Input)
  {
    hide();
    auto text = item->text();
    showObject(text);
    if (!history.contains(text))
      history.append(text);
  }
  if (mode == History)
  {
    auto text = item->text();
    list.clear();
    edit.setText(text);
    mode = Input;
  }
  if (mode == Categories)
  {
    hide();
    showCategory(item->text());
  }
}

void KFindWidget::changedFindString(const QString& str)
{
  list.clear();
  if (str.count() > 3)
  {
    auto l = find(str);
    list.addItems(l);
  }
}
