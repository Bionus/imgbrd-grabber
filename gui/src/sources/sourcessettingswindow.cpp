#include "sourcessettingswindow.h"
#include "ui_sourcessettingswindow.h"
#include <QMessageBox>
#include <QInputDialog>
#include <QFile>
#include <QCryptographicHash>
#include <QSettings>
#include <QNetworkCookie>
#include "functions.h"


SourcesSettingsWindow::SourcesSettingsWindow(Site *site, QWidget *parent) : QDialog(parent), ui(new Ui::SourcesSettingsWindow), m_site(site)
{
	setAttribute(Qt::WA_DeleteOnClose);
	ui->setupUi(this);

	QSettings *settings = site->settings();
	QSettings global(savePath("settings.ini"), QSettings::IniFormat);

	// Refferers
	ui->lineSiteName->setText(settings->value("name", m_site->url()).toString());
	QStringList referers = QStringList() << "none" << "host" << "page" << "image";
	QStringList referers_preview = QStringList() << "" << "none" << "host" << "page" << "image";
	QStringList referers_image = QStringList() << "" << "none" << "host" << "page" << "details" << "image";
	ui->comboReferer->setCurrentIndex(referers.indexOf(settings->value("referer", "none").toString()));
	ui->comboRefererPreview->setCurrentIndex(referers_preview.indexOf(settings->value("referer_preview", "").toString()));
	ui->comboRefererImage->setCurrentIndex(referers_image.indexOf(settings->value("referer_image", "").toString()));
	ui->spinIgnoreAlways->setValue(settings->value("ignore/always", 0).toInt());
	ui->spinIgnore1->setValue(settings->value("ignore/1", 0).toInt());
	ui->checkSsl->setChecked(settings->value("ssl", false).toBool());

	// Download settings
	ui->spinImagesPerPage->setValue(settings->value("download/imagesperpage", 200).toInt());
	ui->spinSimultaneousDownloads->setValue(settings->value("download/simultaneous", 10).toInt());
	ui->spinThrottleDetails->setValue(settings->value("download/throttle_details", 0).toInt());
	ui->spinThrottleImage->setValue(settings->value("download/throttle_image", 0).toInt());
	ui->spinThrottlePage->setValue(settings->value("download/throttle_page", 0).toInt());
	ui->spinThrottleRetry->setValue(settings->value("download/throttle_retry", 0).toInt());
	ui->spinThrottleThumbnail->setValue(settings->value("download/throttle_thumbnail", 0).toInt());

	// Source order
	ui->checkSourcesDefault->setChecked(settings->value("sources/usedefault", true).toBool());
	QStringList sources = QStringList() << "" << "xml" << "json" << "regex" << "rss";
	ui->comboSources1->setCurrentIndex(sources.indexOf(settings->value("sources/source_1", global.value("source_1", sources[0]).toString()).toString()));
	ui->comboSources2->setCurrentIndex(sources.indexOf(settings->value("sources/source_2", global.value("source_2", sources[1]).toString()).toString()));
	ui->comboSources3->setCurrentIndex(sources.indexOf(settings->value("sources/source_3", global.value("source_3", sources[2]).toString()).toString()));
	ui->comboSources4->setCurrentIndex(sources.indexOf(settings->value("sources/source_4", global.value("source_4", sources[3]).toString()).toString()));

	// Credentials
	ui->lineAuthPseudo->setText(settings->value("auth/pseudo", "").toString());
	ui->lineAuthPassword->setText(settings->value("auth/password", "").toString());

	// Login
	QStringList types = QStringList() << "url" << "get" << "post" << "oauth1" << "oauth2";
	QString defaultType = settings->value("login/parameter", true).toBool() ? "url" : settings->value("login/method", "post").toString();
	QString type = settings->value("login/type", defaultType).toString();
	ui->comboLoginType->setCurrentIndex(types.indexOf(type));
	ui->lineLoginGetUrl->setText(settings->value("login/get/url", type != "get" ? "" : settings->value("login/url", "").toString()).toString());
	ui->lineLoginGetPseudo->setText(settings->value("login/get/pseudo", type != "get" ? "" : settings->value("login/pseudo", "").toString()).toString());
	ui->lineLoginGetPassword->setText(settings->value("login/get/password", type != "get" ? "" : settings->value("login/password", "").toString()).toString());
	ui->lineLoginGetCookie->setText(settings->value("login/get/cookie", type != "get" ? "" : settings->value("login/cookie", "").toString()).toString());
	ui->lineLoginPostUrl->setText(settings->value("login/post/url", type != "post" ? "" : settings->value("login/url", "").toString()).toString());
	ui->lineLoginPostPseudo->setText(settings->value("login/post/pseudo", type != "post" ? "" : settings->value("login/pseudo", "").toString()).toString());
	ui->lineLoginPostPassword->setText(settings->value("login/post/password", type != "post" ? "" : settings->value("login/password", "").toString()).toString());
	ui->lineLoginPostCookie->setText(settings->value("login/post/cookie", type != "post" ? "" : settings->value("login/cookie", "").toString()).toString());
	ui->lineLoginOAuth1RequestTokenUrl->setText(settings->value("login/oauth1/requestTokenUrl", "").toString());
	ui->lineLoginOAuth1AuthorizeUrl->setText(settings->value("login/oauth1/authorizeUrl", "").toString());
	ui->lineLoginOAuth1AccessTokenUrl->setText(settings->value("login/oauth1/accessTokenUrl", "").toString());
	ui->lineLoginOAuth2RequestUrl->setText(settings->value("login/oauth2/requestUrl", "").toString());
	ui->lineLoginOAuth2TokenUrl->setText(settings->value("login/oauth2/tokenUrl", "").toString());
	ui->lineLoginOAuth2RefreshTokenUrl->setText(settings->value("login/oauth2/refreshTokenUrl", "").toString());
	ui->lineLoginOAuth2Scope->setText(settings->value("login/oauth2/scope", "").toString());
	ui->spinLoginMaxPage->setValue(settings->value("login/maxPage", 0).toInt());

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
	for (QNetworkCookie cookie : site->cookies())
	{
		ui->tableCookies->setItem(row, 0, new QTableWidgetItem(QString(cookie.name())));
		ui->tableCookies->setItem(row, 1, new QTableWidgetItem(QString(cookie.value())));
		row++;
	}

	// Headers
	QMap<QString, QVariant> headers = site->settings()->value("headers").toMap();
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
		QFile f(savePath("sites/"+m_site->type()+"/sites.txt"));
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

	ui->labelTestCredentials->setText("<i>Connexion...</li>");
	ui->labelTestLogin->setText("<i>Connexion...</li>");

	connect(m_site, &Site::loggedIn, this, &SourcesSettingsWindow::loginTested);
	m_site->login(true);
}

