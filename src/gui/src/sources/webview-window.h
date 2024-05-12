#ifndef WEBVIEW_WINDOW_H
#define WEBVIEW_WINDOW_H

#include <QDialog>
#include <QTimer>


namespace Ui
{
	class WebViewWindow;
}


class QWidget;
class Site;

class WebViewWindow : public QDialog
{
	Q_OBJECT

	public:
		/**
		 * A window used to try to load the site's homepage in a web view.
		 * Will also update the cookies so that they can be used in future "raw" API requests.
		 *
		 * @param site The site to try to load
		 * @param parent The parent widget
		 */
		explicit WebViewWindow(Site *site, QWidget *parent = nullptr);

		/**
		 * A special constructor that will cause the window to close once any of the given cookies is saved.
		 *
		 * @param site The site to try to load
		 * @param cookies The list of cookies to wait for in the web view
		 * @param parent The parent widget
		 */
		explicit WebViewWindow(Site *site, QStringList cookies, QWidget *parent = nullptr);

		/**
		 * Destroys this window.
		 */
		~WebViewWindow() override;

	signals:
		/**
		 * Emitted once a cookie passed to the constructor is saved.
		 *
		 * @param cookieName The name of the cookie that was found
		 */
		void cookieFound(const QString &cookieName);

	private:
		Ui::WebViewWindow *ui;
		Site *m_site;
		QTimer m_saveCookies;
		QStringList m_cookies;
};

#endif // WEBVIEW_WINDOW_H
