#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WindowSystemBPLibrary.h"
#include "MainDragDrop.h"

// Windows Includes
#include "Windows/WindowsApplication.h"

class WINDOWSYSTEM_API DragDropMessageHandler : public IWindowsMessageHandler
{
public:

	AMainDragDrop* Sender;

	virtual bool ProcessMessage(HWND Hwnd, uint32 Message, WPARAM WParam, LPARAM LParam, int32& OutResult) override
	{
		// Drop System.
		HDROP DropInfo = (HDROP)WParam;
		char DroppedFile[MAX_PATH];

		// File Path.
		std::string temp_string;
		TArray<FString> Array_Paths;

		switch (Message)
		{

		case WM_DROPFILES:
			for (int32 FileIndex = 0; DragQueryFileA(DropInfo, FileIndex, (LPSTR)DroppedFile, sizeof(DroppedFile)); FileIndex++)
			{
				if (GetFileAttributesA(DroppedFile) != FILE_ATTRIBUTE_DIRECTORY)
				{
					temp_string = DroppedFile;
					Array_Paths.Add(temp_string.c_str());
				}
			}

			this->Sender->OnFileDrop(Array_Paths);
			DragFinish(DropInfo);
			break;
		}

		return true;
	}
};
