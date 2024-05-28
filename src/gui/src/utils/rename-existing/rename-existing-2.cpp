#include "utils/rename-existing/rename-existing-2.h"
#include <QDir>
#include <QFile>
#include <ui_rename-existing-2.h>
#include "functions.h"
#include "logger.h"
#include "rename-existing-table-model.h"


RenameExisting2::RenameExisting2(QList<RenameExistingFile> details, QString folder, QWidget *parent)
	: QDialog(parent), ui(new Ui::RenameExisting2), m_details(std::move(details)), m_folder(std::move(folder))
{
	ui->setupUi(this);

	m_model = new RenameExistingTableModel(m_details, m_folder, this);
	ui->tableView->setModel(m_model);

	QHeaderView *verticalHeader = ui->tableView->verticalHeader();
	verticalHeader->setSectionResizeMode(QHeaderView::Fixed);
	verticalHeader->setDefaultSectionSize(50);

	QHeaderView *headerView = ui->tableView->horizontalHeader();
	headerView->setSectionResizeMode(QHeaderView::Interactive);
	headerView->resizeSection(0, 50);
	headerView->setSectionResizeMode(1, QHeaderView::Stretch);
	headerView->setSectionResizeMode(2, QHeaderView::Stretch);
}

RenameExisting2::~RenameExisting2()
{
	delete ui;
}

void RenameExisting2::cancel()
{
	emit rejected();
	close();
}

void RenameExisting2::deleteDir(const QString &path)
{
	if (path == m_folder) {
		return;
	}

	QDir directory(path);
	if (directory.entryInfoList(QDir::NoDotAndDotDot | QDir::AllEntries).count() == 0) {
		directory.removeRecursively();

		const QString parent = path.left(path.lastIndexOf(QDir::separator()));
		deleteDir(parent);
	}
}

void RenameExisting2::nextStep()
{
	// Move all images
	for (const RenameExistingFile &image : qAsConst(m_details)) {
		// Ignore images with no change in path
		if (image.newPath.isEmpty() || image.newPath == image.path) {
			continue;
		}

		// Create hierarchy
		const QString path = image.newPath.left(image.newPath.lastIndexOf(QDir::separator()));
		QDir directory(path);
		if (!directory.exists()) {
			QDir dir;
			if (!dir.mkpath(path)) {
				log(QStringLiteral("Could not create destination directory"), Logger::Error);
			}
		}

		// Move file
		if (!QFile::rename(image.path, image.newPath)) {
			log(QStringLiteral("Could not rename file from `%1` to `%2`").arg(image.path, image.newPath), Logger::Error);
		}
		for (const QString &child : image.children) {
			const QString newPath = QString(child).replace(image.path, image.newPath);
			if (!QFile::rename(child, newPath)) {
				log(QStringLiteral("Could not rename child file from `%1` to `%2`").arg(image.path, image.newPath), Logger::Error);
			}
		}

		// Delete old path if necessary
		const QString oldDir = image.path.left(image.path.lastIndexOf(QDir::separator()));
		deleteDir(oldDir);
	}

	// Close
	emit accepted();
	close();
}
