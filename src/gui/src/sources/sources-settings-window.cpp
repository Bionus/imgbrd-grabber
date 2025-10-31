#include "sources/sources-settings-window.h"
#include <QCryptographicHash>
#include <QFileDialog>
#include <QLineEdit>
#include <QMessageBox>
#include <QNetworkCookie>
#include <QSettings>
#include <ui_sources-settings-window.h>
#include "auth/auth.h"
#include "cookies.h"
#include "functions.h"
#include "mixed-settings.h"
#include "models/api/api.h"
#include "models/profile.h"
#include "models/source-engine.h"
#include "network/persistent-cookie-jar.h"
#include "login/login-factory.h"
#if !defined(USE_WEBENGINE)
	#include "webview-window.h"
#endif


void setSource(QComboBox *combo, const QStringList &opts, const QStringList &vals, const QStringList &defs, Site *site, QSettings *settings, int index)
{
	const QString &def = defs[index];
	const QString &global = settings->value("source_" + QString::number(index + 1), def).toString();
	const QString &local = site->setting("sources/source_" + QString::number(index + 1), global).toString();

	combo->clear();
	combo->addItems(opts);
	combo->setCurrentIndex(qMax(0, vals.indexOf(local)));
}

QLineEdit *createLineEdit(QWidget *parent, QString value, bool isPassword)
{
	auto *lineEdit = new QLineEdit(std::move(value), parent);
	if (isPassword) {
		lineEdit->setEchoMode(QLineEdit::Password);
	}
	return lineEdit;
}

