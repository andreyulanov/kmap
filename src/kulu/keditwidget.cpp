#include "keditwidget.h"

KEditWidget::KEditWidget()
{
  setLayout(&root_layout);
  root_layout.setAlignment(Qt::AlignCenter);

  accept_button.setText("Accept");
  cancel_button.setText("Cancel");

  root_layout.addWidget(&edit);
  button_layout.addWidget(&accept_button);
  button_layout.addWidget(&cancel_button);
  root_layout.addLayout(&button_layout);

  connect(&accept_button, &QPushButton::clicked, this,
          &KEditWidget::onAccept);
  connect(&cancel_button, &QPushButton::clicked, this,
          &KEditWidget::hide);
}

void KEditWidget::onAccept()
{
  saveTrack(edit.text());
  hide();
}

void KEditWidget::show(QString title, QString default_track_name)
{
  setWindowTitle(title);
  edit.setText(default_track_name);
  QWidget::show();
}
