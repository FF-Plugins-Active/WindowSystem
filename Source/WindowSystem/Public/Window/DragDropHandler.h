#pragma once

#include "CoreMinimal.h"

THIRD_PARTY_INCLUDES_START
#include "Windows/WindowsHWrapper.h"		// Necessary include.
#include "Windows/WindowsApplication.h"		// File Drag Drop Message Handler.
#include "shellapi.h"						// File Drag Drop Callback.
#include "dwmapi.h"							// Windows 11 Rounded Window Include.
#include <winreg.h>                         // Regedit access.
#include "winuser.h"						// Necessary include.
#include "Windows/MinWindows.h"				// Necessary include.	
THIRD_PARTY_INCLUDES_END

// File Drag Drop Message Handler Subclass.
class FDragDropHandler : public IWindowsMessageHandler
{

public:

	AActor* OwnerActor = nullptr;

	bool ProcessMessage(HWND Hwnd, uint32 Message, WPARAM WParam, LPARAM LParam, int32& OutResult) override;

};