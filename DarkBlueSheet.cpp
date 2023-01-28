#include "DarkBlueSheet.h"

DarkBlueSheet::DarkBlueSheet():
  DarkBlueSheet(styleBase())
{ }

DarkBlueSheet::DarkBlueSheet(QStyle *style):
  QProxyStyle(style)
{ }

QStyle *DarkBlueSheet::styleBase(QStyle *style) const {
  static QStyle *base = !style ? QStyleFactory::create(QStringLiteral("Fusion")) : style;
  return base;
}

QStyle *DarkBlueSheet::baseStyle() const
{
  return styleBase();
}

void DarkBlueSheet::polish(QPalette &palette)
{
  palette.setColor(QPalette::Window,QColor(53,53,53));
  palette.setColor(QPalette::WindowText,Qt::white);
  palette.setColor(QPalette::Disabled,QPalette::WindowText,QColor(127,127,127));
  palette.setColor(QPalette::Base,QColor(42,42,42));
  palette.setColor(QPalette::AlternateBase,QColor(66,66,66));
  palette.setColor(QPalette::ToolTipBase,Qt::white);
  palette.setColor(QPalette::ToolTipText,QColor(53,53,53));
  palette.setColor(QPalette::Text,Qt::white);
  palette.setColor(QPalette::Disabled,QPalette::Text,QColor(127,127,127));
  palette.setColor(QPalette::Dark,QColor(35,35,35));
  palette.setColor(QPalette::Shadow,QColor(20,20,20));
  palette.setColor(QPalette::Button,QColor(53,53,53));
  palette.setColor(QPalette::ButtonText,Qt::white);
  palette.setColor(QPalette::Disabled,QPalette::ButtonText,QColor(127,127,127));
  palette.setColor(QPalette::BrightText,Qt::red);
  palette.setColor(QPalette::Link,QColor(255,40,0));
  palette.setColor(QPalette::Highlight,QColor(255,40,0));
  palette.setColor(QPalette::Disabled,QPalette::Highlight,QColor(80,80,80));
  palette.setColor(QPalette::HighlightedText,Qt::white);
  palette.setColor(QPalette::Disabled,QPalette::HighlightedText,QColor(127,127,127));
}

void DarkBlueSheet::polish(QApplication *app)
{
  if (!app) return;

  QFile sheetfile(QStringLiteral(":/darkbluestyle.qss"));
  if (sheetfile.open(QIODevice::ReadOnly | QIODevice::Text))
  {
    QString darkbluestyle = QString::fromLatin1(sheetfile.readAll());
    app->setStyleSheet(darkbluestyle);
    sheetfile.close();
  }
}
