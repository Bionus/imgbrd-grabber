#ifndef VIEWER_WINDOW_BUTTONS_H
#define VIEWER_WINDOW_BUTTONS_H

#include <QMap>
#include <QStringLiteral>
#include "custom-buttons/state.h"


namespace ViewerWindowButtons
{
	enum SaveState : unsigned short
	{
		Save,
		Saving,
		Saved,
		Copied,
		Moved,
		Linked,
		ExistsMd5,
		ExistsDisk,
		Delete
	};

	const ButtonState DefaultPrevState(0, QStringLiteral("<"), QObject::tr("Previous search result"));
	const ButtonState DefaultNextState(0, QStringLiteral(">"), QObject::tr("Next search result"));
	const ButtonState DefaultDetailsState(0, QObject::tr("Details"), QObject::tr("Media details"));
	const ButtonState DefaultSaveAsState(0, QObject::tr("Save as..."), QObject::tr("Save to irregular location"));

	const ButtonState DefaultSaveStateSave(SaveState::Save, QObject::tr("Save"), QObject::tr("Save to usual location"));
	const ButtonState DefaultSaveStateSaving(SaveState::Saving, QObject::tr("Saving..."), QStringLiteral(""));
	const ButtonState DefaultSaveStateSaved(SaveState::Saved, QObject::tr("Saved!"), QStringLiteral(""));
	const ButtonState DefaultSaveStateCopied(SaveState::Copied, QObject::tr("Copied!"), QStringLiteral(""));
	const ButtonState DefaultSaveStateMoved(SaveState::Moved, QObject::tr("Moved!"), QStringLiteral(""));
	const ButtonState DefaultSaveStateLinked(SaveState::Linked, QObject::tr("Link created!"), QStringLiteral(""));
	const ButtonState DefaultSaveStateExistsMd5(SaveState::ExistsMd5, QObject::tr("MD5 already exists"), QStringLiteral(""));
	const ButtonState DefaultSaveStateExistsDisk(SaveState::ExistsDisk, QObject::tr("Already saved"), QStringLiteral(""));
	const ButtonState DefaultSaveStateDelete(SaveState::Delete, QObject::tr("Delete"), QStringLiteral(""));

	const ButtonState DefaultSaveNQuitStateSave(SaveState::Save, QObject::tr("Save & close"), QObject::tr("Save to usual location and close window"));
	const ButtonState DefaultSaveNQuitStateSaving(SaveState::Saving, QObject::tr("Saving..."), QStringLiteral(""));
	const ButtonState DefaultSaveNQuitStateClose(2, QObject::tr("Close"), QStringLiteral("")); // Consider adding this to SaveButtonState.

	const ButtonState DefaultOpenState(0, QObject::tr("Open"), QObject::tr("Open usual save location in new window"));

	const ButtonState DefaultSaveFavStateSave(SaveState::Save, QObject::tr("Save (fav)"), QObject::tr("Save to favourite location"));
	const ButtonState DefaultSaveFavStateSaving(SaveState::Saving, QObject::tr("Saving... (fav)"), QStringLiteral(""));
	const ButtonState DefaultSaveFavStateSaved(SaveState::Saved, QObject::tr("Saved! (fav)"), QStringLiteral(""));
	const ButtonState DefaultSaveFavStateCopied(SaveState::Copied, QObject::tr("Copied! (fav)"), QStringLiteral(""));
	const ButtonState DefaultSaveFavStateMoved(SaveState::Moved, QObject::tr("Moved! (fav)"), QStringLiteral(""));
	const ButtonState DefaultSaveFavStateLinked(SaveState::Linked, QObject::tr("Link created! (fav)"), QStringLiteral(""));
	const ButtonState DefaultSaveFavStateExistsMd5(SaveState::ExistsMd5, QObject::tr("MD5 already exists (fav)"), QStringLiteral(""));
	const ButtonState DefaultSaveFavStateExistsDisk(SaveState::ExistsDisk, QObject::tr("Already saved (fav)"), QStringLiteral(""));
	const ButtonState DefaultSaveFavStateDelete(SaveState::Delete, QObject::tr("Delete (fav)"), QStringLiteral(""));

	const ButtonState DefaultSaveNQuitFavStateSave(SaveState::Save, QObject::tr("Save & close (fav)"), QObject::tr("Save to favourite location and close window"));
	const ButtonState DefaultSaveNQuitFavStateSaving(SaveState::Saving, QObject::tr("Saving... (fav)"), QStringLiteral(""));
	const ButtonState DefaultSaveNQuitFavStateClose(2, QObject::tr("Close (fav)"), QStringLiteral("")); // Consider adding this to SaveButtonState.

	const ButtonState DefaultOpenFavState(0, QObject::tr("Open (fav)"), QObject::tr("Open favourite save location in new window"));


	const QMap<unsigned short, ButtonState> DefaultStates {
		{ CustomButtons::IsButtonPrev, DefaultPrevState },
		{ CustomButtons::IsButtonNext, DefaultNextState },
		{ CustomButtons::IsButtonDetails, DefaultDetailsState },
		{ CustomButtons::IsButtonSaveAs, DefaultSaveAsState },

		{ CustomButtons::IsButtonSave, DefaultSaveStateSave },
		{ CustomButtons::IsButtonSaveNQuit, DefaultSaveNQuitStateSave },
		{ CustomButtons::IsButtonOpen, DefaultOpenState },

		{ CustomButtons::IsButtonSave | CustomButtons::IsFavoriteButton, DefaultSaveFavStateSave },
		{ CustomButtons::IsButtonSaveNQuit | CustomButtons::IsFavoriteButton, DefaultSaveNQuitFavStateSave },
		{ CustomButtons::IsButtonOpen | CustomButtons::IsFavoriteButton, DefaultOpenFavState },
	};
}

#endif // VIEWER_WINDOW_BUTTONS_H
