// Copyright Epic Games, Inc. All Rights Reserved.

#include "WindowSystemBPLibrary.h"
#include "WindowSystem.h"

// UE Includes.
#include "Slate/WidgetRenderer.h"		// Widget to Texture 2D
#include "HAL/FileManager.h"
#include "HAL/FileManagerGeneric.h"

UWindowSystemBPLibrary::UWindowSystemBPLibrary(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{

}

bool UWindowSystemBPLibrary::IsWindowTopMost(UPARAM(ref)UWindowObject*& InWindowObject, bool bUseNative)
{
	if (IsValid(InWindowObject) == false)
	{
		return false;
	}

	if (InWindowObject->WindowPtr.IsValid() == false)
	{
		return false;
	}

	if (bUseNative == true)
	{
		HWND WidgetWindowHandle = reinterpret_cast<HWND>(InWindowObject->WindowPtr.ToSharedRef().Get().GetNativeWindow().ToSharedRef().Get().GetOSWindowHandle());

		if (GetWindowLong(WidgetWindowHandle, GWL_EXSTYLE) & WS_EX_TOPMOST)
		{
			return true;
		}

		else
		{
			return false;
		}
	}

	else
	{
		return InWindowObject->WindowPtr->IsTopmostWindow();
	}
}

void UWindowSystemBPLibrary::IsWindowHovered(UPARAM(ref)UWindowObject*& InWindowObject, FDelegateHover DelegateHover)
{
	if (IsValid(InWindowObject) == false)
	{
		DelegateHover.Execute(false);
		return;
	}
	
	if (InWindowObject->WindowPtr.IsValid() == false)
	{
		DelegateHover.Execute(false);
		return;
	}
	
	if (InWindowObject->WindowPtr.ToSharedRef().Get().IsDirectlyHovered() == true)
	{
		DelegateHover.Execute(true);
		return;
	}
}

bool UWindowSystemBPLibrary::BringWindowFront(UPARAM(ref)UWindowObject*& InWindowObject, bool bFlashWindow)
{
	if (IsValid(InWindowObject) == false)
	{
		return false;
	}
	
	if (InWindowObject->WindowPtr.IsValid() == false)
	{
		return false;
	}

	InWindowObject->WindowPtr.Get()->BringToFront();

	if (bFlashWindow == true)
	{
		InWindowObject->WindowPtr.Get()->FlashWindow();
	}
	
	return true;
}

bool UWindowSystemBPLibrary::ToggleTopMostOption(UPARAM(ref)UWindowObject*& InWindowObject)
{
	if (IsValid(InWindowObject) == false)
	{
		return false;
	}

	if (InWindowObject->WindowPtr.IsValid() == false)
	{
		return false;
	}

	HWND WidgetWindowHandle = reinterpret_cast<HWND>(InWindowObject->WindowPtr.ToSharedRef().Get().GetNativeWindow().ToSharedRef().Get().GetOSWindowHandle());
	
	if (GetWindowLong(WidgetWindowHandle, GWL_EXSTYLE) & WS_EX_TOPMOST)
	{
		SetWindowPos(WidgetWindowHandle, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
		return true;
	}

	else
	{
		SetWindowPos(WidgetWindowHandle, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
		return true;
	}
}

bool UWindowSystemBPLibrary::ToggleShowOnTaskBar(UPARAM(ref)UWindowObject*& InWindowObject, bool bShowOnTaskBar)
{
	if (IsValid(InWindowObject) == false)
	{
		return false;
	}

	if (InWindowObject->WindowPtr.IsValid() == false)
	{
		return false;
	}

	HWND WidgetWindowHandle = reinterpret_cast<HWND>(InWindowObject->WindowPtr.ToSharedRef().Get().GetNativeWindow().ToSharedRef().Get().GetOSWindowHandle());

	if (bShowOnTaskBar == true)
	{
		if (InWindowObject->bIsTransparent == true)
		{
			SetWindowLongPtr(WidgetWindowHandle, GWL_EXSTYLE, WS_EX_APPWINDOW | WS_EX_TRANSPARENT | WS_EX_LAYERED);
		}

		else
		{
			SetWindowLongPtr(WidgetWindowHandle, GWL_EXSTYLE, WS_EX_APPWINDOW);
		}
		
		UWindowSystemBPLibrary::BringWindowFront(InWindowObject, true);
		
		InWindowObject->bShowOnTaskBar = bShowOnTaskBar;
	}

	else
	{
		if (InWindowObject->bIsTransparent == true)
		{
			SetWindowLongPtr(WidgetWindowHandle, GWL_EXSTYLE, WS_EX_NOACTIVATE | WS_EX_TRANSPARENT | WS_EX_LAYERED);
		}

		else
		{
			SetWindowLongPtr(WidgetWindowHandle, GWL_EXSTYLE, WS_EX_NOACTIVATE);
		}
		
		InWindowObject->bShowOnTaskBar = bShowOnTaskBar;
	}

	return true;
}

bool UWindowSystemBPLibrary::ToggleOpacity(UPARAM(ref)UWindowObject*& InWindowObject, bool bEnable, bool bPassDragDrop)
{
	if (IsValid(InWindowObject) == false)
	{
		return false;
	}

	if (InWindowObject->WindowPtr.IsValid() == false)
	{
		return false;
	}

	if (InWindowObject->bIsFileDropEnabled == true)
	{
		if (bPassDragDrop == false)
		{
			return false;
		}
	}

	HWND WidgetWindowHandle = reinterpret_cast<HWND>(InWindowObject->WindowPtr.ToSharedRef().Get().GetNativeWindow().ToSharedRef().Get().GetOSWindowHandle());

	if (bEnable)
	{
		if (InWindowObject->bShowOnTaskBar == true)
		{
			SetWindowLongPtr(WidgetWindowHandle, GWL_EXSTYLE, WS_EX_APPWINDOW | WS_EX_TRANSPARENT | WS_EX_LAYERED);
		}

		else
		{
			SetWindowLongPtr(WidgetWindowHandle, GWL_EXSTYLE, WS_EX_NOACTIVATE | WS_EX_TRANSPARENT | WS_EX_LAYERED);
		}

		InWindowObject->bIsTransparent = true;
	}

	else
	{
		if (InWindowObject->bShowOnTaskBar == true)
		{
			SetWindowLongPtr(WidgetWindowHandle, GWL_EXSTYLE, WS_EX_APPWINDOW | WS_EX_TRANSPARENT);
		}

		else
		{
			SetWindowLongPtr(WidgetWindowHandle, GWL_EXSTYLE, WS_EX_NOACTIVATE | WS_EX_TRANSPARENT);
		}

		InWindowObject->bIsTransparent = false;
	}

	return true;
}

bool UWindowSystemBPLibrary::SetWindowOpacity(UPARAM(ref)UWindowObject*& InWindowObject, float NewOpacity)
{
	if (IsValid(InWindowObject) == false)
	{
		return false;
	}
	
	if (InWindowObject->WindowPtr.IsValid() == false)
	{
		return false;
	}

	InWindowObject->WindowPtr->SetOpacity(NewOpacity);
	return true;
}

bool UWindowSystemBPLibrary::SetWindowState(UPARAM(ref)UWindowObject*& InWindowObject, EWindowState OutWindowState)
{
	if (IsValid(InWindowObject) == true)
	{
		if (InWindowObject->WindowPtr.IsValid() == true)
		{
			switch (OutWindowState)
			{
			case EWindowState::Minimized:
				InWindowObject->WindowPtr->Minimize();
				return true;
				break;

			case EWindowState::Restored:
				InWindowObject->WindowPtr->Restore();
				return true;
				break;

			case EWindowState::Maximized:
				InWindowObject->WindowPtr->Maximize();
				return true;
				break;
			}

			return true;
		}

		else
		{
			return false;
		}
	}

	else
	{
		return false;
	}
}

bool UWindowSystemBPLibrary::SetWindowShape(UPARAM(ref)UWindowObject*& InWindowObject, FMargin InExtend, float InDuration, float NewOpacity)
{
	if (IsValid(InWindowObject) == true)
	{
		if (InWindowObject->WindowPtr.IsValid() == true)
		{
			FSlateRect CurrentShape = InWindowObject->WindowPtr.ToSharedRef().Get().GetRectInScreen();

			FSlateRect NewShape;
			NewShape.Top = CurrentShape.Top + InExtend.Top;
			NewShape.Left = CurrentShape.Left + InExtend.Left;
			NewShape.Bottom = CurrentShape.Bottom + InExtend.Bottom;
			NewShape.Right = CurrentShape.Right + InExtend.Right;

			FCurveSequence CurveSequence;
			CurveSequence.AddCurve(0, InDuration, ECurveEaseFunction::CubicInOut);

			InWindowObject->WindowPtr.ToSharedRef().Get().MorphToShape(CurveSequence, NewOpacity, NewShape);

			return true;
		}

		else
		{
			return false;
		}
	}

	else
	{
		return false;
	}
}

bool UWindowSystemBPLibrary::SetWindowPosition(UPARAM(ref)UWindowObject*& InWindowObject, FVector2D InNewPosition)
{
	if (IsValid(InWindowObject) == true)
	{
		if (InWindowObject->WindowPtr.IsValid() == true)
		{
			InWindowObject->WindowPtr.Get()->MoveWindowTo(InNewPosition);

			return true;
		}

		else
		{
			return false;
		}
	}

	else
	{
		return false;
	}
}

void UWindowSystemBPLibrary::SetMainWindowPosition(FVector2D InNewPosition)
{
	TSharedRef<SWindow> Window = GEngine->GameViewport->GetWindow().ToSharedRef();
	Window.Get().MoveWindowTo(InNewPosition);
}

bool UWindowSystemBPLibrary::SetWindowTitle(UPARAM(ref)UWindowObject*& InWindowObject, FText InNewTitle)
{
	if (IsValid(InWindowObject) == true)
	{
		if (InWindowObject->WindowPtr.IsValid() == true)
		{
			InWindowObject->WindowPtr.Get()->SetTitle(InNewTitle);

			return true;
		}

		else
		{
			return false;
		}
	}

	else
	{
		return false;
	}
}

bool UWindowSystemBPLibrary::GetWindowState(UPARAM(ref)UWindowObject*& InWindowObject, EWindowState& OutWindowState)
{
	if (IsValid(InWindowObject) == true)
	{
		if (InWindowObject->WindowPtr.IsValid() == true)
		{
			WINDOWPLACEMENT WindowPlacement;
			GetWindowPlacement(reinterpret_cast<HWND>(InWindowObject->WindowPtr.ToSharedRef().Get().GetNativeWindow().ToSharedRef().Get().GetOSWindowHandle()), &WindowPlacement);

			switch (WindowPlacement.showCmd)
			{
			case SW_NORMAL:
				OutWindowState = EWindowState::Restored;
				return true;
				break;

			case SW_MAXIMIZE:
				OutWindowState = EWindowState::Maximized;
				return true;
				break;

			case SW_SHOWMINIMIZED:
				OutWindowState = EWindowState::Minimized;
				return true;
				break;
			}
		}

		else
		{
			return false;
		}
	}

	return false;
}

bool UWindowSystemBPLibrary::GetWindowPosition(UPARAM(ref)UWindowObject*& InWindowObject, FVector2D& OutPosition)
{
	if (IsValid(InWindowObject) == true)
	{
		if (InWindowObject->WindowPtr.IsValid() == true)
		{
			OutPosition = InWindowObject->WindowPtr->GetPositionInScreen();
			return true;
		}

		else
		{
			OutPosition;
			return false;
		}
	}

	else
	{
		OutPosition;
		return false;
	}
}

bool UWindowSystemBPLibrary::GetWindowWidget(UPARAM(ref)UWindowObject*& InWindowObject, UUserWidget*& OutWidget)
{
	if (IsValid(InWindowObject) == true)
	{
		if (IsValid(InWindowObject->ContentWidget) == true)
		{
			OutWidget = InWindowObject->ContentWidget;

			return true;
		}

		else
		{
			return false;
		}
	}

	else
	{
		return false;
	}
}

bool UWindowSystemBPLibrary::GetWindowTitle(UPARAM(ref)UWindowObject*& InWindowObject, FText& OutWindowTitle)
{
	if (IsValid(InWindowObject) == true)
	{
		OutWindowTitle = InWindowObject->WindowPtr.ToSharedRef().Get().GetTitle();
		return true;
	}

	else
	{
		OutWindowTitle = FText::FromString(TEXT(""));
		return false;
	}
}

FText UWindowSystemBPLibrary::GetMainWindowTitle()
{
	return GEngine->GameViewport->GetWindow().ToSharedRef().Get().GetTitle();
}

bool UWindowSystemBPLibrary::GetWindowTag(UPARAM(ref)UWindowObject*& InWindowObject, FName& OutWindowTag)
{
	if (IsValid(InWindowObject) == true)
	{
		if (IsValid(InWindowObject) == true)
		{
			OutWindowTag = InWindowObject->WindowTag;
			return true;
		}

		else
		{
			return false;
		}
	}

	else
	{
		return false;
	}
}

bool UWindowSystemBPLibrary::TakeSSWindow(UPARAM(ref)UWindowObject*& InWindowObject, UTextureRenderTarget2D*& OutTextureRenderTarget2D)
{
	if (IsValid(InWindowObject) == true)
	{
		if (InWindowObject->WindowPtr.IsValid() == true)
		{
			UWindowSystemBPLibrary::BringWindowFront(InWindowObject, false);

			FVector2D WindowSize = InWindowObject->WindowPtr->GetClientSizeInScreen();
			UTextureRenderTarget2D* TextureTarget = FWidgetRenderer::CreateTargetFor(WindowSize, TextureFilter::TF_Default, false);

			FWidgetRenderer* WidgetRenderer = new FWidgetRenderer(true);
			WidgetRenderer->DrawWidget(TextureTarget, InWindowObject->ContentWidget->TakeWidget(), WindowSize, 0, false);

			if (IsValid(TextureTarget) == true)
			{
				OutTextureRenderTarget2D = TextureTarget;

				return true;
			}

			else
			{
				return false;
			}
		}

		else
		{
			return false;
		}
	}

	else
	{
		return false;
	}
}

void UWindowSystemBPLibrary::SelectFileFromDialog(FDelegateOpenFile DelegateFileNames, const FString InDialogName, const FString InOkLabel, const FString InDefaultPath, TMap<FString, FString> InExtensions, int32 DefaultExtensionIndex, bool bIsNormalizeOutputs, bool bAllowFolderSelection)
{
	AsyncTask(ENamedThreads::AnyNormalThreadNormalTask, [DelegateFileNames, InDialogName, InOkLabel, InDefaultPath, InExtensions, DefaultExtensionIndex, bIsNormalizeOutputs, bAllowFolderSelection]()
		{
			IFileOpenDialog* FileOpenDialog;
			HRESULT FileDialogInstance = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_ALL, IID_PPV_ARGS(&FileOpenDialog));

			IShellItemArray* ShellItems;
			TArray<FString> Array_FilePaths;

			// If dialog instance successfully created.
			if (SUCCEEDED(FileDialogInstance))
			{
				// https://stackoverflow.com/questions/70174174/c-com-comdlg-filterspec-array-overrun
				int32 ExtensionCount = InExtensions.Num();
				COMDLG_FILTERSPEC* ExtensionArray = new COMDLG_FILTERSPEC[ExtensionCount];
				COMDLG_FILTERSPEC* EachExtension = ExtensionArray;

				TArray<FString> ExtensionKeys;
				InExtensions.GetKeys(ExtensionKeys);

				TArray<FString> ExtensionValues;
				InExtensions.GenerateValueArray(ExtensionValues);

				for (int32 ExtensionIndex = 0; ExtensionIndex < ExtensionCount; ExtensionIndex++)
				{
					EachExtension->pszName = *ExtensionKeys[ExtensionIndex];
					EachExtension->pszSpec = *ExtensionValues[ExtensionIndex];
					++EachExtension;
				}

				FileOpenDialog->SetFileTypes(ExtensionCount, ExtensionArray);

				// Starts from 1
				FileOpenDialog->SetFileTypeIndex(DefaultExtensionIndex + 1);

				DWORD dwOptions;
				FileOpenDialog->GetOptions(&dwOptions);

				if (bAllowFolderSelection == true)
				{
					// https://learn.microsoft.com/en-us/windows/win32/api/shobjidl_core/ne-shobjidl_core-_fileopendialogoptions
					FileOpenDialog->SetOptions(dwOptions | FOS_PICKFOLDERS | FOS_ALLOWMULTISELECT | FOS_FILEMUSTEXIST | FOS_OKBUTTONNEEDSINTERACTION);
				}

				else
				{
					// https://learn.microsoft.com/en-us/windows/win32/api/shobjidl_core/ne-shobjidl_core-_fileopendialogoptions
					FileOpenDialog->SetOptions(dwOptions | FOS_ALLOWMULTISELECT | FOS_FILEMUSTEXIST | FOS_OKBUTTONNEEDSINTERACTION);
				}

				if (InDialogName.IsEmpty() != true)
				{
					FileOpenDialog->SetTitle(*InDialogName);
				}

				if (InOkLabel.IsEmpty() != true)
				{
					FileOpenDialog->SetOkButtonLabel(*InOkLabel);
				}

				if (InDefaultPath.IsEmpty() != true)
				{
					FString DefaultPathString = InDefaultPath;

					FPaths::MakePlatformFilename(DefaultPathString);

					IShellItem* DefaultFolder = NULL;
					HRESULT DefaultPathResult = SHCreateItemFromParsingName(*DefaultPathString, nullptr, IID_PPV_ARGS(&DefaultFolder));

					if (SUCCEEDED(DefaultPathResult))
					{
						FileOpenDialog->SetFolder(DefaultFolder);
						DefaultFolder->Release();
					}
				}

				HWND WindowHandle = reinterpret_cast<HWND>(GEngine->GameViewport->GetWindow()->GetNativeWindow()->GetOSWindowHandle());
				FileDialogInstance = FileOpenDialog->Show(WindowHandle);

				// If dialog successfully showed up.
				if (SUCCEEDED(FileDialogInstance))
				{
					FileDialogInstance = FileOpenDialog->GetResults(&ShellItems);

					// Is results got.
					if (SUCCEEDED(FileDialogInstance))
					{
						DWORD ItemCount;
						ShellItems->GetCount(&ItemCount);

						for (DWORD ItemIndex = 0; ItemIndex < ItemCount; ItemIndex++)
						{
							IShellItem* EachItem;
							ShellItems->GetItemAt(ItemIndex, &EachItem);

							PWSTR EachFilePathSTR = NULL;
							EachItem->GetDisplayName(SIGDN_FILESYSPATH, &EachFilePathSTR);

							FString EachFilePath = EachFilePathSTR;

							if (bIsNormalizeOutputs == true)
							{
								FPaths::NormalizeFilename(EachFilePath);
							}

							Array_FilePaths.Add(EachFilePath);

							EachItem->Release();
						}

						ShellItems->Release();
						FileOpenDialog->Release();
						CoUninitialize();

						AsyncTask(ENamedThreads::GameThread, [DelegateFileNames, Array_FilePaths, bAllowFolderSelection]()
							{
								if (Array_FilePaths.IsEmpty() == false)
								{
									FSelectedFiles SelectedFiles;
									SelectedFiles.IsSuccessfull = true;
									SelectedFiles.IsFolder = bAllowFolderSelection;
									SelectedFiles.Strings = Array_FilePaths;

									DelegateFileNames.ExecuteIfBound(SelectedFiles);
								}

								else
								{
									FSelectedFiles SelectedFiles;
									SelectedFiles.IsSuccessfull = false;
									SelectedFiles.IsFolder = bAllowFolderSelection;

									DelegateFileNames.ExecuteIfBound(SelectedFiles);
								}
							}
						);

					}

					// Function couldn't get results.
					else
					{
						AsyncTask(ENamedThreads::GameThread, [DelegateFileNames, ShellItems, FileOpenDialog, bAllowFolderSelection]()
							{
								FileOpenDialog->Release();
								CoUninitialize();

								FSelectedFiles SelectedFiles;
								SelectedFiles.IsSuccessfull = false;
								SelectedFiles.IsFolder = bAllowFolderSelection;

								DelegateFileNames.ExecuteIfBound(SelectedFiles);
							}
						);
					}
				}

				// Dialog didn't show up.
				else
				{
					AsyncTask(ENamedThreads::GameThread, [DelegateFileNames, FileOpenDialog, ShellItems, bAllowFolderSelection]()
						{
							FileOpenDialog->Release();
							CoUninitialize();

							FSelectedFiles SelectedFiles;
							SelectedFiles.IsSuccessfull = false;
							SelectedFiles.IsFolder = bAllowFolderSelection;

							DelegateFileNames.ExecuteIfBound(SelectedFiles);
						}
					);
				}
			}

			// Function couldn't create dialog.
			else
			{
				AsyncTask(ENamedThreads::GameThread, [DelegateFileNames, FileOpenDialog, ShellItems, bAllowFolderSelection]()
					{
						FileOpenDialog->Release();
						CoUninitialize();

						FSelectedFiles SelectedFiles;
						SelectedFiles.IsSuccessfull = false;
						SelectedFiles.IsFolder = bAllowFolderSelection;

						DelegateFileNames.ExecuteIfBound(SelectedFiles);
					}
				);
			}
		}
	);
}

void UWindowSystemBPLibrary::SaveFileDialog(FDelegateSaveFile DelegateSaveFile, const FString InDialogName, const FString InOkLabel, const FString InDefaultPath, TMap<FString, FString> InExtensions, int32 DefaultExtensionIndex, bool bIsNormalizeOutputs)
{
	AsyncTask(ENamedThreads::AnyNormalThreadNormalTask, [DelegateSaveFile, InDialogName, InOkLabel, InDefaultPath, InExtensions, DefaultExtensionIndex, bIsNormalizeOutputs]()
		{
			IFileSaveDialog* SaveFileDialog;
			HRESULT SaveDialogInstance = CoCreateInstance(CLSID_FileSaveDialog, NULL, CLSCTX_ALL, IID_PPV_ARGS(&SaveFileDialog));

			IShellItem* ShellItem;

			// If dialog instance successfully created.
			if (SUCCEEDED(SaveDialogInstance))
			{
				// https://stackoverflow.com/questions/70174174/c-com-comdlg-filterspec-array-overrun
				int32 ExtensionCount = InExtensions.Num();
				COMDLG_FILTERSPEC* ExtensionArray = new COMDLG_FILTERSPEC[ExtensionCount];
				COMDLG_FILTERSPEC* EachExtension = ExtensionArray;

				TArray<FString> ExtensionKeys;
				InExtensions.GetKeys(ExtensionKeys);

				TArray<FString> ExtensionValues;
				InExtensions.GenerateValueArray(ExtensionValues);

				for (int32 ExtensionIndex = 0; ExtensionIndex < ExtensionCount; ExtensionIndex++)
				{
					EachExtension->pszName = *ExtensionKeys[ExtensionIndex];
					EachExtension->pszSpec = *ExtensionValues[ExtensionIndex];
					++EachExtension;
				}

				SaveFileDialog->SetFileTypes(ExtensionCount, ExtensionArray);

				// Starts from 1
				SaveFileDialog->SetFileTypeIndex(DefaultExtensionIndex + 1);

				DWORD dwOptions;
				SaveFileDialog->GetOptions(&dwOptions);

				if (InDialogName.IsEmpty() != true)
				{
					SaveFileDialog->SetTitle(*InDialogName);
				}

				if (InOkLabel.IsEmpty() != true)
				{
					SaveFileDialog->SetOkButtonLabel(*InOkLabel);
				}

				if (InDefaultPath.IsEmpty() != true)
				{
					FString DefaultPathString = InDefaultPath;

					FPaths::MakePlatformFilename(DefaultPathString);

					IShellItem* DefaultFolder = NULL;
					HRESULT DefaultPathResult = SHCreateItemFromParsingName(*DefaultPathString, nullptr, IID_PPV_ARGS(&DefaultFolder));

					if (SUCCEEDED(DefaultPathResult))
					{
						SaveFileDialog->SetFolder(DefaultFolder);
						DefaultFolder->Release();
					}
				}

				HWND WindowHandle = reinterpret_cast<HWND>(GEngine->GameViewport->GetWindow()->GetNativeWindow()->GetOSWindowHandle());
				SaveDialogInstance = SaveFileDialog->Show(WindowHandle);

				// Dialog didn't show up.
				if (SUCCEEDED(SaveDialogInstance))
				{
					SaveFileDialog->GetResult(&ShellItem);

					UINT FileTypeIndex = 0;
					SaveFileDialog->GetFileTypeIndex(&FileTypeIndex);
					FString ExtensionString = ExtensionValues[FileTypeIndex - 1];

					TArray<FString> ExtensionParts;
					ExtensionString.ParseIntoArray(ExtensionParts, TEXT("."), true);

					PWSTR pFileName;
					ShellItem->GetDisplayName(SIGDN_FILESYSPATH, &pFileName);

					FString FilePath = FString(pFileName) + TEXT(".") + ExtensionParts[1];

					if (bIsNormalizeOutputs == true)
					{
						FPaths::NormalizeFilename(FilePath);
					}

					ShellItem->Release();
					SaveFileDialog->Release();
					CoUninitialize();

					AsyncTask(ENamedThreads::GameThread, [DelegateSaveFile, FilePath]()
						{
							DelegateSaveFile.ExecuteIfBound(true, FilePath);
						}
					);
				}

				else
				{
					AsyncTask(ENamedThreads::GameThread, [DelegateSaveFile]()
						{
							DelegateSaveFile.ExecuteIfBound(false, TEXT(""));
						}
					);
				}
			}

			// Function couldn't create dialog.
			else
			{
				AsyncTask(ENamedThreads::GameThread, [DelegateSaveFile]()
					{
						DelegateSaveFile.ExecuteIfBound(false, TEXT(""));
					}
				);
			}
		}
	);
}

bool UWindowSystemBPLibrary::GetFolderContents(TArray<FFolderContent>& OutContents, FString& ErrorCode, FString InPath)
{
	if (InPath.IsEmpty() == true)
	{
		ErrorCode = "Path is empty.";
		return false;
	}

	if (FPaths::DirectoryExists(InPath) == false)
	{
		ErrorCode = "Directory doesn't exist.";
		return false;
	}

	class FFindDirectories : public IPlatformFile::FDirectoryVisitor
	{
	public:

		TArray<FFolderContent> Array_Contents;

		FFindDirectories() {}
		virtual bool Visit(const TCHAR* CharPath, bool bIsDirectory) override
		{
			if (bIsDirectory == true)
			{
				FFolderContent EachContent;

				FString Path = FString(CharPath) + "/";
				FPaths::NormalizeDirectoryName(Path);

				EachContent.Path = Path;
				EachContent.Name = FPaths::GetBaseFilename(Path);
				EachContent.bIsFile = false;

				Array_Contents.Add(EachContent);
			}

			else if (bIsDirectory == false)
			{
				FFolderContent EachContent;

				EachContent.Path = CharPath;
				EachContent.Name = FPaths::GetCleanFilename(CharPath);
				EachContent.bIsFile = true;

				Array_Contents.Add(EachContent);
			}

			return true;
		}
	};

	FFindDirectories GetFoldersVisitor;
	FPlatformFileManager::Get().GetPlatformFile().IterateDirectory(*InPath, GetFoldersVisitor);

	OutContents = GetFoldersVisitor.Array_Contents;

	return true;
}

void UWindowSystemBPLibrary::SearchInFolder(FDelegateSearch DelegateSearch, FString InPath, FString InSearch, bool bSearchExact)
{
	if (InPath.IsEmpty() == true)
	{
		FContentArrayContainer EmptyContainer;
		DelegateSearch.Execute(false, "Path is empty.", EmptyContainer);

		return;
	}

	if (InSearch.IsEmpty() == true)
	{
		FContentArrayContainer EmptyContainer;
		DelegateSearch.Execute(false, "Search is empty.", EmptyContainer);

		return;
	}

	if (FPaths::DirectoryExists(InPath) == false)
	{
		FContentArrayContainer EmptyContainer;
		DelegateSearch.Execute(false, "Directory doesn't exist.", EmptyContainer);

		return;
	}

	AsyncTask(ENamedThreads::AnyNormalThreadNormalTask, [DelegateSearch, InPath, InSearch, bSearchExact]()
		{
			IFileManager& FileManager = FFileManagerGeneric::Get();

			TArray<FString> Array_Contents;
			TArray<FFolderContent> Array_Founds;

			FileManager.FindFilesRecursive(Array_Contents, *InPath, TEXT("*"), true, true, false);

			for (int32 ContentIndex = 0; ContentIndex < Array_Contents.Num(); ContentIndex++)
			{
				FFolderContent EachContent;

				if (bSearchExact == true)
				{
					if (FPaths::GetBaseFilename(Array_Contents[ContentIndex]) == InSearch)
					{
						EachContent.Name = FPaths::GetCleanFilename(Array_Contents[ContentIndex]);
						EachContent.Path = Array_Contents[ContentIndex];
						EachContent.bIsFile = FPaths::FileExists(Array_Contents[ContentIndex]);

						Array_Founds.Add(EachContent);
					}
				}

				else
				{
					if (FPaths::GetBaseFilename(Array_Contents[ContentIndex]).Contains(InSearch) == true)
					{
						EachContent.Name = FPaths::GetCleanFilename(Array_Contents[ContentIndex]);
						EachContent.Path = Array_Contents[ContentIndex];
						EachContent.bIsFile = FPaths::FileExists(Array_Contents[ContentIndex]);

						Array_Founds.Add(EachContent);
					}
				}
			}

			AsyncTask(ENamedThreads::GameThread, [DelegateSearch, Array_Founds]()
				{
					FContentArrayContainer ArrayContainer;
					ArrayContainer.OutContents = Array_Founds;

					DelegateSearch.ExecuteIfBound(true, "Success", ArrayContainer);
				}
			);
		}
	);
}