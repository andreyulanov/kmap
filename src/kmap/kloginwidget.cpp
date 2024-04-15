#include "kloginwidget.h"
#include <QLabel>

KLoginWidget::KLoginWidget(QWidget *parent)
    : QWidget{parent}
{
    root_layout 			= new QVBoxLayout(this);
    login_form_layout		= new QFormLayout;
    QLabel* login_label		= new QLabel("Login");
    QLabel* password_label 	= new QLabel("Password");
    login_edit				= new QLineEdit;
    password_edit	 		= new QLineEdit;
    show_password_check_box = new QCheckBox("Show password");
    button_box 				= new QDialogButtonBox(QDialogButtonBox::Ok
                                                 | QDialogButtonBox::Cancel);

    login_form_layout->addRow(login_label, login_edit);
    login_form_layout->addRow(password_label, password_edit);
    login_form_layout->addWidget(show_password_check_box);
    root_layout->addLayout(login_form_layout);
    root_layout->addWidget(button_box);

    password_edit->setEchoMode(QLineEdit::Password);

    connect(button_box, &QDialogButtonBox::accepted, this, &KLoginWidget::onAccepted);
    connect(button_box, &QDialogButtonBox::rejected, this, &KLoginWidget::onRejected);
    connect(show_password_check_box, &QCheckBox::stateChanged, this, &KLoginWidget::showPassrodChanged);
}

KLoginWidget::KLoginWidget(const QSize& size,
                           const QString& login,
                           const QString& password,
                           QWidget *parent)
    : KLoginWidget(parent)
{
    this->setFixedSize(size);
    login_edit->setText(login);
    password_edit->setText(password);
}

void KLoginWidget::onAccepted()
{
    emit connectToServer(login_edit->text(), password_edit->text());
    this->hide();
}

void KLoginWidget::onRejected()
{
    this->hide();
}

void KLoginWidget::showPassrodChanged(int state)
{
    if (state == Qt::Unchecked)
        password_edit->setEchoMode(QLineEdit::Password);
    else
        password_edit->setEchoMode(QLineEdit::Normal);
}
