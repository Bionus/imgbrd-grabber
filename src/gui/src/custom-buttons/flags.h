#ifndef CUSTOM_BUTTONS_FLAGS_H
#define CUSTOM_BUTTONS_FLAGS_H


namespace CustomButtons
{
	constexpr unsigned short IsUtilityButton		=          0b00000001 ;	// Nib0.0, bit 0.
		constexpr unsigned short IsButtonDetails	=  0b0000000100000001 ;	// Nib2.0, bit8.
		constexpr unsigned short IsButtonOpen		=  0b0000010000000001 ;	// Nib2.2, bit10.

	constexpr unsigned short IsNavButton			=          0b00000010 ;	// Nib0.1, bit 1.
		//constexpr unsigned short IsQuit			=  0b0000000100000010 ;	// Nib2.0, bit 8.
		constexpr unsigned short IsButtonPrev		=  0b0000001000000010 ;	// Nib2.1, bit 9.
		constexpr unsigned short IsButtonNext		=  0b0000010000000010 ;	// Nib2.2, bit 10.

	constexpr unsigned short IsSavingButton			=          0b00000100 ;	// Nib0.2, bit 2.
		constexpr unsigned short IsButtonSave		=  0b0000000100000100 ;	// Nib2.0, bit8.
		constexpr unsigned short IsButtonSaveAs		=  0b0000001000000100 ;	// Nib2.1, bit9.
		constexpr unsigned short IsButtonSaveNQuit	=  0b0000010000000100 ;	// Nib2.2, bit10.

	constexpr unsigned short IsFavoriteButton		=          0b00001000 ;	// Nib0.3, bit 3.
}

#endif // CUSTOM_BUTTONS_FLAGS_H
