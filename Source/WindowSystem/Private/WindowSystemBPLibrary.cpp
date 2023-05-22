// Copyright Epic Games, Inc. All Rights Reserved.

#include "WindowSystemBPLibrary.h"
#include "WindowSystem.h"

// UE Includes.
#include "Slate/WidgetRenderer.h"		// Widget to Texture 2D

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