void SourcesSettingsWindow::loginTested(Site*, Site::LoginResult result)
{
	switch (result)
	{
		case Site::LoginResult::Success:
			ui->labelTestCredentials->setText("<i>" + tr("Success!") + "</i>");
			ui->labelTestLogin->setText("<i>" + tr("Success!") + "</i>");
			break;

		case Site::LoginResult::Error:
			ui->labelTestCredentials->setText("<i>" + tr("Failure") + "</i>");
			ui->labelTestLogin->setText("<i>" + tr("Failure") + "</i>");
			break;

		default:
			ui->labelTestCredentials->setText("<i>" + tr("Unable to test") + "</i>");
			ui->labelTestLogin->setText("<i>" + tr("Unable to test") + "</i>");
			break;
	}
}

void SourcesSettingsWindow::save()
{
	QSettings *settings = m_site->settings();

	settings->setValue("name", ui->lineSiteName->text());
	QStringList referers = QStringList() << "none" << "host" << "page" << "image";
	QStringList referers_preview = QStringList() << "" << "none" << "host" << "page" << "image";
	QStringList referers_image = QStringList() << "" << "none" << "host" << "page" << "details" << "image";
	settings->setValue("referer", referers[ui->comboReferer->currentIndex()]);
	settings->setValue("referer_preview", referers_preview[ui->comboRefererPreview->currentIndex()]);
	settings->setValue("referer_image", referers_image[ui->comboRefererImage->currentIndex()]);
	settings->setValue("ignore/always", ui->spinIgnoreAlways->value());
	settings->setValue("ignore/1", ui->spinIgnore1->value());
	settings->setValue("ssl", ui->checkSsl->isChecked());

	settings->setValue("download/imagesperpage", ui->spinImagesPerPage->value());
	settings->setValue("download/simultaneous", ui->spinSimultaneousDownloads->value());
	settings->setValue("download/throttle_details", ui->spinThrottleDetails->value());
	settings->setValue("download/throttle_image", ui->spinThrottleImage->value());
	settings->setValue("download/throttle_page", ui->spinThrottlePage->value());
	settings->setValue("download/throttle_retry", ui->spinThrottleRetry->value());
	settings->setValue("download/throttle_thumbnail", ui->spinThrottleThumbnail->value());

	QStringList sources = QStringList() << "" << "xml" << "json" << "regex" << "rss";
	settings->setValue("sources/usedefault", ui->checkSourcesDefault->isChecked());
	settings->setValue("sources/source_1", sources[qMax(0, ui->comboSources1->currentIndex())]);
	settings->setValue("sources/source_2", sources[qMax(0, ui->comboSources2->currentIndex())]);
	settings->setValue("sources/source_3", sources[qMax(0, ui->comboSources3->currentIndex())]);
	settings->setValue("sources/source_4", sources[qMax(0, ui->comboSources4->currentIndex())]);


	settings->setValue("auth/pseudo", ui->lineAuthPseudo->text());
	settings->setValue("auth/password", ui->lineAuthPassword->text());

	// Login
	QStringList types = QStringList() << "url" << "get" << "post" << "oauth1" << "oauth2";
	settings->setValue("login/type", types[ui->comboLoginType->currentIndex()]);
	settings->setValue("login/get/url", ui->lineLoginGetUrl->text());
	settings->setValue("login/get/pseudo", ui->lineLoginGetPseudo->text());
	settings->setValue("login/get/password", ui->lineLoginGetPassword->text());
	settings->setValue("login/get/cookie", ui->lineLoginGetCookie->text());
	settings->setValue("login/post/url", ui->lineLoginPostUrl->text());
	settings->setValue("login/post/pseudo", ui->lineLoginPostPseudo->text());
	settings->setValue("login/post/password", ui->lineLoginPostPassword->text());
	settings->setValue("login/post/cookie", ui->lineLoginPostCookie->text());
	settings->setValue("login/oauth1/requestTokenUrl", ui->lineLoginOAuth1RequestTokenUrl->text());
	settings->setValue("login/oauth1/authorizeUrl", ui->lineLoginOAuth1AuthorizeUrl->text());
	settings->setValue("login/oauth1/accessTokenUrl", ui->lineLoginOAuth1AccessTokenUrl->text());
	settings->setValue("login/oauth2/requestUrl", ui->lineLoginOAuth2RequestUrl->text());
	settings->setValue("login/oauth2/tokenUrl", ui->lineLoginOAuth2TokenUrl->text());
	settings->setValue("login/oauth2/refreshTokenUrl", ui->lineLoginOAuth2RefreshTokenUrl->text());
	settings->setValue("login/oauth2/scope", ui->lineLoginOAuth2Scope->text());
	settings->setValue("login/maxPage", ui->spinLoginMaxPage->value());

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
	settings->setValue("cookies", cookies);

	// Headers
	QMap<QString, QVariant> headers;
	for (int i = 0; i < ui->tableHeaders->rowCount(); ++i)
	{
		if (ui->tableHeaders->item(i, 0)->text().isEmpty())
			continue;

		headers.insert(ui->tableHeaders->item(i, 0)->text(), ui->tableHeaders->item(i, 1)->text());
	}
	settings->setValue("headers", headers);

	settings->sync();

	m_site->loadConfig();
}
