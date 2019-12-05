#include "utils/blacklist-fix/blacklist-fix-2.h"
#include <QFile>
#include <QtConcurrent>
#include <ui_blacklist-fix-2.h>
#include "functions.h"
#include "helpers.h"
#include "loader/token.h"
#include "models/filtering/post-filter.h"


BlacklistFix2::BlacklistFix2(QList<QMap<QString, QString>> details, Blacklist blacklist, QWidget *parent)
	: QDialog(parent), ui(new Ui::BlacklistFix2), m_details(std::move(details)), m_blacklist(std::move(blacklist))
{
	ui->setupUi(this);

	ui->tableWidget->setRowCount(m_details.size());
	m_previews.reserve(m_details.count());
	for (int i = 0; i < m_details.size(); i++) {
		QStringList found;
		QString color = "blue";
		if (m_details[i].contains("tags")) {
			QMap<QString, Token> tokens;
			tokens.insert("allos", Token(m_details[i]["tags"].split(' ')));
			found = m_blacklist.match(tokens);
			color = found.empty() ? "green" : "red";
		}

		QTableWidgetItem *id = new QTableWidgetItem(QString::number(i + 1));
		id->setIcon(QIcon(":/images/colors/" + color + ".png"));

		QLabel *preview = new QLabel();
		m_previews.append(preview);

		ui->tableWidget->setItem(i, 0, id);
		ui->tableWidget->setCellWidget(i, 1, preview);
		ui->tableWidget->setItem(i, 2, new QTableWidgetItem(m_details.at(i).value("path")));
		ui->tableWidget->setItem(i, 3, new QTableWidgetItem(found.join(" ")));
		ui->tableWidget->resizeColumnToContents(0);
	}
	QHeaderView *headerView = ui->tableWidget->horizontalHeader();
	headerView->setSectionResizeMode(QHeaderView::Interactive);
	headerView->resizeSection(1, 50);
	headerView->setSectionResizeMode(2, QHeaderView::Stretch);

	QtConcurrent::run(this, &BlacklistFix2::loadThumbnails);
}
BlacklistFix2::~BlacklistFix2()
{
	delete ui;
}

void BlacklistFix2::loadThumbnails()
{
	for (int i = 0; i < m_previews.count(); ++i) {
		m_previews[i]->setPixmap(QPixmap(m_details[i]["path_full"]).scaledToHeight(50, Qt::SmoothTransformation));
	}
}

void BlacklistFix2::on_buttonSelectBlacklisted_clicked()
{
	ui->tableWidget->setSelectionMode(QTableWidget::MultiSelection);
	for (int i = 0; i < ui->tableWidget->rowCount(); i++) {
		if (!ui->tableWidget->item(i, 3)->text().isEmpty()) {
			ui->tableWidget->selectRow(i);
		}
	}
	ui->tableWidget->setSelectionMode(QTableWidget::ExtendedSelection);
}
void BlacklistFix2::on_buttonCancel_clicked()
{
	emit rejected();
	close();
}
void BlacklistFix2::on_buttonOk_clicked()
{
	// Delete selected images
	QList<QTableWidgetItem *> selected = ui->tableWidget->selectedItems();
	if (selected.isEmpty()) {
		error(this, "You didn't select any image do delete.");
		return;
	}

	// List all rows to be deleted
	QList<int> rows;
	for (QTableWidgetItem *item : selected) {
		int row = item->row();
		if (!rows.contains(row)) {
			rows.append(row);
		}
	}

	// Sort in ascending order to help the following foreach with deletion
	std::sort(rows.begin(), rows.end());

	// Delete files and their associated rows
	int rem = 0;
	for (int i : qAsConst(rows)) {
		int pos = i - rem;
		QFile::remove(m_details.at(ui->tableWidget->item(pos, 0)->text().toInt() - 1).value("path_full"));
		ui->tableWidget->removeRow(pos);
		rem++;
	}

	// Close
	emit accepted();
	close();
}
