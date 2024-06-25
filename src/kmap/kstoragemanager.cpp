#include <QDir>
#include <QDebug>
#include "kstoragemanager.h"

///
/// \brief kStorageManager::kStorageManager
/// \param _mmc_path root app's data dir
///
kStorageManager::kStorageManager(QString _mmc_path)
{
    mmc_path = _mmc_path;
    class_path 		= mmc_path + "/class";
    logs_path		= mmc_path + "/logs";
    objects_path 	= mmc_path + "/objects";
    packs_path 		= mmc_path + "/packs";
    tracks_path 	= mmc_path + "/tracks";
    database_path	= mmc_path + "/main.db";
    create(mmc_path);
    create(class_path);
    create(logs_path);
    create(objects_path);
    create(packs_path);
    create(tracks_path);

}

///
/// \brief Create dir if it does not exist.
/// \param path path to the directory to create
///
void kStorageManager::create(QString path)
{
    QDir dir(path);
    if (!dir.exists())
    {
        if (!dir.mkpath(path))
            qCritical() << "Unable to create directory" << path;
    }
}
