#include "sources/sourcessettingswindow.h"
#include <QCryptographicHash>
#include <QFile>
#include <QInputDialog>
#include <QMessageBox>
#include <QNetworkCookie>
#include <QSettings>
#include <ui_sourcessettingswindow.h>
#include "functions.h"


SourcesSettingsWindow::SourcesSettingsWindow(Profile *profile, Site *site, QWidget *parent)
	: QDialog(parent), ui(new Ui::SourcesSettingsWindow), m_site(site), m_globalSettings(profile->getSettings())
{
	setAttribute(Qt::WA_DeleteOnClose);
	ui->setupUi(this);

	// Refferers
	ui->lineSiteName->setText(site->setting("name", m_site->url()).toString());
	QStringList referers = QStringList() << "none" << "host" << "page" << "image";
	QStringList referers_preview = QStringList() << "" << "none" << "host" << "page" << "image";
	QStringList referers_image = QStringList() << "" << "none" << "host" << "page" << "details" << "image";
	ui->comboReferer->setCurrentIndex(referers.indexOf(site->setting("referer", "none").toString()));
	ui->comboRefererPreview->setCurrentIndex(referers_preview.indexOf(site->setting("referer_preview", "").toString()));
	ui->comboRefererImage->setCurrentIndex(referers_image.indexOf(site->setting("referer_image", "").toString()));
	ui->spinIgnoreAlways->setValue(site->setting("ignore/always", 0).toInt());
	ui->spinIgnore1->setValue(site->setting("ignore/1", 0).toInt());
	ui->checkSsl->setChecked(site->setting("ssl", false).toBool());

	// Download settings
	ui->spinImagesPerPage->setValue(site->setting("download/imagesperpage", 200).toInt());
	ui->spinSimultaneousDownloads->setValue(site->setting("download/simultaneous", 10).toInt());
	ui->spinThrottleDetails->setValue(site->setting("download/throttle_details", 0).toInt());
	ui->spinThrottleImage->setValue(site->setting("download/throttle_image", 0).toInt());
	ui->spinThrottlePage->setValue(site->setting("download/throttle_page", 0).toInt());
	ui->spinThrottleRetry->setValue(site->setting("download/throttle_retry", 0).toInt());
	ui->spinThrottleThumbnail->setValue(site->setting("download/throttle_thumbnail", 0).toInt());

	// Source order
	ui->checkSourcesDefault->setChecked(site->setting("sources/usedefault", true).toBool());
	QStringList sources = QStringList() << "" << "xml" << "json" << "regex" << "rss";
	ui->comboSources1->setCurrentIndex(sources.indexOf(site->setting("sources/source_1", m_globalSettings->value("source_1", sources[0]).toString()).toString()));
	ui->comboSources2->setCurrentIndex(sources.indexOf(site->setting("sources/source_2", m_globalSettings->value("source_2", sources[1]).toString()).toString()));
	ui->comboSources3->setCurrentIndex(sources.indexOf(site->setting("sources/source_3", m_globalSettings->value("source_3", sources[2]).toString()).toString()));
	ui->comboSources4->setCurrentIndex(sources.indexOf(site->setting("sources/source_4", m_globalSettings->value("source_4", sources[3]).toString()).toString()));

	// Credentials
	ui->lineAuthPseudo->setText(site->setting("auth/pseudo", "").toString());
	ui->lineAuthPassword->setText(site->setting("auth/password", "").toString());

	// Login
	QStringList types = QStringList() << "url" << "get" << "post" << "oauth1" << "oauth2";
	QString defaultType = site->setting("login/parameter", true).toBool() ? "url" : site->setting("login/method", "post").toString();
	QString type = site->setting("login/type", defaultType).toString();
	ui->comboLoginType->setCurrentIndex(types.indexOf(type));
	ui->lineLoginGetUrl->setText(site->setting("login/get/url", type != "get" ? "" : site->setting("login/url", "").toString()).toString());
	ui->lineLoginGetPseudo->setText(site->setting("login/get/pseudo", type != "get" ? "" : site->setting("login/pseudo", "").toString()).toString());
	ui->lineLoginGetPassword->setText(site->setting("login/get/password", type != "get" ? "" : site->setting("login/password", "").toString()).toString());
	ui->lineLoginGetCookie->setText(site->setting("login/get/cookie", type != "get" ? "" : site->setting("login/cookie", "").toString()).toString());
	ui->lineLoginPostUrl->setText(site->setting("login/post/url", type != "post" ? "" : site->setting("login/url", "").toString()).toString());
	ui->lineLoginPostPseudo->setText(site->setting("login/post/pseudo", type != "post" ? "" : site->setting("login/pseudo", "").toString()).toString());
	ui->lineLoginPostPassword->setText(site->setting("login/post/password", type != "post" ? "" : site->setting("login/password", "").toString()).toString());
	ui->lineLoginPostCookie->setText(site->setting("login/post/cookie", type != "post" ? "" : site->setting("login/cookie", "").toString()).toString());
	ui->lineLoginOAuth1RequestTokenUrl->setText(site->setting("login/oauth1/requestTokenUrl", "").toString());
	ui->lineLoginOAuth1AuthorizeUrl->setText(site->setting("login/oauth1/authorizeUrl", "").toString());
	ui->lineLoginOAuth1AccessTokenUrl->setText(site->setting("login/oauth1/accessTokenUrl", "").toString());
	ui->lineLoginOAuth2RequestUrl->setText(site->setting("login/oauth2/requestUrl", "").toString());
	ui->lineLoginOAuth2TokenUrl->setText(site->setting("login/oauth2/tokenUrl", "").toString());
	ui->lineLoginOAuth2RefreshTokenUrl->setText(site->setting("login/oauth2/refreshTokenUrl", "").toString());
	ui->lineLoginOAuth2Scope->setText(site->setting("login/oauth2/scope", "").toString());
	ui->spinLoginMaxPage->setValue(site->setting("login/maxPage", 0).toInt());

	// Hide hash if unncessary
	if (site->value("PasswordSalt").isEmpty())
	{ ui->buttonAuthHash->hide(); }
	else
	{ ui->lineAuthPassword->setEchoMode(QLineEdit::Normal); }

	// Cookies
	QList<QNetworkCookie> cookies = site->cookies();
	ui->tableCookies->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
	ui->tableCookies->setRowCount(cookies.count());
	int row = 0;
	for (const QNetworkCookie &cookie : site->cookies())
	{
		ui->tableCookies->setItem(row, 0, new QTableWidgetItem(QString(cookie.name())));
		ui->tableCookies->setItem(row, 1, new QTableWidgetItem(QString(cookie.value())));
		row++;
	}

	// Headers
	QMap<QString, QVariant> headers = site->setting("headers").toMap();
	ui->tableHeaders->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
	ui->tableHeaders->setRowCount(headers.count());
	int headerRow = 0;
	QMapIterator<QString, QVariant> i(headers);
	while (i.hasNext())
	{
		i.next();
		ui->tableHeaders->setItem(headerRow, 0, new QTableWidgetItem(i.key()));
		ui->tableHeaders->setItem(headerRow, 1, new QTableWidgetItem(i.value().toString()));
		headerRow++;
	}

	// Hide login testing buttons if we can't tests this site's login
	if (!m_site->canTestLogin())
	{
		ui->widgetTestCredentials->hide();
		ui->widgetTestLogin->hide();
	}

	connect(this, SIGNAL(accepted()), this, SLOT(save()));
}

