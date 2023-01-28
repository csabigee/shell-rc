#ifndef _DARKBLUESHEET_H
#define _DARKBLUESHEET_H

#include <QApplication>
#include <QProxyStyle>
#include <QStyleFactory>
#include <QFile>

class DarkBlueSheet : public QProxyStyle
{
  Q_OBJECT

public:
  DarkBlueSheet();
  explicit DarkBlueSheet(QStyle *style);

  QStyle *baseStyle() const;

  void polish(QPalette &palette) override;
  void polish(QApplication *app) override;

private:
  QStyle *styleBase(QStyle *style = Q_NULLPTR) const;
};

#endif  /* _DARKBLUESHEET_H */
