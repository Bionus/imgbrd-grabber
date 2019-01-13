#include "utils/blacklist-fix/blacklist-fix-2.h"
#include <QFile>
#include <ui_blacklist-fix-2.h>
#include "loader/token.h"
#include "models/filtering/post-filter.h"


BlacklistFix2::BlacklistFix2(QList<QMap<QString, QString>> details, Blacklist blacklist, QWidget *parent)
	: QDialog(parent), ui(new Ui::BlacklistFix2), m_details(std::move(details)), m_blacklist(std::move(blacklist))
{
	ui->setupUi(this);

	ui->tableWidget->setRowCount(m_details.size());
	QStringList found = QStringList(), tags;
	m_previews.reserve(m_details.count());
	for (int i = 0; i < m_details.size(); i++) {
		QString color = "blue";
		if (m_details.at(i).contains("tags")) {
			QMap<QString, Token> tokens;
			tokens.insert("allos", Token(tags));
			found = m_blacklist.match(tokens);
			color = found.empty() ? "green" : "red";
		}
		QTableWidgetItem *id = new QTableWidgetItem(QString::number(i + 1));
		id->setIcon(QIcon(":/images/colors/" + color + ".png"));
		ui->tableWidget->setItem(i, 0, id);
		QLabel *preview = new QLabel();
		preview->setPixmap(QPixmap(m_details.at(i).value("path_full")).scaledToHeight(50, Qt::SmoothTransformation));
		m_previews.append(preview);
		ui->tableWidget->setCellWidget(i, 1, preview);
		ui->tableWidget->setItem(i, 2, new QTableWidgetItem(m_details.at(i).value("path")));
		ui->tableWidget->setItem(i, 3, new QTableWidgetItem(found.join(" ")));
		ui->tableWidget->resizeColumnToContents(0);
	}
	QHeaderView *headerView = ui->tableWidget->horizontalHeader();
	headerView->setSectionResizeMode(QHeaderView::Interactive);
	headerView->resizeSection(1, 50);
	headerView->setSectionResizeMode(2, QHeaderView::Stretch);
}
BlacklistFix2::~BlacklistFix2()
{
	delete ui;
}

void BlacklistFix2::on_buttonSelectBlacklisted_clicked()
{
	for (int i = 0; i < ui->tableWidget->rowCount(); i++) {
		if (!ui->tableWidget->item(i, 3)->text().isEmpty()) {
			ui->tableWidget->selectRow(i);
		}
	}
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
	const int count = selected.size();
	QSet<int> toDelete = QSet<int>();
	for (int i = 0; i < count; i++) {
		toDelete.insert(selected.at(i)->row());
	}
	int rem = 0;
	for (int i : toDelete) {
		QFile::remove(m_details.at(ui->tableWidget->item(i - rem, 0)->text().toInt() - 1).value("path_full"));
		ui->tableWidget->removeRow(i - rem);
		rem++;
	}

	// Close
	emit accepted();
	close();
}
