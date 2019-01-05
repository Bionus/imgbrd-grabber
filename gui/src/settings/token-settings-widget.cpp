#include "token-settings-widget.h"
#include <QSettings>
#include "ui_token-settings-widget.h"

TokenSettingsWidget::TokenSettingsWidget(QSettings *settings, QString name, bool enableShorter, const QString &defaultEmpty, const QString &defaultMultiple, QWidget *parent)
	: QWidget(parent), ui(new Ui::TokenSettingsWidget), m_settings(settings), m_name(std::move(name)), m_enableShorter(enableShorter)
{
	ui->setupUi(this);

	static const QStringList tagsSort { "original", "name" };

	ui->lineIfNone->setText(m_settings->value(m_name + "_empty", defaultEmpty).toString());
	ui->comboSort->setCurrentIndex(tagsSort.indexOf(m_settings->value(m_name + "_sort", "original").toString()));
	ui->spinMoreThanN->setValue(m_settings->value(m_name + "_multiple_limit", 1).toInt());
	ui->spinKeepN->setValue(m_settings->value(m_name + "_multiple_keepN", 1).toInt());
	ui->spinKeepNThenAdd->setValue(m_settings->value(m_name + "_multiple_keepNThenAdd_keep", 1).toInt());
	ui->lineKeepNThenAdd->setText(m_settings->value(m_name + "_multiple_keepNThenAdd_add", " (+ %count%)").toString());
	ui->lineSeparator->setText(m_settings->value(m_name + "_sep", "+").toString());
	ui->lineReplaceAll->setText(m_settings->value(m_name + "_value", defaultMultiple).toString());

	const QString multiple = m_settings->value(m_name + "_multiple", "keepAll").toString();
	if		(multiple == "keepAll")			{ ui->radioKeepAll->setChecked(true);		}
	else if	(multiple == "keepN")			{ ui->radioKeepN->setChecked(true);			}
	else if	(multiple == "keepNThenAdd")	{ ui->radioKeepNThenAdd->setChecked(true);	}
	else if	(multiple == "replaceAll")		{ ui->radioReplaceAll->setChecked(true);	}
	else if	(multiple == "multiple")		{ ui->radioMultiple->setChecked(true);		}

	ui->checkUseShorter->setVisible(m_enableShorter);
	if (m_enableShorter) {
		ui->checkUseShorter->setChecked(settings->value(m_name + "_useshorter", true).toBool());
	}
}

TokenSettingsWidget::~TokenSettingsWidget()
{
	delete ui;
}

void TokenSettingsWidget::save()
{
	static const QStringList tagsSort { "original", "name" };

	m_settings->setValue(m_name + "_empty", ui->lineIfNone->text());
	m_settings->setValue(m_name + "_sort", tagsSort.at(ui->comboSort->currentIndex()));
	m_settings->setValue(m_name + "_useall", ui->radioKeepAll->isChecked());
	m_settings->setValue(m_name + "_multiple_limit", ui->spinMoreThanN->value());
	m_settings->setValue(m_name + "_multiple_keepN", ui->spinKeepN->value());
	m_settings->setValue(m_name + "_multiple_keepNThenAdd_keep", ui->spinKeepNThenAdd->value());
	m_settings->setValue(m_name + "_multiple_keepNThenAdd_add", ui->lineKeepNThenAdd->text());
	m_settings->setValue(m_name + "_sep", ui->lineSeparator->text());
	m_settings->setValue(m_name + "_value", ui->lineReplaceAll->text());

	QString artistMultiple;
	if		(ui->radioKeepAll->isChecked())			{ artistMultiple = "keepAll";		}
	else if	(ui->radioKeepN->isChecked())			{ artistMultiple = "keepN";			}
	else if	(ui->radioKeepNThenAdd->isChecked())	{ artistMultiple = "keepNThenAdd";	}
	else if	(ui->radioReplaceAll->isChecked())		{ artistMultiple = "replaceAll";	}
	else if	(ui->radioMultiple->isChecked())		{ artistMultiple = "multiple";		}
	m_settings->setValue(m_name + "_multiple", artistMultiple);

	if (m_enableShorter) {
		m_settings->setValue(m_name + "_useshorter", ui->checkUseShorter->isChecked());
	}
}
