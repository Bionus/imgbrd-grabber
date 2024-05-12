#include "webview-window.h"
#include <QWebEngineCookieStore>
#include <QWebEnginePage>
#include <QWebEngineProfile>
#include <utility>
#include <ui_webview-window.h>
#include "models/site.h"
#include "network/persistent-cookie-jar.h"


WebViewWindow::WebViewWindow(Site *site, QWidget *parent)
	: QDialog(parent), ui(new Ui::WebViewWindow), m_site(site)
{
	setAttribute(Qt::WA_DeleteOnClose);
	ui->setupUi(this);

	const QUrl url(m_site->fixUrl("/"));

	// Build web view
	auto *profile = new QWebEngineProfile(ui->webView);
	profile->setPersistentCookiesPolicy(QWebEngineProfile::NoPersistentCookies);
	profile->setHttpUserAgent(m_site->userAgent());
	auto *page = new QWebEnginePage(profile, ui->webView);
	page->load(url);
	ui->webView->setPage(page);

	// Set the web view's cookies based on the site's cookie jar
	QWebEngineCookieStore *cookieStore = profile->cookieStore();
	auto *cookieJar = m_site->cookieJar();
	for (const auto &cookie : cookieJar->getAllCookies()) {
		cookieStore->setCookie(cookie);
	}

	// Timer to de-bounce the saving of the cookies on disk
	m_saveCookies.setSingleShot(true);
	m_saveCookies.setInterval(1000);
	connect(&m_saveCookies, &QTimer::timeout, [=]() {
		cookieJar->save();
	});

	// Cookie listener to automatically sync the web view cookies with the site's cookie jar
	connect(cookieStore, &QWebEngineCookieStore::cookieRemoved, [=](const QNetworkCookie &cookie) {
		if (m_cookies.isEmpty()) {
			cookieJar->deleteCookie(cookie);
			m_saveCookies.start();
		}
	});
	connect(cookieStore, &QWebEngineCookieStore::cookieAdded, [=](const QNetworkCookie &cookie) {
		if (m_cookies.isEmpty() || m_cookies.contains(cookie.name())) {
			cookieJar->insertCookie(cookie);
			m_saveCookies.start();

			if (!m_cookies.isEmpty()) {
				emit cookieFound(cookie.name());
				close();
			}
		}
	});

	// Handle the signals from the basic UI
	connect(ui->lineUrl, &QLineEdit::returnPressed, [this, page]() {
		page->load(QUrl(ui->lineUrl->text()));
	});
	connect(ui->buttonGo, &QPushButton::clicked, [this, page]() {
		page->load(QUrl(ui->lineUrl->text()));
	});
	connect(page, &QWebEnginePage::urlChanged, [this](const QUrl &url) {
		ui->lineUrl->setText(url.toString());
	});
}

WebViewWindow::WebViewWindow(Site *site, QStringList cookies, QWidget *parent)
	: WebViewWindow(site, parent)
{
	m_cookies = std::move(cookies);
}

WebViewWindow::~WebViewWindow()
{
	delete ui;

	// If we are still pending a "save cookies" operation, do it now
	if (m_saveCookies.isActive()) {
		m_saveCookies.stop();
		m_site->cookieJar()->save();
	}
}
