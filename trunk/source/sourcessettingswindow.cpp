#include <QMessageBox>
#include <QInputDialog>
#include <QFile>
#include <QCryptographicHash>
#include <QSettings>
#include "sourcessettingswindow.h"
#include "ui_sourcessettingswindow.h"
#include "functions.h"

SourcesSettingsWindow::SourcesSettingsWindow(Site *site, QWidget *parent) : QDialog(parent), ui(new Ui::SourcesSettingsWindow), m_site(site)
{
	ui->setupUi(this);

	QSettings settings(savePath("sites/"+m_site->type()+"/"+m_site->name()+"/settings.ini"), QSettings::IniFormat);
	QSettings global(savePath("settings.ini"), QSettings::IniFormat);

	QStringList referers = QStringList() << "none" << "host" << "page" << "image";
	ui->comboReferer->setCurrentIndex(referers.indexOf(settings.value("referer", "none").toString()));

	ui->checkSourcesDefault->setChecked(settings.value("sources/usedefault", true).toBool());
	QStringList sources = QStringList() << "xml" << "json" << "regex" << "rss";
	ui->comboSources1->setCurrentIndex(sources.indexOf(settings.value("sources/source_1", global.value("source_1", sources[0]).toString()).toString()));
	ui->comboSources2->setCurrentIndex(sources.indexOf(settings.value("sources/source_2", global.value("source_2", sources[1]).toString()).toString()));
	ui->comboSources3->setCurrentIndex(sources.indexOf(settings.value("sources/source_3", global.value("source_3", sources[2]).toString()).toString()));
	ui->comboSources4->setCurrentIndex(sources.indexOf(settings.value("sources/source_4", global.value("source_4", sources[3]).toString()).toString()));

	ui->lineAuthPseudo->setText(settings.value("auth/pseudo", "").toString());
	ui->lineAuthPassword->setText(settings.value("auth/password", "").toString());

	ui->checkLoginParameter->setChecked(settings.value("login/parameter", true).toBool());
	QStringList methods = QStringList() << "get" << "post";
	ui->comboLoginMethod->setCurrentIndex(methods.indexOf(settings.value("login/method", "post").toString()));
	ui->lineLoginUrl->setText(settings.value("login/url", "").toString());
	ui->lineLoginPseudo->setText(settings.value("login/pseudo", "").toString());
	ui->lineLoginPassword->setText(settings.value("login/password", "").toString());

	connect(this, SIGNAL(accepted()), this, SLOT(save()));
}

SourcesSettingsWindow::~SourcesSettingsWindow()
{
	delete ui;
}

void SourcesSettingsWindow::on_buttonAuthHash_clicked()
{
	QString password = QInputDialog::getText(this, tr("Hasher un mot de passe"), tr("Veuillez entrer votre mot de passe, dans le format adapté.<br/>Par exemple, pour danbooru, le format est \"%1\" (sans les guillemets).").arg("choujin-steiner--%1--").arg(tr("VOTRE_MOT_DE_PASSE")), QLineEdit::Password);
	if (!password.isEmpty())
	{ ui->lineAuthPassword->setText(QCryptographicHash::hash(password.toUtf8(), QCryptographicHash::Sha1).toHex()); }
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
		QStringList stes = sites.split("\r\n");
		stes.removeAll(m_site->name());
		f.open(QIODevice::WriteOnly);
			f.write(stes.join("\r\n").toAscii());
		f.close();
		close();
		emit siteDeleted(m_site->name());
	}
}

void SourcesSettingsWindow::save()
{
	QSettings settings(savePath("sites/"+m_site->type()+"/"+m_site->name()+"/settings.ini"), QSettings::IniFormat);

	QStringList referers = QStringList() << "none" << "host" << "page" << "image";
	settings.setValue("referer", referers[ui->comboReferer->currentIndex()]);

	settings.setValue("sources/usedefault", ui->checkSourcesDefault->isChecked());
	QStringList sources = QStringList() << "xml" << "json" << "regex" << "rss";
	settings.setValue("sources/source_1", sources[ui->comboSources1->currentIndex()]);
	settings.setValue("sources/source_2", sources[ui->comboSources2->currentIndex()]);
	settings.setValue("sources/source_3", sources[ui->comboSources3->currentIndex()]);
	settings.setValue("sources/source_4", sources[ui->comboSources4->currentIndex()]);

	settings.setValue("auth/pseudo", ui->lineAuthPseudo->text());
	settings.setValue("auth/password", ui->lineAuthPassword->text());

	settings.setValue("login/parameter", ui->checkLoginParameter->isChecked());
	QStringList methods = QStringList() << "get" << "post";
	settings.setValue("login/method", methods[ui->comboLoginMethod->currentIndex()]);
	settings.setValue("login/url", ui->lineLoginUrl->text());
	settings.setValue("login/pseudo", ui->lineLoginPseudo->text());
	settings.setValue("login/password", ui->lineLoginPassword->text());

	settings.sync();
}