SourcesSettingsWindow::~SourcesSettingsWindow()
{
	delete ui;
}

void SourcesSettingsWindow::addCookie()
{
	ui->tableCookies->setRowCount(ui->tableCookies->rowCount() + 1);
}
void SourcesSettingsWindow::addHeader()
{
	ui->tableHeaders->setRowCount(ui->tableHeaders->rowCount() + 1);
}

void SourcesSettingsWindow::on_buttonAuthHash_clicked()
{
	QString password = QInputDialog::getText(this, tr("Hash a password"), tr("Please enter your password below.<br/>It will then be hashed using the format \"%1\".").arg(m_site->value("PasswordSalt")));
	if (!password.isEmpty())
	{ ui->lineAuthPassword->setText(QCryptographicHash::hash(m_site->value("PasswordSalt").replace("%password%", password).toUtf8(), QCryptographicHash::Sha1).toHex()); }
}

void SourcesSettingsWindow::deleteSite()
{
	int reponse = QMessageBox::question(this, tr("Delete a site"), tr("Are you sure you want to delete the site %1?").arg(m_site->name()), QMessageBox::Yes | QMessageBox::No);
	if (reponse == QMessageBox::Yes)
	{
		QFile f(m_site->getSource()->getPath() + "/sites.txt");
		f.open(QIODevice::ReadOnly);
			QString sites = f.readAll();
		f.close();
		sites.replace("\r\n", "\n").replace("\r", "\n").replace("\n", "\r\n");
		QStringList stes = sites.split("\r\n", QString::SkipEmptyParts);
		stes.removeAll(m_site->url());
		f.open(QIODevice::WriteOnly);
			f.write(stes.join("\r\n").toLatin1());
		f.close();
		close();
		emit siteDeleted(m_site->url());
	}
}

