#ifndef CUSTOM_BUTTONS_DEFAULTS_H
#define CUSTOM_BUTTONS_DEFAULTS_H

#include <QStringLiteral>
#include "state.h"


namespace Ui
{
	// TODO(Bionus): move this out of the Qt-reserved Ui namespace
	// TODO(Bionus): duplicate of ZoomWindow::SaveButtonState, should only keep one
	enum SaveButtonState : unsigned short
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
}

namespace CustomButtons
{
	const ButtonState DefaultPrevState(0, QStringLiteral("<"), QStringLiteral("Previous search result"));
	const ButtonState DefaultNextState(0, QStringLiteral(">"), QStringLiteral("Next search result"));
	const ButtonState DefaultDetailsState(0, QStringLiteral("Details"), QStringLiteral("Media details"));
	const ButtonState DefaultSaveAsState(0, QStringLiteral("Save as..."), QStringLiteral("Save to irregular location"));

	const ButtonState DefaultSaveStateSave(Ui::SaveButtonState::Save, QStringLiteral("Save"), QStringLiteral("Save to usual location"));
	const ButtonState DefaultSaveStateSaving(Ui::SaveButtonState::Saving, QStringLiteral("Saving..."), QStringLiteral(""));
	const ButtonState DefaultSaveStateSaved(Ui::SaveButtonState::Saved, QStringLiteral("Saved!"), QStringLiteral(""));
	const ButtonState DefaultSaveStateCopied(Ui::SaveButtonState::Copied, QStringLiteral("Copied!"), QStringLiteral(""));
	const ButtonState DefaultSaveStateMoved(Ui::SaveButtonState::Moved, QStringLiteral("Moved!"), QStringLiteral(""));
	const ButtonState DefaultSaveStateLinked(Ui::SaveButtonState::Linked, QStringLiteral("Link created!"), QStringLiteral(""));
	const ButtonState DefaultSaveStateExistsMd5(Ui::SaveButtonState::ExistsMd5, QStringLiteral("MD5 already exists"), QStringLiteral(""));
	const ButtonState DefaultSaveStateExistsDisk(Ui::SaveButtonState::ExistsDisk, QStringLiteral("Already saved"), QStringLiteral(""));
	const ButtonState DefaultSaveStateDelete(Ui::SaveButtonState::Delete, QStringLiteral("Delete"), QStringLiteral(""));

	const ButtonState DefaultSaveNQuitStateSave(Ui::SaveButtonState::Save, QStringLiteral("Save & close"), QStringLiteral("Save to usual location and close window"));
	const ButtonState DefaultSaveNQuitStateSaving(Ui::SaveButtonState::Saving, QStringLiteral("Saving..."), QStringLiteral(""));
	const ButtonState DefaultSaveNQuitStateClose(2, QStringLiteral("Close"), QStringLiteral("")); // Consider adding this to SaveButtonState.

	const ButtonState DefaultOpenState(0, QStringLiteral("Open"), QStringLiteral("Open usual save location in new window"));

	const ButtonState DefaultSaveFavStateSave(Ui::SaveButtonState::Save, QStringLiteral("Save (fav)"), QStringLiteral("Save to favourite location"));
	const ButtonState DefaultSaveFavStateSaving(Ui::SaveButtonState::Saving, QStringLiteral("Saving... (fav)"), QStringLiteral(""));
	const ButtonState DefaultSaveFavStateSaved(Ui::SaveButtonState::Saved, QStringLiteral("Saved! (fav)"), QStringLiteral(""));
	const ButtonState DefaultSaveFavStateCopied(Ui::SaveButtonState::Copied, QStringLiteral("Copied! (fav)"), QStringLiteral(""));
	const ButtonState DefaultSaveFavStateMoved(Ui::SaveButtonState::Moved, QStringLiteral("Moved! (fav)"), QStringLiteral(""));
	const ButtonState DefaultSaveFavStateLinked(Ui::SaveButtonState::Linked, QStringLiteral("Link created! (fav)"), QStringLiteral(""));
	const ButtonState DefaultSaveFavStateExistsMd5(Ui::SaveButtonState::ExistsMd5, QStringLiteral("MD5 already exists (fav)"), QStringLiteral(""));
	const ButtonState DefaultSaveFavStateExistsDisk(Ui::SaveButtonState::ExistsDisk, QStringLiteral("Already saved (fav)"), QStringLiteral(""));
	const ButtonState DefaultSaveFavStateDelete(Ui::SaveButtonState::Delete, QStringLiteral("Delete (fav)"), QStringLiteral(""));

	const ButtonState DefaultSaveNQuitFavStateSave(Ui::SaveButtonState::Save, QStringLiteral("Save (fav)"), QStringLiteral("Save to usual location and close window"));
	const ButtonState DefaultSaveNQuitFavStateSaving(Ui::SaveButtonState::Saving, QStringLiteral("Saving... (fav)"), QStringLiteral(""));
	const ButtonState DefaultSaveNQuitFavStateClose(2, QStringLiteral("Close (fav)"), QStringLiteral("")); // Consider adding this to SaveButtonState.

	const ButtonState DefaultOpenFavState(0, QStringLiteral("Open (fav)"), QStringLiteral("Open favourite save location in new window"));
}

#endif // CUSTOM_BUTTONS_DEFAULTS_H
