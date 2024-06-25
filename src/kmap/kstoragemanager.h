#ifndef KSTORAGEMANAGER_H
#define KSTORAGEMANAGER_H
#include <QString>

///
/// \brief The kStorageManager class manages all data pathes.
/// Please do not manage paths manually, use this class instead.
///
class kStorageManager
{
    QString mmc_path;
    QString class_path;
    QString logs_path;
    QString objects_path;
    QString packs_path;
    QString tracks_path;
    QString database_path;
    void create(QString dir);

public:
    kStorageManager(QString _mmc_pathath);
    QString mmcPath() 		{return mmc_path;}
    QString packsPath() 	{return packs_path;}
    QString classPath() 	{return class_path;}
    QString tracksPath() 	{return tracks_path;}
    QString objectsPath() 	{return objects_path;}
    QString logsPath() 		{return logs_path;}
    QString databasePath()  {return database_path;}

};

#endif // KSTORAGEMANAGER_H
