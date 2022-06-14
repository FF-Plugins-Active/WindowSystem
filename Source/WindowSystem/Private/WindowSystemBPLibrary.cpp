// Copyright Epic Games, Inc. All Rights Reserved.

#include "WindowSystemBPLibrary.h"
#include "WindowSystem.h"

// UE Includes.
#include "Slate/WidgetRenderer.h"		// Widget to Texture 2D

#include "GenericPlatform/GenericApplication.h"

UWindowSystemBPLibrary::UWindowSystemBPLibrary(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{

}

FVector2D UWindowSystemBPLibrary::GetDesktopResolution()
{
	FVector2D DesktopResolution;
	DesktopResolution.X = GetSystemMetrics(SM_CXSCREEN);
	DesktopResolution.Y = GetSystemMetrics(SM_CYSCREEN);

	return DesktopResolution;
}

void UWindowSystemBPLibrary::IsWindowTopMost(UPARAM(ref)UWindowObject*& InWindowObject, bool& bIsTopMost)
{
	if (IsValid(InWindowObject) == true)
	{
		if (InWindowObject->WindowPtr.IsValid() == true)
		{
			if (InWindowObject->WindowPtr->IsTopmostWindow() == true)
			{
				bIsTopMost = true;
			}
		}
	}
}

void UWindowSystemBPLibrary::IsWindowHovered(UPARAM(ref)UWindowObject*& InWindowObject, FDelegateHover DelegateHover)
{
	if (IsValid(InWindowObject) == true)
	{
		if (InWindowObject->WindowPtr.IsValid() == true)
		{
			if (InWindowObject->WindowPtr.ToSharedRef().Get().IsDirectlyHovered() == true)
			{
				DelegateHover.Execute(true);
			}
		}
	}
}

bool UWindowSystemBPLibrary::BringWindowFront(UPARAM(ref)UWindowObject*& InWindowObject, bool bFlashWindow)
{
	if (IsValid(InWindowObject) == true)
	{
		if (InWindowObject->WindowPtr.IsValid() == true)
		{
			InWindowObject->WindowPtr.Get()->BringToFront();

			if (bFlashWindow == true)
			{
				InWindowObject->WindowPtr.Get()->FlashWindow();
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

bool UWindowSystemBPLibrary::SetWindowOpacity(UPARAM(ref)UWindowObject*& InWindowObject, float NewOpacity)
{
	if (IsValid(InWindowObject) == true)
	{
		if (InWindowObject->WindowPtr.IsValid() == true)
		{
			InWindowObject->WindowPtr->SetOpacity(NewOpacity);

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

bool UWindowSystemBPLibrary::TakeSSWidget(UUserWidget* InWidget, FVector2D InSize, UTextureRenderTarget2D*& OutTextureRenderTarget2D)
{
	if (IsValid(InWidget) == true)
	{
		UTextureRenderTarget2D* TextureTarget = FWidgetRenderer::CreateTargetFor(InSize, TextureFilter::TF_Default, false);

		FWidgetRenderer* WidgetRenderer = new FWidgetRenderer(true);
		WidgetRenderer->DrawWidget(TextureTarget, InWidget->TakeWidget(), InSize, 0, false);

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