void SourcesSettingsWindow::testLogin()
{
	save();

	setLoginStatus(tr("Connection..."));

	connect(m_site, &Site::loggedIn, this, &SourcesSettingsWindow::loginTested);
	m_site->login(true);
}

void SourcesSettingsWindow::loginTested(Site*, Site::LoginResult result)
{
	switch (result)
	{
		case Site::LoginResult::Success:
			setLoginStatus(tr("Success!"));
			break;

		case Site::LoginResult::Error:
			setLoginStatus(tr("Failure"));
			break;

		default:
			setLoginStatus(tr("Unable to test"));
			break;
	}
}

void SourcesSettingsWindow::setLoginStatus(const QString &msg)
{
	QString italic = QString("<i>%1</li>").arg(msg);
	ui->labelTestCredentials->setText(italic);
	ui->labelTestLogin->setText(italic);
}

void SourcesSettingsWindow::save()
{
	m_site->setSetting("name", ui->lineSiteName->text(), m_site->url());
	QStringList referers = QStringList() << "none" << "host" << "page" << "image";
	QStringList referers_preview = QStringList() << "" << "none" << "host" << "page" << "image";
	QStringList referers_image = QStringList() << "" << "none" << "host" << "page" << "details" << "image";
	m_site->setSetting("referer", referers[ui->comboReferer->currentIndex()], "none");
	m_site->setSetting("referer_preview", referers_preview[ui->comboRefererPreview->currentIndex()], "");
	m_site->setSetting("referer_image", referers_image[ui->comboRefererImage->currentIndex()], "");
	m_site->setSetting("ignore/always", ui->spinIgnoreAlways->value(), 0);
	m_site->setSetting("ignore/1", ui->spinIgnore1->value(), 0);
	m_site->setSetting("ssl", ui->checkSsl->isChecked(), false);

	m_site->setSetting("download/imagesperpage", ui->spinImagesPerPage->value(), 200);
	m_site->setSetting("download/simultaneous", ui->spinSimultaneousDownloads->value(), 10);
	m_site->setSetting("download/throttle_details", ui->spinThrottleDetails->value(), 0);
	m_site->setSetting("download/throttle_image", ui->spinThrottleImage->value(), 0);
	m_site->setSetting("download/throttle_page", ui->spinThrottlePage->value(), 0);
	m_site->setSetting("download/throttle_retry", ui->spinThrottleRetry->value(), 0);
	m_site->setSetting("download/throttle_thumbnail", ui->spinThrottleThumbnail->value(), 0);

	QStringList sources = QStringList() << "" << "xml" << "json" << "regex" << "rss";
	m_site->setSetting("sources/usedefault", ui->checkSourcesDefault->isChecked(), true);
	m_site->setSetting("sources/source_1", sources[qMax(0, ui->comboSources1->currentIndex())], m_globalSettings->value("source_1", sources[0]).toString());
	m_site->setSetting("sources/source_2", sources[qMax(0, ui->comboSources2->currentIndex())], m_globalSettings->value("source_2", sources[1]).toString());
	m_site->setSetting("sources/source_3", sources[qMax(0, ui->comboSources3->currentIndex())], m_globalSettings->value("source_3", sources[2]).toString());
	m_site->setSetting("sources/source_4", sources[qMax(0, ui->comboSources4->currentIndex())], m_globalSettings->value("source_4", sources[3]).toString());


	m_site->setSetting("auth/pseudo", ui->lineAuthPseudo->text(), "");
	m_site->setSetting("auth/password", ui->lineAuthPassword->text(), "");

	// Login
	QStringList types = QStringList() << "url" << "get" << "post" << "oauth1" << "oauth2";
	m_site->setSetting("login/type", types[ui->comboLoginType->currentIndex()], "url");
	m_site->setSetting("login/get/url", ui->lineLoginGetUrl->text(), "");
	m_site->setSetting("login/get/pseudo", ui->lineLoginGetPseudo->text(), "");
	m_site->setSetting("login/get/password", ui->lineLoginGetPassword->text(), "");
	m_site->setSetting("login/get/cookie", ui->lineLoginGetCookie->text(), "");
	m_site->setSetting("login/post/url", ui->lineLoginPostUrl->text(), "");
	m_site->setSetting("login/post/pseudo", ui->lineLoginPostPseudo->text(), "");
	m_site->setSetting("login/post/password", ui->lineLoginPostPassword->text(), "");
	m_site->setSetting("login/post/cookie", ui->lineLoginPostCookie->text(), "");
	m_site->setSetting("login/oauth1/requestTokenUrl", ui->lineLoginOAuth1RequestTokenUrl->text(), "");
	m_site->setSetting("login/oauth1/authorizeUrl", ui->lineLoginOAuth1AuthorizeUrl->text(), "");
	m_site->setSetting("login/oauth1/accessTokenUrl", ui->lineLoginOAuth1AccessTokenUrl->text(), "");
	m_site->setSetting("login/oauth2/requestUrl", ui->lineLoginOAuth2RequestUrl->text(), "");
	m_site->setSetting("login/oauth2/tokenUrl", ui->lineLoginOAuth2TokenUrl->text(), "");
	m_site->setSetting("login/oauth2/refreshTokenUrl", ui->lineLoginOAuth2RefreshTokenUrl->text(), "");
	m_site->setSetting("login/oauth2/scope", ui->lineLoginOAuth2Scope->text(), "");
	m_site->setSetting("login/maxPage", ui->spinLoginMaxPage->value(), 0);

	// Cookies
	QList<QVariant> cookies;
	for (int i = 0; i < ui->tableCookies->rowCount(); ++i)
	{
		if (ui->tableCookies->item(i, 0)->text().isEmpty())
			continue;

		QNetworkCookie cookie;
		cookie.setName(ui->tableCookies->item(i, 0)->text().toLatin1());
		cookie.setValue(ui->tableCookies->item(i, 1)->text().toLatin1());
		cookies.append(cookie.toRawForm());
	}
	m_site->setSetting("cookies", cookies, QList<QVariant>());

	// Headers
	QMap<QString, QVariant> headers;
	for (int i = 0; i < ui->tableHeaders->rowCount(); ++i)
	{
		if (ui->tableHeaders->item(i, 0)->text().isEmpty())
			continue;

		headers.insert(ui->tableHeaders->item(i, 0)->text(), ui->tableHeaders->item(i, 1)->text());
	}
	m_site->setSetting("headers", headers, QMap<QString, QVariant>());

	m_site->syncSettings();

	m_site->loadConfig();
}
