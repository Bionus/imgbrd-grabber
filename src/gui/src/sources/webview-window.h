/*#ifndef WEBVIEW_WINDOW_H
#define WEBVIEW_WINDOW_H

#include <QDialog>
#include <QStringList>
#include <QTimer>


class QWidget;
class Site;

class WebViewWindow : public QDialog
{

	public:
		WebViewWindow(Site *site, QStringList cookies = {}, QWidget *parent = nullptr);
		~WebViewWindow() override;

	private:
		Site *m_site;
		QTimer m_saveCookies;
};

#endif // WEBVIEW_WINDOW_H
*/