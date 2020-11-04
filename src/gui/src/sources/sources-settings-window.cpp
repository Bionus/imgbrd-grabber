#include "sources/sources-settings-window.h"
#include <QCryptographicHash>
#include <QFile>
#include <QInputDialog>
#include <QLineEdit>
#include <QMessageBox>
#include <QNetworkCookie>
#include <QSettings>
#include <ui_sources-settings-window.h>
#include "auth/auth-field.h"
#include "auth/auth-hash-field.h"
#include "auth/field-auth.h"
#include "auth/http-auth.h"
#include "auth/http-basic-auth.h"
#include "auth/oauth2-auth.h"
#include "auth/url-auth.h"
#include "functions.h"
#include "login/http-basic-login.h"
#include "login/http-get-login.h"
#include "login/http-post-login.h"
#include "login/oauth2-login.h"
#include "login/url-login.h"
#include "mixed-settings.h"
#include "models/api/api.h"
#include "models/profile.h"
#include "models/source.h"


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
	auto le = new QLineEdit(std::move(value), parent);
	if (isPassword) {
		le->setEchoMode(QLineEdit::Password);
	}
	return le;
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

	// Download settings
	ui->spinSimultaneousDownloads->setValue(site->setting("download/simultaneous", 10).toInt());
	ui->spinThrottleDetails->setValue(site->setting("download/throttle_details", 0).toInt());
	ui->spinThrottleImage->setValue(site->setting("download/throttle_image", 0).toInt());
	ui->spinThrottlePage->setValue(site->setting("download/throttle_page", 0).toInt());
	ui->spinThrottleRetry->setValue(site->setting("download/throttle_retry", 0).toInt());
	ui->spinThrottleThumbnail->setValue(site->setting("download/throttle_thumbnail", 0).toInt());

	// Source order
	ui->checkSourcesDefault->setChecked(site->setting("sources/usedefault", true).toBool());
	const QStringList defs { "xml", "json", "regex", "rss" };
	QStringList sources { "" };
	QStringList opts { "" };
	for (Api *api : site->getSource()->getApis()) {
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
		{ "oauth2", tr("OAuth 2") }
	};
	static const QMap<QString, QString> fieldLabels
	{
		{ "pseudo", tr("Username") },
		{ "userId", tr("User ID") },
		{ "password", tr("Password") },
		{ "salt", tr("Salt") },
		{ "apiKey", tr("API key") }
	};
	QStringList types;
	QMultiMap<QString, QLineEdit*> fields;
	auto auths = m_site->getSource()->getAuths();
	int activeLoginIndex = 0;
	for (auto it = auths.constBegin(); it != auths.constEnd(); ++it) {
		bool canTestLogin = false;

		const QString type = it.value()->type();
		ui->comboLoginType->addItem(typeNames.contains(type) ? typeNames[type] : type, type);
		if (type == loginType) {
			activeLoginIndex = ui->comboLoginType->count() - 1;
		}

		// Build credential fields
		QWidget *credentialsWidget = new QWidget(this);
		QFormLayout *formLayout = new QFormLayout;
		formLayout->setContentsMargins(0, 0, 0, 0);

		QList<AuthSettingField> settingFields = it.value()->settingFields();
		for (const auto &field : settingFields) {
			const QString id = field.id;
			const QString val = m_site->settings()->value("auth/" + id).toString();

			m_credentialFields[type][id] = createLineEdit(credentialsWidget, val.isEmpty() ? field.def : val, field.isPassword);
			formLayout->addRow(fieldLabels.contains(id) ? fieldLabels[id] : id, m_credentialFields[type][id]);
			fields.insert(id, m_credentialFields[type][id]);
		}

		// TODO(Bionus): factorize login testability creation
		if (type == "oauth2") {
			auto *oauth = dynamic_cast<OAuth2Auth*>(it.value());
			canTestLogin = OAuth2Login(oauth, m_site, nullptr, m_site->settings()).isTestable();
		} else if (type == "http_basic") {
			auto *basicAuth = dynamic_cast<HttpBasicAuth*>(it.value());
			canTestLogin = HttpBasicLogin(basicAuth, m_site, nullptr, m_site->settings()).isTestable();
		} else {
			auto fieldAuth = dynamic_cast<FieldAuth*>(it.value());
			if (type == "url") {
				canTestLogin = UrlLogin(dynamic_cast<UrlAuth*>(fieldAuth), m_site, nullptr, m_site->settings()).isTestable();
			} else if (type == "post") {
				canTestLogin = HttpPostLogin(dynamic_cast<HttpAuth*>(fieldAuth), m_site, nullptr, m_site->settings()).isTestable();
			} else if (type == "get") {
				canTestLogin = HttpGetLogin(dynamic_cast<HttpAuth*>(fieldAuth), m_site, nullptr, m_site->settings()).isTestable();
			}
		}

		credentialsWidget->setLayout(formLayout);
		ui->stackedCredentials->addWidget(credentialsWidget);
		m_canTestLogin.append(canTestLogin);
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
	QMap<QString, QVariant> headers = site->setting("headers").toMap();
	ui->tableHeaders->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
	ui->tableHeaders->setRowCount(headers.count());
	int headerRow = 0;
	for (auto it = headers.constBegin(); it != headers.constEnd(); ++it) {
		ui->tableHeaders->setItem(headerRow, 0, new QTableWidgetItem(it.key()));
		ui->tableHeaders->setItem(headerRow, 1, new QTableWidgetItem(it.value().toString()));
		headerRow++;
	}

	ui->comboLoginType->setCurrentIndex(activeLoginIndex);
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

void SourcesSettingsWindow::deleteSite()
{
	const int reponse = QMessageBox::question(this, tr("Delete a site"), tr("Are you sure you want to delete the site %1?").arg(m_site->name()), QMessageBox::Yes | QMessageBox::No);
	if (reponse == QMessageBox::Yes) {
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
}

void SourcesSettingsWindow::setLoginStatus(const QString &msg)
{
	const QString italic = QStringLiteral("<i>%1</li>").arg(msg);
	ui->labelTestLogin->setText(italic);
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

	m_site->setSetting("download/simultaneous", ui->spinSimultaneousDownloads->value(), 10);
	m_site->setSetting("download/throttle_details", ui->spinThrottleDetails->value(), 0);
	m_site->setSetting("download/throttle_image", ui->spinThrottleImage->value(), 0);
	m_site->setSetting("download/throttle_page", ui->spinThrottlePage->value(), 0);
	m_site->setSetting("download/throttle_retry", ui->spinThrottleRetry->value(), 0);
	m_site->setSetting("download/throttle_thumbnail", ui->spinThrottleThumbnail->value(), 0);

	const QStringList defs { "xml", "json", "regex", "rss" };
	QStringList sources { "" };
	for (Api *api : m_site->getSource()->getApis()) {
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
	QList<QVariant> cookies;
	for (int i = 0; i < ui->tableCookies->rowCount(); ++i) {
		QTableWidgetItem *key = ui->tableCookies->item(i, 0);
		QTableWidgetItem *value = ui->tableCookies->item(i, 1);
		if (key == nullptr || key->text().isEmpty()) {
			continue;
		}

		QNetworkCookie cookie;
		cookie.setName(key->text().toLatin1());
		cookie.setValue(value != nullptr ? value->text().toLatin1() : "");
		cookies.append(cookie.toRawForm());
	}
	m_site->setSetting("cookies", cookies, QList<QVariant>());

	// Headers
	QMap<QString, QVariant> headers;
	for (int i = 0; i < ui->tableHeaders->rowCount(); ++i) {
		QTableWidgetItem *key = ui->tableHeaders->item(i, 0);
		QTableWidgetItem *value = ui->tableHeaders->item(i, 1);
		if (key == nullptr || key->text().isEmpty()) {
			continue;
		}

		headers.insert(key->text(), value != nullptr ? value->text().toLatin1() : "");
	}
	m_site->setSetting("headers", headers, QMap<QString, QVariant>());

	m_site->syncSettings();
	m_site->loadConfig();
}

void SourcesSettingsWindow::save()
{
	saveSettings();
	accept();
}
