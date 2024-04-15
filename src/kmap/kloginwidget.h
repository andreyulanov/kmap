#ifndef KACCOUNTWIDGET_H
#define KACCOUNTWIDGET_H

#include <QObject>
#include <QWidget>
#include <QFormLayout>
#include <QLineEdit>
#include <QDialogButtonBox>
#include <QCheckBox>

class KLoginWidget : public QWidget
{
    Q_OBJECT
public:
    explicit KLoginWidget(QWidget *parent = nullptr);
    explicit KLoginWidget(const QSize& size,
                          const QString& login = QString(),
                          const QString& password = QString(),
                          QWidget *parent = nullptr);
private:
    QVBoxLayout*		root_layout;
    QFormLayout*		login_form_layout;
    QDialogButtonBox*	button_box;
    QLineEdit* 			login_edit;
    QLineEdit*			password_edit;
    QCheckBox* 			show_password_check_box;

signals:
    void connectToServer(const QString& login, const QString& password);
private slots:
    void onAccepted();
    void onRejected();
    void showPassrodChanged(int state);
};

#endif // KACCOUNTWIDGET_H
