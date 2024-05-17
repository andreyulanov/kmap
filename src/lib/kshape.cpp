#include "kshape.h"
#include "kserialize.h"
#include <math.h>
#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QMetaEnum>

void KShape::save(QFile* f)
{
  using namespace KSerialize;
  write(f, id);
  write(f, type);
  write(f, style);
  write(f, layer);
  write(f, width_mm);
  write(f, min_mip);
  write(f, max_mip);
  write(f, coor_precision_coef);
  write(f, (uchar)pen.red());
  write(f, (uchar)pen.green());
  write(f, (uchar)pen.blue());
  write(f, (uchar)pen.alpha());
  write(f, (uchar)brush.red());
  write(f, (uchar)brush.green());
  write(f, (uchar)brush.blue());
  write(f, (uchar)brush.alpha());
  write(f, (uchar)tcolor.red());
  write(f, (uchar)tcolor.green());
  write(f, (uchar)tcolor.blue());
  write(f, (uchar)tcolor.alpha());
  write(f, image);
}

int KShape::getWidthPix()
{
  return round(width_mm / pixel_size_mm);
}

void KShape::load(QFile* f, double _pixel_size_mm)
{
  using namespace KSerialize;
  pixel_size_mm = _pixel_size_mm;
  read(f, id);
  read(f, type);
  read(f, style);
  read(f, layer);
  read(f, width_mm);
  read(f, min_mip);
  read(f, max_mip);
  read(f, coor_precision_coef);
  uchar red, green, blue, alpha;
  read(f, red);
  read(f, green);
  read(f, blue);
  read(f, alpha);
  pen = QColor(red, green, blue, alpha);
  read(f, red);
  read(f, green);
  read(f, blue);
  read(f, alpha);
  brush = QColor(red, green, blue, alpha);
  read(f, red);
  read(f, green);
  read(f, blue);
  read(f, alpha);
  tcolor = QColor(red, green, blue, alpha);
  QImage img;
  read(f, img);
  if (!img.isNull())
    image =
        img.scaledToWidth(getWidthPix(), Qt::SmoothTransformation);
}

