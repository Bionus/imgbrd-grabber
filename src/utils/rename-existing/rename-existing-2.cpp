#include <QDir>
#include <QFile>
#include "rename-existing-2.h"
#include "ui_rename-existing-2.h"
#include "functions.h"



RenameExisting2::RenameExisting2(QList<QPair<QString,QString>> details, QString folder, QWidget *parent) : QDialog(parent), ui(new Ui::RenameExisting2), m_details(details), m_folder(folder)
{
	ui->setupUi(this);

	bool thumbnails = details.count() < 50;

	int i = 0;
	ui->tableWidget->setRowCount(m_details.size());
	for (QPair<QString,QString> image : m_details)
	{
		if (thumbnails)
		{
			QLabel *preview = new QLabel();
			preview->setPixmap(QPixmap(image.first).scaledToHeight(50, Qt::SmoothTransformation));
			m_previews.append(preview);
			ui->tableWidget->setCellWidget(i, 0, preview);
		}

		ui->tableWidget->setItem(i, 1, new QTableWidgetItem(image.first.right(image.first.length() - m_folder.length() - 1)));
		ui->tableWidget->setItem(i, 2, new QTableWidgetItem(image.second.right(image.second.length() - m_folder.length() - 1)));
		i++;
	}

	QHeaderView *headerView = ui->tableWidget->horizontalHeader();
	headerView->setSectionResizeMode(QHeaderView::Interactive);
	headerView->resizeSection(0, 50);
	headerView->setSectionResizeMode(1, QHeaderView::Stretch);
	headerView->setSectionResizeMode(2, QHeaderView::Stretch);

	if (!thumbnails)
		ui->tableWidget->removeColumn(0);
}

RenameExisting2::~RenameExisting2()
{
	delete ui;
}

void RenameExisting2::on_buttonCancel_clicked()
{
	emit rejected();
	close();
}

void RenameExisting2::deleteDir(QString path)
{
	if (path == m_folder)
		return;

	QDir directory(path);
	if (directory.entryInfoList(QDir::NoDotAndDotDot | QDir::AllEntries).count() == 0)
	{
		directory.removeRecursively();

		QString parent = path.left(path.lastIndexOf(QDir::toNativeSeparators("/")));
		deleteDir(parent);
	}
}

void RenameExisting2::on_buttonOk_clicked()
{
	// Move all images
	for (QPair<QString,QString> images : m_details)
	{
		// Create hierarchy
		QString path = images.second.left(images.second.lastIndexOf(QDir::toNativeSeparators("/")));
		QDir directory(path);
		if (!directory.exists())
		{
			QDir dir;
			if (!dir.mkpath(path))
			{ log("Could not create destination directory", Log::Error); }
		}

		// Move file
		QFile(images.first).rename(images.second);

		// Delete old path if necessary
		QString oldDir = images.first.left(images.first.lastIndexOf(QDir::toNativeSeparators("/")));
		deleteDir(oldDir);
	}

	// Close
	emit accepted();
	close();
}
