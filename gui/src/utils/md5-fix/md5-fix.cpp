#include "utils/md5-fix/md5-fix.h"
#include <QCryptographicHash>
#include <QDir>
#include <QDirIterator>
#include <QMessageBox>
#include <QSettings>
#include <ui_md5-fix.h>
#include "functions.h"
#include "helpers.h"
#include "models/profile.h"


md5Fix::md5Fix(Profile *profile, QWidget *parent)
	: QDialog(parent), ui(new Ui::md5Fix), m_profile(profile)
{
	ui->setupUi(this);

	QSettings *settings = profile->getSettings();
	ui->lineFolder->setText(settings->value("Save/path").toString());
	ui->lineFilename->setText(settings->value("Save/filename").toString());
	ui->lineSuffixes->setText(getExternalLogFilesSuffixes(profile->getSettings()).join(", "));
	ui->progressBar->hide();

	resize(size().width(), 0);
}

md5Fix::~md5Fix()
{
	delete ui;
}

void md5Fix::on_buttonCancel_clicked()
{
	emit rejected();
	close();
}

void md5Fix::on_buttonStart_clicked()
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
			QString fileName = file.first;
			QString path = dir.absoluteFilePath(fileName);

			QString md5 = "";
			if (ui->radioForce->isChecked())
			{
				QFile fle(path);
				fle.open(QFile::ReadOnly);
				md5 = QCryptographicHash::hash(fle.readAll(), QCryptographicHash::Md5).toHex();
			}
			else
			{
				QRegExp regx("%([^%]*)%");
				QString reg = QRegExp::escape(ui->lineFilename->text());
				int pos = 0, cur = 0, id = -1;
				while ((pos = regx.indexIn(reg, pos)) != -1)
				{
					pos += 4;
					reg.replace(regx.cap(0), "(.+)");
					if (regx.cap(1) == "md5")
					{ id = cur; }
					cur++;
				}
				QRegExp rx(reg);
				rx.setMinimal(true);
				pos = 0;
				while ((pos = rx.indexIn(fileName, pos)) != -1)
				{
					pos += rx.matchedLength();
					md5 = rx.cap(id + 1);
				}
			}

			if (!md5.isEmpty())
			{
				m_profile->addMd5(md5, path);
				count++;
			}

			ui->progressBar->setValue(ui->progressBar->value() + 1);
		}
	}

	// Hide progress bar
	ui->progressBar->hide();
	ui->progressBar->setValue(0);
	ui->progressBar->setMaximum(0);

	ui->buttonStart->setEnabled(true);

	QMessageBox::information(this, tr("Finished"), tr("%n MD5(s) loaded", "", count));
}