SourcesSettingsWindow::SourcesSettingsWindow(Profile *profile, Site *site, QWidget *parent)
	: QDialog(parent), ui(new Ui::SourcesSettingsWindow), m_site(site), m_globalSettings(profile->getSettings())
{
	setAttribute(Qt::WA_DeleteOnClose);
	ui->setupUi(this);

	// Refferers
	ui->lineSiteName->setText(site->setting("name", m_site->url()).toString());
	const QStringList referers { "none", "host", "page", "image" };
	const QStringList referers_preview { "", "none", "host", "page", "image" };
	const QStringList referers_image { "", "none", "host", "page", "details", "image" };
	ui->comboReferer->setCurrentIndex(referers.indexOf(site->setting("referer", "none").toString()));
	ui->comboRefererPreview->setCurrentIndex(referers_preview.indexOf(site->setting("referer_preview", "").toString()));
	ui->comboRefererImage->setCurrentIndex(referers_image.indexOf(site->setting("referer_image", "").toString()));
	ui->spinIgnoreAlways->setValue(site->setting("ignore/always", 0).toInt());
	ui->spinIgnore1->setValue(site->setting("ignore/1", 0).toInt());
	ui->checkSsl->setChecked(site->setting("ssl", false).toBool());

	// Search settings
	ui->lineAddedTags->setText(site->setting("added_tags").toString());
	ui->lineRemovedTags->setText(site->setting("removed_tags").toString());

	// Download settings
	ui->spinSimultaneousDownloads->setValue(site->setting("download/simultaneous", 10).toInt());
	ui->spinThrottleDetails->setValue(site->setting("download/throttle_details", 1).toInt());
	ui->spinThrottleImage->setValue(site->setting("download/throttle_image", 1).toInt());
	ui->spinThrottlePage->setValue(site->setting("download/throttle_page", 1).toInt());
	ui->spinThrottleRetry->setValue(site->setting("download/throttle_retry", 60).toInt());
	ui->spinThrottleThumbnail->setValue(site->setting("download/throttle_thumbnail", 0).toInt());

	// Source order
	ui->checkSourcesDefault->setChecked(site->setting("sources/usedefault", true).toBool());
	const QStringList defs { "xml", "json", "regex", "rss" };
	QStringList sources { "" };
	QStringList opts { "" };
	for (Api *api : site->getSourceEngine()->getApis()) {
		const QString name = api->getName().toLower();
		sources.append(name == "html" ? "regex" : name);
		opts.append(api->getName());
	}
	setSource(ui->comboSources1, opts, sources, defs, site, m_globalSettings, 0);
	setSource(ui->comboSources2, opts, sources, defs, site, m_globalSettings, 1);
	setSource(ui->comboSources3, opts, sources, defs, site, m_globalSettings, 2);
	setSource(ui->comboSources4, opts, sources, defs, site, m_globalSettings, 3);

	// Login
	const QString loginType = site->setting("login/type", "url").toString();
	static const QMap<QString, QString> typeNames
	{
		{ "url", tr("Through URL") },
		{ "http_basic", tr("HTTP Basic") },
		{ "get", tr("GET") },
		{ "post", tr("POST") },
		{ "oauth1", tr("OAuth 1") },
		{ "oauth2_password", tr("OAuth 2 (password)") },
		{ "oauth2_password_json", tr("OAuth 2 (JSON password)") },
		{ "oauth2_client_credentials", tr("OAuth 2 (client credentials)") },
		{ "oauth2_client_credentials_header", tr("OAuth 2 (client credentials header)") },
		{ "oauth2_refresh_token", tr("OAuth 2 (refresh token)") },
		{ "oauth2_authorization_code", tr("OAuth 2 (authorization code)") },
		{ "oauth2_pkce", tr("OAuth 2 (PKCE)") }
	};
	static const QMap<QString, QString> fieldLabels
	{
		{ "pseudo", tr("Username") },
		{ "userId", tr("User ID") },
		{ "password", tr("Password") },
		{ "salt", tr("Salt") },
		{ "apiKey", tr("API key") },
		{ "consumerKey", tr("Consumer key") },
		{ "consumerSecret", tr("Consumer secret") },
		{ "accessToken", tr("Access token") },
		{ "refreshToken", tr("Refresh token") }
	};
	QMultiMap<QString, QLineEdit*> fields;
	auto auths = m_site->getSourceEngine()->getAuths();
	int activeLoginIndex = 0;
	for (auto it = auths.constBegin(); it != auths.constEnd(); ++it) {
		const QString &id = it.key();
		const QString type = it.value()->type();
		const QString name = it.value()->name();
		ui->comboLoginType->addItem(typeNames.contains(name) ? typeNames[name] : name, id);
		if (id == loginType) {
			activeLoginIndex = ui->comboLoginType->count() - 1;
		}

		// Build credential fields
		auto *credentialsWidget = new QWidget(this);
		auto *formLayout = new QFormLayout;
		formLayout->setContentsMargins(0, 0, 0, 0);

		QList<AuthSettingField> settingFields = it.value()->settingFields();
		for (const auto &field : settingFields) {
			const QString id = field.id;
			const QString val = m_site->settings()->value("auth/" + id).toString();

			m_credentialFields[type][id] = createLineEdit(credentialsWidget, val.isEmpty() ? field.def : val, field.isPassword);
			formLayout->addRow(fieldLabels.contains(id) ? fieldLabels[id] : id, m_credentialFields[type][id]);
			fields.insert(id, m_credentialFields[type][id]);
		}

		credentialsWidget->setLayout(formLayout);
		ui->stackedCredentials->addWidget(credentialsWidget);

		// Enable/disable the "Test" button
		Login *login = LoginFactory::build(m_site, it.value(), nullptr);
		m_canTestLogin.append(login->isTestable());
		login->deleteLater();
	}
	for (const QString key : fields.keys()) {
		const QList<QLineEdit*> l = fields.values(key);
		for (int i = 0; i < l.count() - 1; ++i) {
			for (int j = i + 1; j < l.count(); ++j) {
				connect(l[i], &QLineEdit::textChanged, l[j], &QLineEdit::setText);
				connect(l[j], &QLineEdit::textChanged, l[i], &QLineEdit::setText);
			}
		}
	}

	// Cookies
	QList<QNetworkCookie> cookies = site->cookies();
	ui->tableCookies->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
	ui->tableCookies->setRowCount(cookies.count());
	int cookieRow = 0;
	for (const QNetworkCookie &cookie : site->cookies()) {
		ui->tableCookies->setItem(cookieRow, 0, new QTableWidgetItem(QString(cookie.name())));
		ui->tableCookies->setItem(cookieRow, 1, new QTableWidgetItem(QString(cookie.value())));
		cookieRow++;
	}

	// Headers
	QMap<QString, QString> headers = site->settingsHeaders();
	ui->tableHeaders->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
	ui->tableHeaders->setRowCount(headers.count());
	int headerRow = 0;
	for (auto it = headers.constBegin(); it != headers.constEnd(); ++it) {
		ui->tableHeaders->setItem(headerRow, 0, new QTableWidgetItem(it.key()));
		ui->tableHeaders->setItem(headerRow, 1, new QTableWidgetItem(it.value()));
		headerRow++;
	}

	if (ui->comboLoginType->currentIndex() != activeLoginIndex) {
		ui->comboLoginType->setCurrentIndex(activeLoginIndex);
	} else {
		setLoginType(activeLoginIndex);
	}

	#if !defined(USE_WEBENGINE)
		ui->buttonOpenInWebView->setDisabled(true);
	#endif
}

