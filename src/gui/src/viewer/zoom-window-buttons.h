#ifndef CUSTOM_BUTTONS_DEFAULTS_H
#define CUSTOM_BUTTONS_DEFAULTS_H

#include <QStringLiteral>
#include "custom-buttons/state.h"


namespace ZoomWindowButtons
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

	const ButtonState DefaultPrevState(0, QStringLiteral("<"), QStringLiteral("Previous search result"));
	const ButtonState DefaultNextState(0, QStringLiteral(">"), QStringLiteral("Next search result"));
	const ButtonState DefaultDetailsState(0, QStringLiteral("Details"), QStringLiteral("Media details"));
	const ButtonState DefaultSaveAsState(0, QStringLiteral("Save as..."), QStringLiteral("Save to irregular location"));

	const ButtonState DefaultSaveStateSave(SaveState::Save, QStringLiteral("Save"), QStringLiteral("Save to usual location"));
	const ButtonState DefaultSaveStateSaving(SaveState::Saving, QStringLiteral("Saving..."), QStringLiteral(""));
	const ButtonState DefaultSaveStateSaved(SaveState::Saved, QStringLiteral("Saved!"), QStringLiteral(""));
	const ButtonState DefaultSaveStateCopied(SaveState::Copied, QStringLiteral("Copied!"), QStringLiteral(""));
	const ButtonState DefaultSaveStateMoved(SaveState::Moved, QStringLiteral("Moved!"), QStringLiteral(""));
	const ButtonState DefaultSaveStateLinked(SaveState::Linked, QStringLiteral("Link created!"), QStringLiteral(""));
	const ButtonState DefaultSaveStateExistsMd5(SaveState::ExistsMd5, QStringLiteral("MD5 already exists"), QStringLiteral(""));
	const ButtonState DefaultSaveStateExistsDisk(SaveState::ExistsDisk, QStringLiteral("Already saved"), QStringLiteral(""));
	const ButtonState DefaultSaveStateDelete(SaveState::Delete, QStringLiteral("Delete"), QStringLiteral(""));

	const ButtonState DefaultSaveNQuitStateSave(SaveState::Save, QStringLiteral("Save & close"), QStringLiteral("Save to usual location and close window"));
	const ButtonState DefaultSaveNQuitStateSaving(SaveState::Saving, QStringLiteral("Saving..."), QStringLiteral(""));
	const ButtonState DefaultSaveNQuitStateClose(2, QStringLiteral("Close"), QStringLiteral("")); // Consider adding this to SaveButtonState.

	const ButtonState DefaultOpenState(0, QStringLiteral("Open"), QStringLiteral("Open usual save location in new window"));

	const ButtonState DefaultSaveFavStateSave(SaveState::Save, QStringLiteral("Save (fav)"), QStringLiteral("Save to favourite location"));
	const ButtonState DefaultSaveFavStateSaving(SaveState::Saving, QStringLiteral("Saving... (fav)"), QStringLiteral(""));
	const ButtonState DefaultSaveFavStateSaved(SaveState::Saved, QStringLiteral("Saved! (fav)"), QStringLiteral(""));
	const ButtonState DefaultSaveFavStateCopied(SaveState::Copied, QStringLiteral("Copied! (fav)"), QStringLiteral(""));
	const ButtonState DefaultSaveFavStateMoved(SaveState::Moved, QStringLiteral("Moved! (fav)"), QStringLiteral(""));
	const ButtonState DefaultSaveFavStateLinked(SaveState::Linked, QStringLiteral("Link created! (fav)"), QStringLiteral(""));
	const ButtonState DefaultSaveFavStateExistsMd5(SaveState::ExistsMd5, QStringLiteral("MD5 already exists (fav)"), QStringLiteral(""));
	const ButtonState DefaultSaveFavStateExistsDisk(SaveState::ExistsDisk, QStringLiteral("Already saved (fav)"), QStringLiteral(""));
	const ButtonState DefaultSaveFavStateDelete(SaveState::Delete, QStringLiteral("Delete (fav)"), QStringLiteral(""));

	const ButtonState DefaultSaveNQuitFavStateSave(SaveState::Save, QStringLiteral("Save & close (fav)"), QStringLiteral("Save to favourite location and close window"));
	const ButtonState DefaultSaveNQuitFavStateSaving(SaveState::Saving, QStringLiteral("Saving... (fav)"), QStringLiteral(""));
	const ButtonState DefaultSaveNQuitFavStateClose(2, QStringLiteral("Close (fav)"), QStringLiteral("")); // Consider adding this to SaveButtonState.

	const ButtonState DefaultOpenFavState(0, QStringLiteral("Open (fav)"), QStringLiteral("Open favourite save location in new window"));
}

#endif // CUSTOM_BUTTONS_DEFAULTS_H
