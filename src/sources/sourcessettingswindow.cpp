#include <QMessageBox>
#include <QInputDialog>
#include <QFile>
#include <QCryptographicHash>
#include <QSettings>
#include <QNetworkCookie>
#include "sourcessettingswindow.h"
#include "ui_sourcessettingswindow.h"
#include "functions.h"

SourcesSettingsWindow::SourcesSettingsWindow(Site *site, QWidget *parent) : QDialog(parent), ui(new Ui::SourcesSettingsWindow), m_site(site)
{
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
	QStringList sources = QStringList() << "xml" << "json" << "regex" << "rss";
	ui->comboSources1->setCurrentIndex(sources.indexOf(settings->value("sources/source_1", global.value("source_1", sources[0]).toString()).toString()));
	ui->comboSources2->setCurrentIndex(sources.indexOf(settings->value("sources/source_2", global.value("source_2", sources[1]).toString()).toString()));
	ui->comboSources3->setCurrentIndex(sources.indexOf(settings->value("sources/source_3", global.value("source_3", sources[2]).toString()).toString()));
	ui->comboSources4->setCurrentIndex(sources.indexOf(settings->value("sources/source_4", global.value("source_4", sources[3]).toString()).toString()));

	// Credentials
	ui->lineAuthPseudo->setText(settings->value("auth/pseudo", "").toString());
	ui->lineAuthPassword->setText(settings->value("auth/password", "").toString());

	// Login
	ui->checkLoginParameter->setChecked(settings->value("login/parameter", true).toBool());
	QStringList methods = QStringList() << "get" << "post";
	ui->comboLoginMethod->setCurrentIndex(methods.indexOf(settings->value("login/method", "post").toString()));
	ui->lineLoginUrl->setText(settings->value("login/url", "").toString());
	ui->lineLoginPseudo->setText(settings->value("login/pseudo", "").toString());
	ui->lineLoginPassword->setText(settings->value("login/password", "").toString());
	ui->lineLoginCookie->setText(settings->value("login/cookie", "").toString());

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

void SourcesSettingsWindow::on_buttonAuthHash_clicked()
{
	QString password = QInputDialog::getText(this, tr("Hasher un mot de passe"), tr("Veuillez entrer votre mot de passe ci-dessous.<br/>Il sera ensuite hashé en utilisant le format \"%1\".").arg(m_site->value("PasswordSalt")));
	if (!password.isEmpty())
	{ ui->lineAuthPassword->setText(QCryptographicHash::hash(m_site->value("PasswordSalt").replace("%password%", password).toUtf8(), QCryptographicHash::Sha1).toHex()); }
}

void SourcesSettingsWindow::deleteSite()
{
	int reponse = QMessageBox::question(this, tr("Grabber - Supprimer un site"), tr("Êtes-vous sûr de vouloir supprimer le site %1 ?").arg(m_site->name()), QMessageBox::Yes | QMessageBox::No);
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
		case Site::LoginSuccess:
			ui->labelTestCredentials->setText("<i>" + tr("Succès !") + "</i>");
			ui->labelTestLogin->setText("<i>" + tr("Succès !") + "</i>");
			break;

		case Site::LoginError:
			ui->labelTestCredentials->setText("<i>" + tr("Échec") + "</i>");
			ui->labelTestLogin->setText("<i>" + tr("Échec") + "</i>");
			break;

		case Site::LoginNoLogin:
			ui->labelTestCredentials->setText("<i>" + tr("Impossible de tester") + "</i>");
			ui->labelTestLogin->setText("<i>" + tr("Impossible de tester") + "</i>");
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

	QStringList sources = QStringList() << "xml" << "json" << "regex" << "rss";
	settings->setValue("sources/usedefault", ui->checkSourcesDefault->isChecked());
	settings->setValue("sources/source_1", sources[ui->comboSources1->currentIndex()]);
	settings->setValue("sources/source_2", sources[ui->comboSources2->currentIndex()]);
	settings->setValue("sources/source_3", sources[ui->comboSources3->currentIndex()]);
	settings->setValue("sources/source_4", sources[ui->comboSources4->currentIndex()]);

	settings->setValue("auth/pseudo", ui->lineAuthPseudo->text());
	settings->setValue("auth/password", ui->lineAuthPassword->text());

	// Login
	QStringList methods = QStringList() << "get" << "post";
	settings->setValue("login/parameter", ui->checkLoginParameter->isChecked());
	settings->setValue("login/method", methods[ui->comboLoginMethod->currentIndex()]);
	settings->setValue("login/url", ui->lineLoginUrl->text());
	settings->setValue("login/pseudo", ui->lineLoginPseudo->text());
	settings->setValue("login/password", ui->lineLoginPassword->text());
	settings->setValue("login/cookie", ui->lineLoginCookie->text());

	// Cookies
	QList<QVariant> cookies;
	for (int i = 0; i < ui->tableCookies->rowCount(); ++i)
	{
		QNetworkCookie cookie;
		cookie.setName(ui->tableCookies->item(i, 0)->text().toLatin1());
		cookie.setValue(ui->tableCookies->item(i, 1)->text().toLatin1());
		cookies.append(cookie.toRawForm());
	}
	settings->setValue("cookies", cookies);

	settings->sync();

	m_site->load();
}
