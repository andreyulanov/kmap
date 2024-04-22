#ifndef KSETTINGS_H
#define KSETTINGS_H

#include <QSettings>
#include <QObject>

/// The class manages configuration and settings.
/// The main purpose of the class is to provide abstration over settings storage.
/// A new instance of the class is cheep to create and remove, so use it like this:
/// \code{.cpp}
/// class MyClass
/// {
/// public:
/// 	MyClass()
/// 	{
/// 		KSettings k_settins();
/// 		myParam = k_settins.myParam();
/// 	}
/// 	~MyClass()
/// 	{
/// 		KSettings k_settins();
/// 		k_settins.saveMyParam(myParam);
/// 	}
/// private:
/// 	int myParam;
/// };
/// \endcode
class KSettings : public QObject
{
public:
    explicit KSettings(QObject *parent = nullptr);
    QString jid();
    QString password();
    void saveJid(QString);
    void savePassword(QString);
    void saveAccount(QString, QString);
private:
    QSettings q_settings;
};

#endif // KSETTINGS_H
