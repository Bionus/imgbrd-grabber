#ifndef VIDEO_PLAYER_H
#define VIDEO_PLAYER_H

#include "player.h"
#include <QString>


namespace Ui
{
	class VideoPlayer;
}


class QMediaPlayer;
class QMediaPlaylist;
class QVideoWidget;
class QWidget;

class VideoPlayer : public Player
{
	Q_OBJECT

	public:
		explicit VideoPlayer(QWidget *parent = nullptr);
		~VideoPlayer();

		bool supports(const QString &file) override;
		void load(const QString &file) override;
		void unload() override;
		int duration() override;

	protected slots:
		void playPause();
		void durationChanged(qint64 duration);
		void positionChanged(qint64 pos);
		void seek(int position);

	private:
		Ui::VideoPlayer *ui;
		QStringList m_supportedFormats;
		QVideoWidget *m_videoWidget;
		QMediaPlayer *m_mediaPlayer;
		QMediaPlaylist *m_mediaPlaylist;
		bool m_noSeek = false;
};

#endif // VIDEO_PLAYER_H