SourcesSettingsWindow::~SourcesSettingsWindow()
{
	delete ui;
}

void SourcesSettingsWindow::addCookie()
{
	ui->tableCookies->setRowCount(ui->tableCookies->rowCount() + 1);
}

void SourcesSettingsWindow::importCookies()
{
	const QString path = QFileDialog::getOpenFileName(this, tr("Import cookies"), "", tr("Cookie files (*.txt *.json)"));
	if (path.isEmpty()) {
		return;
	}

	const QList<QNetworkCookie> cookies = loadCookiesFromFile(path);
	for (const QNetworkCookie &cookie : cookies) {
		const int row = ui->tableCookies->rowCount();
		ui->tableCookies->setRowCount(ui->tableCookies->rowCount() + 1);
		ui->tableCookies->setItem(row, 0, new QTableWidgetItem(QString(cookie.name())));
		ui->tableCookies->setItem(row, 1, new QTableWidgetItem(QString(cookie.value())));
	}

	QMessageBox::information(this, QObject::tr("Success"), tr("%n cookie(s) imported.", nullptr, cookies.count()));
}

void SourcesSettingsWindow::addHeader()
{
	ui->tableHeaders->setRowCount(ui->tableHeaders->rowCount() + 1);
}

void SourcesSettingsWindow::deleteSite()
{
	const int response = QMessageBox::question(this, tr("Delete a site"), tr("Are you sure you want to delete the site %1?").arg(m_site->name()), QMessageBox::Yes | QMessageBox::No);
	if (response == QMessageBox::Yes) {
		emit siteDeleted(m_site);
	}
}

void SourcesSettingsWindow::setLoginType(int index)
{
	ui->stackedCredentials->setCurrentIndex(index);

	if (index < m_canTestLogin.count()) {
		ui->widgetTestLogin->setVisible(m_canTestLogin[index]);
	}
}

void SourcesSettingsWindow::testLogin()
{
	saveSettings();

	setLoginStatus(tr("Connection..."));

	connect(m_site, &Site::loggedIn, this, &SourcesSettingsWindow::loginTested);
	m_site->login(true);
}

void SourcesSettingsWindow::loginTested(Site *site, Site::LoginResult result)
{
	Q_UNUSED(site)

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

	updateFields();
}

void SourcesSettingsWindow::setLoginStatus(const QString &msg)
{
	const QString italic = QStringLiteral("<i>%1</li>").arg(msg);
	ui->labelTestLogin->setText(italic);
}

void SourcesSettingsWindow::openInWebView()
{
	#if defined(USE_WEBENGINE)
		auto *window = new WebViewWindow(m_site, this);
		window->show();
	#endif
}

void SourcesSettingsWindow::clearOtherCookies()
{
	auto *cookieJar = m_site->cookieJar();
	cookieJar->clear();
	cookieJar->save();
}

void SourcesSettingsWindow::updateFields()
{
	for (auto it = m_credentialFields.begin(); it != m_credentialFields.end(); ++it) {
		for (auto jt = it.value().begin(); jt != it.value().end(); ++jt) {
			const QString &type = it.key();
			const QString &id = jt.key();

			const QString val = m_site->settings()->value("auth/" + id).toString();
			if (!val.isEmpty()) {
				m_credentialFields[type][id]->setText(val);
			}
		}
	}
}

