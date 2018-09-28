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

	resize(size().width(), 0);
}

Md5Fix::~Md5Fix()
{
	delete ui;
}

void Md5Fix::on_buttonCancel_clicked()
{
	emit rejected();
	close();
}

void Md5Fix::on_buttonStart_clicked()
{
	ui->buttonStart->setEnabled(false);

	// Check that directory exists
	QDir dir(ui->lineFolder->text());
	if (!dir.exists())
	{
		error(this, tr("This folder does not exist."));
		ui->buttonStart->setEnabled(true);
		return;
	}

	// Make sure the input is valid
	if (!ui->radioForce->isChecked() && !ui->lineFilename->text().contains("%md5%"))
	{
		error(this, tr("If you want to get the MD5 from the filename, you have to include the %md5% token in it."));
		ui->buttonStart->setEnabled(true);
		return;
	}

	// Suffixes
	QStringList suffixes = ui->lineSuffixes->text().split(',');
	for (QString &suffix : suffixes)
		suffix = suffix.trimmed();

	// Get all files from the destination directory
	auto files = listFilesFromDirectory(dir, suffixes);

	int count = 0;
	if (files.count() > 0)
	{
		// Show progress bar
		ui->progressBar->setValue(0);
		ui->progressBar->setMaximum(files.size());
		ui->progressBar->show();

		// Parse all files
		for (const auto &file : files)
		{
			const QString fileName = file.first;
			const QString path = dir.absoluteFilePath(fileName);

			QString md5;
			if (ui->radioForce->isChecked())
			{
				QFile fle(path);
				fle.open(QFile::ReadOnly);
				md5 = QCryptographicHash::hash(fle.readAll(), QCryptographicHash::Md5).toHex();
			}
			else
			{
				QRegularExpression regx("%([^%]*)%");
				QString reg = QRegExp::escape(ui->lineFilename->text());
				auto matches = regx.globalMatch(ui->lineFilename->text());
				while (matches.hasNext())
				{
					const auto match = matches.next();
					const bool isMd5 = match.captured(1) == QLatin1String("md5");
					reg.replace(match.captured(0), isMd5 ? QStringLiteral("(?<md5>.+?)") : QStringLiteral("(.+?)"));
				}

				QRegularExpression rx(reg);
				const auto match = rx.match(fileName);
				if (match.hasMatch())
				{ md5 = match.captured("md5"); }
				else
				{ log(QStringLiteral("Unable to detect MD5 file `%1`").arg(path), Logger::Warning); }
			}

			if (!md5.isEmpty())
			{
				static QRegularExpression rxMd5("^[0-9A-F]{32,}$", QRegularExpression::CaseInsensitiveOption);
				if (rxMd5.match(md5).hasMatch())
				{
					m_profile->addMd5(md5, path);
					count++;
				}
				else
				{ log(QStringLiteral("Invalid detected MD5 '%1' for file `%2`").arg(md5, path), Logger::Warning); }
			}

			ui->progressBar->setValue(ui->progressBar->value() + 1);
		}
	}

	// Hide progress bar
	ui->progressBar->hide();
	ui->progressBar->setValue(0);
	ui->progressBar->setMaximum(0);

	ui->buttonStart->setEnabled(true);

	m_profile->sync();

	QMessageBox::information(this, tr("Finished"), tr("%n MD5(s) loaded", "", count));
}
