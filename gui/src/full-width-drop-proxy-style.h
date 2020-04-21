#ifndef FULL_WITH_PROXY_STYLE_H
#define FULL_WITH_PROXY_STYLE_H

#include <QProxyStyle>
#include <QStyle>


class QPainter;
class QStyleOption;
class QWidget;

class FullWidthDropProxyStyle : public QProxyStyle
{
	public:
		FullWidthDropProxyStyle(QStyle *style = nullptr);
		void drawPrimitive(QStyle::PrimitiveElement element, const QStyleOption *option, QPainter *painter, const QWidget *widget = nullptr) const override;
};

#endif // FULL_WITH_PROXY_STYLE_H