void SourcesSettingsWindow::saveSettings()
{
	m_site->setSetting("name", ui->lineSiteName->text(), m_site->url());
	const QStringList referers { "none", "host", "page", "image" };
	const QStringList referers_preview { "", "none", "host", "page", "image" };
	const QStringList referers_image { "", "none", "host", "page", "details", "image" };
	m_site->setSetting("referer", referers[ui->comboReferer->currentIndex()], "none");
	m_site->setSetting("referer_preview", referers_preview[ui->comboRefererPreview->currentIndex()], "");
	m_site->setSetting("referer_image", referers_image[ui->comboRefererImage->currentIndex()], "");
	m_site->setSetting("ignore/always", ui->spinIgnoreAlways->value(), 0);
	m_site->setSetting("ignore/1", ui->spinIgnore1->value(), 0);
	m_site->setSetting("ssl", ui->checkSsl->isChecked(), false);

	// Search settings
	m_site->setSetting("added_tags", ui->lineAddedTags->text(), "");
	m_site->setSetting("removed_tags", ui->lineRemovedTags->text(), "");

	m_site->setSetting("download/simultaneous", ui->spinSimultaneousDownloads->value(), 10);
	m_site->setSetting("download/throttle_details", ui->spinThrottleDetails->value(), 0);
	m_site->setSetting("download/throttle_image", ui->spinThrottleImage->value(), 0);
	m_site->setSetting("download/throttle_page", ui->spinThrottlePage->value(), 0);
	m_site->setSetting("download/throttle_retry", ui->spinThrottleRetry->value(), 60);
	m_site->setSetting("download/throttle_thumbnail", ui->spinThrottleThumbnail->value(), 0);

	const QStringList defs { "xml", "json", "regex", "rss" };
	QStringList sources { "" };
	for (Api *api : m_site->getSourceEngine()->getApis()) {
		const QString name = api->getName().toLower();
		sources.append(name == "html" ? "regex" : name);
	}
	QStringList chosen = QStringList()
		<< sources[ui->comboSources1->currentIndex()]
		<< sources[ui->comboSources2->currentIndex()]
		<< sources[ui->comboSources3->currentIndex()]
		<< sources[ui->comboSources4->currentIndex()];
	m_site->setSetting("sources/usedefault", ui->checkSourcesDefault->isChecked(), true);
	m_site->setSetting("sources/source_1", chosen[0], m_globalSettings->value("source_1", defs[0]).toString());
	m_site->setSetting("sources/source_2", chosen[1], m_globalSettings->value("source_2", defs[1]).toString());
	m_site->setSetting("sources/source_3", chosen[2], m_globalSettings->value("source_3", defs[2]).toString());
	m_site->setSetting("sources/source_4", chosen[3], m_globalSettings->value("source_4", defs[3]).toString());

	// Ensure at least one source is selected
	bool allEmpty = true;
	for (const QString &chos : qAsConst(chosen)) {
		if (!chos.isEmpty()) {
			allEmpty = false;
		}
	}
	if (allEmpty) {
		QMessageBox::critical(this, tr("Error"), tr("You should at least select one source"));
		return;
	}

	// Login
	m_site->setSetting("login/type", ui->comboLoginType->currentData(), "url");
	for (auto itt = m_credentialFields.begin(); itt != m_credentialFields.end(); ++itt) {
		for (auto itf = itt.value().begin(); itf != itt.value().end(); ++itf) {
			m_site->setSetting("auth/" + itf.key(), itf.value()->text(), "");
		}
	}

	// Cookies
	QStringList cookies;
	for (int i = 0; i < ui->tableCookies->rowCount(); ++i) {
		QTableWidgetItem *key = ui->tableCookies->item(i, 0);
		QTableWidgetItem *value = ui->tableCookies->item(i, 1);
		if (key == nullptr || key->text().isEmpty()) {
			continue;
		}

		QNetworkCookie cookie;
		cookie.setName(key->text().toLatin1());
		cookie.setValue(value != nullptr ? value->text().toLatin1() : QByteArray());
		cookies.append(cookie.toRawForm());
	}
	m_site->setSetting("cookies", cookies, QStringList());

	// Headers
	MixedSettings *settings = m_site->settings();
	settings->beginGroup("Headers");
	for (int i = 0; i < ui->tableHeaders->rowCount(); ++i) {
		QTableWidgetItem *key = ui->tableHeaders->item(i, 0);
		QTableWidgetItem *value = ui->tableHeaders->item(i, 1);
		if (key == nullptr || key->text().isEmpty()) {
			continue;
		}

		settings->setValue(key->text(), value != nullptr ? value->text() : "");
	}
	settings->endGroup();

	m_site->syncSettings();
	m_site->loadConfig();
}

void SourcesSettingsWindow::save()
{
	saveSettings();
	accept();
}
