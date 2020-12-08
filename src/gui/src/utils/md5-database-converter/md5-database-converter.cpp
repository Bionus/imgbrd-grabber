#include "utils/md5-database-converter/md5-database-converter.h"
#include <QMessageBox>
#include <ui_md5-database-converter.h>
#include "models/md5-database/md5-database.h"
#include "models/md5-database/md5-database-text.h"
#include "models/md5-database/md5-database-sqlite.h"
#include "models/profile.h"


Md5DatabaseConverter::Md5DatabaseConverter(Profile *profile, QWidget *parent)
	: QDialog(parent), ui(new Ui::Md5DatabaseConverter), m_profile(profile)
{
	ui->setupUi(this);

	Md5Database *md5s = m_profile->md5Database();
	auto *md5sText = dynamic_cast<Md5DatabaseText*>(md5s);
	if (md5sText == nullptr) {
		ui->labelError->setText(tr("You are not using a TXT MD5 database."));
		ui->buttonStart->setEnabled(false);
	} else {
		ui->labelError->hide();
	}

	m_dbSqlite = new Md5DatabaseSqlite(m_profile->getPath() + "/md5s.sqlite", m_profile->getSettings());
	m_dbText = md5sText;

	ui->progressBar->hide();

	resize(size().width(), 0);
}

Md5DatabaseConverter::~Md5DatabaseConverter()
{
	delete ui;
}

void Md5DatabaseConverter::cancel()
{
	emit rejected();
	close();
	deleteLater();
}

void Md5DatabaseConverter::start()
{
	ui->buttonStart->setEnabled(false);

	// Show progress bar
	ui->progressBar->setValue(0);
	ui->progressBar->setMinimum(0);
	ui->progressBar->setMaximum(0);
	ui->progressBar->show();

	// Migrate database
	const auto &md5s = m_dbText->getAll();
	m_dbSqlite->setMd5s(md5s);
	m_dbSqlite->sync();

	// Hide progress bar
	ui->progressBar->hide();
	resize(size().width(), 0);

	QMessageBox::information(this, tr("Finished"), tr("%n md5(s) converted (out of %1)", "", m_dbSqlite->count()).arg(m_dbText->count()));
}
