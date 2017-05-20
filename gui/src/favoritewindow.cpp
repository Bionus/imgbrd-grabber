#include "favoritewindow.h"
#include "ui_favoritewindow.h"
#include <QFile>
#include <QFileDialog>
#include <QSettings>
#include "models/profile.h"
#include "functions.h"


/**
 * Constructor of the favoriteWindow class, completing its window.
 * @param	tag			The favorite that will be edited
 * @param	note		Its note, from 0 to 100
 * @param	lastviewed	The date when the user last cliked on "Set as viewed"
 * @param	imagepath	Path to the favorite's image
 * @param	parent		The parent window
 */
favoriteWindow::favoriteWindow(Profile *profile, Favorite favorite, QWidget *parent)
	: QDialog(parent), ui(new Ui::favoriteWindow), m_profile(profile), m_favorite(favorite)
{
	ui->setupUi(this);

	ui->tagLineEdit->setText(m_favorite.getName());
	ui->noteSpinBox->setValue(m_favorite.getNote());
	ui->lastViewedDateTimeEdit->setDateTime(m_favorite.getLastViewed());

	connect(this, SIGNAL(accepted()), this, SLOT(save()));
}

/**
 * Destructor of the favoriteWindow class
 */
favoriteWindow::~favoriteWindow()
{
	delete ui;
}

/**
 * Removes the favorite from the list and closes the window
 */
void favoriteWindow::on_buttonRemove_clicked()
{
	m_profile->removeFavorite(m_favorite);
	close();
}

/**
 * Opens a window to choose an image to set the imagepath value.
 */
void favoriteWindow::on_openButton_clicked()
{
	QString file = QFileDialog::getOpenFileName(this, tr("Choose an image"), QSettings(savePath("settings.ini"), QSettings::IniFormat).value("Save/path").toString(), "Images (*.png *.gif *.jpg *.jpeg)");
	if (!file.isEmpty())
	{ ui->imageLineEdit->setText(file); }
}

/**
 * Update the local favorites file and add thumb if necessary.
 */
void favoriteWindow::save()
{
	Favorite oldFav = m_favorite;
	m_favorite = Favorite(ui->tagLineEdit->text(), ui->noteSpinBox->value(), ui->lastViewedDateTimeEdit->dateTime());
	m_favorite.setImagePath(savePath("thumbs/" + m_favorite.getName(true) + ".png"));

	if (oldFav.getName() != m_favorite.getName())
	{
		if (QFile::exists(savePath("thumbs/" + oldFav.getName(true) + ".png")))
		{ QFile::rename(savePath("thumbs/" + oldFav.getName(true) + ".png"), m_favorite.getImagePath()); }
		m_profile->removeFavorite(oldFav);
	}

	if (QFile::exists(ui->imageLineEdit->text()))
	{
		QPixmap img(ui->imageLineEdit->text());
		if (!img.isNull())
		{ m_favorite.setImage(img); }
	}
	m_profile->addFavorite(m_favorite);
}
