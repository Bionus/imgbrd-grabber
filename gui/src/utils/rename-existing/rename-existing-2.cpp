#include "utils/rename-existing/rename-existing-2.h"
#include <QDir>
#include <QFile>
#include <ui_rename-existing-2.h>
#include "functions.h"


RenameExisting2::RenameExisting2(QList<RenameExistingFile> details, QString folder, QWidget *parent)
	: QDialog(parent), ui(new Ui::RenameExisting2), m_details(std::move(details)), m_folder(std::move(folder))
{
	ui->setupUi(this);

	const bool showThumbnails = details.count() < 50;

	int i = 0;
	ui->tableWidget->setRowCount(m_details.size());
	for (const RenameExistingFile &image : qAsConst(m_details))
	{
		if (showThumbnails)
		{
			QLabel *preview = new QLabel();
			preview->setPixmap(QPixmap(image.path).scaledToHeight(50, Qt::SmoothTransformation));
			m_previews.append(preview);
			ui->tableWidget->setCellWidget(i, 0, preview);
		}

		ui->tableWidget->setItem(i, 1, new QTableWidgetItem(image.path.right(image.path.length() - m_folder.length() - 1)));
		if (image.path == image.newPath)
		{
			auto item = new QTableWidgetItem("No change");
			item->setForeground(Qt::red);
			ui->tableWidget->setItem(i, 2, item);
		}
		else
		{ ui->tableWidget->setItem(i, 2, new QTableWidgetItem(image.newPath.right(image.newPath.length() - m_folder.length() - 1))); }

		i++;
	}

	QHeaderView *headerView = ui->tableWidget->horizontalHeader();
	headerView->setSectionResizeMode(QHeaderView::Interactive);
	headerView->resizeSection(0, 50);
	headerView->setSectionResizeMode(1, QHeaderView::Stretch);
	headerView->setSectionResizeMode(2, QHeaderView::Stretch);

	if (!showThumbnails)
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

void RenameExisting2::deleteDir(const QString &path)
{
	if (path == m_folder)
		return;

	QDir directory(path);
	if (directory.entryInfoList(QDir::NoDotAndDotDot | QDir::AllEntries).count() == 0)
	{
		directory.removeRecursively();

		const QString parent = path.left(path.lastIndexOf(QDir::separator()));
		deleteDir(parent);
	}
}

void RenameExisting2::on_buttonOk_clicked()
{
	// Move all images
	for (const RenameExistingFile &image : qAsConst(m_details))
	{
		// Ignore images with no change in path
		if (image.newPath.isEmpty() || image.newPath == image.path)
			continue;

		// Create hierarchy
		const QString path = image.newPath.left(image.newPath.lastIndexOf(QDir::separator()));
		QDir directory(path);
		if (!directory.exists())
		{
			QDir dir;
			if (!dir.mkpath(path))
			{ log(QStringLiteral("Could not create destination directory"), Logger::Error); }
		}

		// Move file
		QFile::rename(image.path, image.newPath);
		for (const QString &child : image.children)
		{
			const QString newPath = QString(child).replace(image.path, image.newPath);
			QFile::rename(child, newPath);
		}

		// Delete old path if necessary
		const QString oldDir = image.path.left(image.path.lastIndexOf(QDir::separator()));
		deleteDir(oldDir);
	}

	// Close
	emit accepted();
	close();
}