void KShapeManager::loadShapes(QString path, QString images_dir)
{
  QFile         f(path);
  QSet<QString> id_set;
  if (f.open(QIODevice::ReadOnly))
  {
    QJsonParseError error;
    auto doc = QJsonDocument::fromJson(f.readAll(), &error);
    f.close();
    if (doc.isNull())
      return;

    if (doc.isArray())
    {
      auto ar = doc.array();
      for (auto json_val: ar)
      {
        auto obj = json_val.toObject();
        if (obj.isEmpty())
          continue;
        auto _main_mip = obj.value("main_mip").toDouble();
        if (_main_mip > 0)
        {
          main_mip = _main_mip;
          continue;
        }
        auto _tile_mip = obj.value("tile_mip").toDouble();
        if (_tile_mip > 0)
        {
          tile_mip = _tile_mip;
          continue;
        }
        auto _coor_precision_coef =
            obj.value("default_coor_precision_coef").toInt();
        if (_coor_precision_coef > 0)
        {
          default_coor_precision_coef = _coor_precision_coef;
          continue;
        }

        auto sh = new KShape;

        sh->id = obj.value("id").toString();
        if (id_set.contains(sh->id))
        {
          error_str = sh->id + " has been already defined!";
          return;
        }
        id_set.insert(sh->id);
        sh->pan_code = obj.value("code").toInt();
        sh->pan_key  = obj.value("key").toString();
        sh->attrname = obj.value("attrname").toString();
        sh->attrval  = obj.value("attrval").toString();

        auto type_str = obj.value("type").toString();
        if (!type_str.isEmpty())
        {
          bool ok  = false;
          sh->type = static_cast<KShape::Type>(
              QMetaEnum::fromType<KShape::Type>().keyToValue(
                  type_str.toUtf8(), &ok));
          if (!ok)
          {
            error_str = type_str + "is an unrecognized type!";
            return;
          }
        }

        sh->style      = KShape::Solid;
        auto style_str = obj.value("style").toString();
        if (!style_str.isEmpty())
        {
          bool ok   = false;
          sh->style = static_cast<KShape::Style>(
              QMetaEnum::fromType<KShape::Style>().keyToValue(
                  style_str.toUtf8(), &ok));
          if (!ok)
          {
            error_str = type_str + "is an unrecognized style!";
            return;
          }
        }

        sh->layer    = obj.value("layer").toInt();
        sh->width_mm = obj.value("width_mm").toDouble();

        auto l = obj.value("pen").toArray().toVariantList();
        if (l.count() >= 3)
          sh->pen = QColor{l.at(0).toInt(), l.at(1).toInt(),
                           l.at(2).toInt()};
        if (l.count() == 4)
          sh->pen.setAlpha(l.at(3).toInt());
        else
          sh->pen.setAlpha(255);

        l = obj.value("brush").toArray().toVariantList();
        if (l.count() >= 3)
          sh->brush = QColor{l.at(0).toInt(), l.at(1).toInt(),
                             l.at(2).toInt()};
        if (l.count() == 4)
          sh->brush.setAlpha(l.at(3).toInt());
        else
          sh->brush.setAlpha(255);

        l = obj.value("tcolor").toArray().toVariantList();
        if (l.count() >= 3)
          sh->tcolor = QColor{l.at(0).toInt(), l.at(1).toInt(),
                              l.at(2).toInt()};
        if (l.count() == 4)
          sh->tcolor.setAlpha(l.at(3).toInt());
        else
          sh->tcolor.setAlpha(255);

        sh->min_mip     = obj.value("min_mip").toDouble();
        sh->max_mip     = obj.value("max_mip").toDouble();
        sh->name_code   = obj.value("name_code").toInt();
        auto image_name = obj.value("image").toString();
        if (!image_name.isEmpty())
        {
          auto image_path = images_dir + "/" + image_name;
          auto image      = QImage(image_path);
          if (image.isNull())
            qDebug() << "error opening" << image_path;
          else
            sh->image = image.scaledToWidth(sh->getWidthPix(),
                                            Qt::SmoothTransformation);
        }

        auto attributes = obj.value("attributes");
        if (attributes.isArray())
        {
          auto attr_array = attributes.toArray();
          for (auto attr_entry: attr_array)
          {
            auto attr_obj = attr_entry.toObject();
            if (obj.isEmpty())
              continue;
            KAttribute attr;
            attr.code    = attr_obj.value("code").toInt();
            attr.name    = attr_obj.value("name").toString();
            attr.visible = attr_obj.value("visible").toBool();
            attr.max_mip = attr_obj.value("max_mip").toDouble();
            sh->attributes.append(attr);
          }
        }

        sh->coor_precision_coef =
            obj.value("coor_precision_coef").toInt();
        if (sh->coor_precision_coef == 0)
          sh->coor_precision_coef = default_coor_precision_coef;

        shapes.append(sh);
      }
    }
  }
}

KShapeManager::KShapeManager(QString _images_dir)
{
  images_dir = _images_dir;
}

int KShapeManager::getShapeIdxById(QString id)
{
  for (int i = -1; auto sh: shapes)
  {
    i++;
    if (sh->id == id)
      return i;
  }
  return -1;
}

KShape KShapeManager::getShapeById(QString id)
{
  auto idx = getShapeIdxById(id);
  if (idx >= 0)
    return *shapes.at(idx);
  else
    return KShape();
}

int KShapeManager::getShapeIdx(int code, QString key,
                               QStringList attr_names,
                               QStringList attr_values)
{
  for (int idx = -1; auto& sh: shapes)
  {
    idx++;
    bool code_match = true;
    bool key_match  = true;
    bool attr_match = true;

    if (sh->pan_code == 0 && sh->pan_key.isEmpty() &&
        sh->attrname.isEmpty() && sh->attrval.isEmpty())
      continue;

    if (sh->pan_code > 0 && sh->pan_code != code)
      code_match = false;
    if (!sh->pan_key.isEmpty() && sh->pan_key != key)
      key_match = false;
    if (!sh->attrname.isEmpty() && !attr_names.contains(sh->attrname))
      attr_match = false;
    if (!sh->attrval.isEmpty() && !attr_values.contains(sh->attrval))
      attr_match = false;

    if (code_match && key_match && attr_match)
      return idx;
  }
  return -1;
}

QVector<KShape> KShapeManager::getShapes()
{
  QVector<KShape> ret;
  for (auto sh1: shapes)
  {
    auto sh2 = *sh1;
    ret.append(sh2);
  }
  return ret;
}

KShapeImageList KShapeManager::getShapeImageList()
{
  QVector<KShapeImage> ret;
  for (auto sh: shapes)
    ret.append({sh->id, sh->image});
  return ret;
}
