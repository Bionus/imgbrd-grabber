#include "utils/md5-fix/md5-fix.h"
#include <QCryptographicHash>
#include <QDir>
#include <QDirIterator>
#include <QMessageBox>
#include <QSettings>
#include <ui_md5-fix.h>
#include "functions.h"
#include "helpers.h"
#include "logger.h"
#include "models/profile.h"


Md5Fix::Md5Fix(Profile *profile, QWidget *parent)
	: QDialog(parent), ui(new Ui::Md5Fix), m_profile(profile)
{
	ui->setupUi(this);

	QSettings *settings = profile->getSettings();
	ui->lineFolder->setText(settings->value("Save/path").toString());
	ui->lineFilename->setText(settings->value("Save/filename").toString());
	ui->lineSuffixes->setText(getExternalLogFilesSuffixes(profile->getSettings()).join(", "));
	ui->progressBar->hide();

	m_worker = new Md5FixWorker();
	m_worker->moveToThread(&m_thread);
	connect(&m_thread, &QThread::finished, m_worker, &QObject::deleteLater);
	connect(this, &Md5Fix::startWorker, m_worker, &Md5FixWorker::doWork);
	connect(m_worker, &Md5FixWorker::maximumSet, this, &Md5Fix::workerMaximumSet);
	connect(m_worker, &Md5FixWorker::valueSet, this, &Md5Fix::workerValueSet);
	connect(m_worker, &Md5FixWorker::md5Calculated, this, &Md5Fix::workerMd5Calculated);
	connect(m_worker, &Md5FixWorker::finished, this, &Md5Fix::workerFinished);

	m_thread.start();

	resize(size().width(), 0);
}

Md5Fix::~Md5Fix()
{
	delete ui;

	m_thread.quit();
	m_thread.wait();

	m_worker->deleteLater();
}

void Md5Fix::cancel()
{
	emit rejected();
	close();
}

void Md5Fix::workerMaximumSet(int max)
{
	if (max > 0) {
		ui->progressBar->setValue(0);
		ui->progressBar->setMaximum(max);
		ui->progressBar->show();
	}
}

void Md5Fix::workerValueSet(int value)
{
	ui->progressBar->setValue(value);
}

void Md5Fix::workerMd5Calculated(const QString &md5, const QString &path)
{
	m_profile->addMd5(md5, path);
}

void Md5Fix::workerFinished(int loadedCount)
{
	// Hide progress bar
	ui->progressBar->hide();
	ui->progressBar->setValue(0);
	ui->progressBar->setMaximum(0);

	ui->buttonStart->setEnabled(true);

	m_profile->sync();

	QMessageBox::information(this, tr("Finished"), tr("%n MD5(s) loaded", "", loadedCount));
}

void Md5Fix::start()
{
	ui->buttonStart->setEnabled(false);

	// Check that directory exists
	QString dir = ui->lineFolder->text();
	if (!QDir(dir).exists()) {
		error(this, tr("This folder does not exist."));
		ui->buttonStart->setEnabled(true);
		return;
	}

	// Make sure the input is valid
	bool force = ui->radioForce->isChecked();
	if (!force && !ui->lineFilename->text().contains("%md5%")) {
		error(this, tr("If you want to get the MD5 from the filename, you have to include the %md5% token in it."));
		ui->buttonStart->setEnabled(true);
		return;
	}

	// Suffixes
	QStringList suffixes = ui->lineSuffixes->text().split(',');
	for (QString &suffix : suffixes) {
		suffix = suffix.trimmed();
	}

	emit startWorker(dir, ui->lineFilename->text(), suffixes, force);
}
