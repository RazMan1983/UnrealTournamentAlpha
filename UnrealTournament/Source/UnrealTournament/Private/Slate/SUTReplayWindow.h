// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.
#pragma once

#include "SlateBasics.h"
#include "Slate/SlateGameResources.h"
#include "Runtime/NetworkReplayStreaming/NetworkReplayStreaming/Public/NetworkReplayStreaming.h"

#if !UE_SERVER

class UNREALTOURNAMENT_API SUTReplayWindow : public SCompoundWidget
{
	SLATE_BEGIN_ARGS(SUTReplayWindow)
	{}

	SLATE_ARGUMENT(TWeakObjectPtr<class UUTLocalPlayer>, PlayerOwner)
	SLATE_ARGUMENT(TWeakObjectPtr<class UDemoNetDriver>, DemoNetDriver)
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);

	inline TWeakObjectPtr<class UUTLocalPlayer> GetPlayerOwner()
	{
		return PlayerOwner;
	}

	inline TWeakObjectPtr<class UDemoNetDriver> GetDemoNetDriver()
	{
		return DemoNetDriver;
	}

	virtual void Tick(const FGeometry & AllottedGeometry, const double InCurrentTime, const float InDeltaTime) override;
	virtual void OnArrangeChildren(const FGeometry& AllottedGeometry, FArrangedChildren& ArrangedChildren) const override;

protected:

	float RecordTimeStart;
	float RecordTimeStop;
	FReply OnMarkRecordStartClicked();
	FReply OnMarkRecordStopClicked();
	FReply OnRecordButtonClicked();
	void RecordSeekCompleted(bool bSucceeded);
	FReply OnScreenshotButtonClicked();
	FReply OnScreenshotConfigButtonClicked();
	void ScreenshotConfigResult(TSharedPtr<SCompoundWidget> Dialog, uint16 ButtonID);

	TSharedPtr<class SUTProgressSlider> TimeSlider;
	TSharedPtr<class SButton> RecordButton;
	TSharedPtr<class SButton> MarkStartButton;
	TSharedPtr<class SButton> MarkEndButton;
	TSharedPtr<class SBorder> TimeBar;

	TSharedPtr< SComboBox< TSharedPtr<FString> > > BookmarksComboBox;
	TArray<TSharedPtr<FString>> BookmarkNameList;
	TSharedPtr<STextBlock> SelectedBookmark;
	void OnBookmarkSetSelected(TSharedPtr<FString> NewSelection, ESelectInfo::Type SelectInfo);
	void RefreshBookmarksComboBox();
	int32 LastSpectedPlayerID;
	/** utility to generate a simple text widget for list and combo boxes given a string value */
	TSharedRef<SWidget> GenerateStringListWidget(TSharedPtr<FString> InItem);
	void OnKillBookmarksSelected();
	void OnMultiKillBookmarksSelected();
	void OnSpreeKillBookmarksSelected();
	void OnFlagReturnsBookmarksSelected();
	FString GetSpectatedPlayerID();

	//Time remaining to auto hide the time bar
	float HideTimeBarTime;
	FLinearColor GetTimeBarColor() const;
	FSlateColor GetTimeBarBorderColor() const;

	void OnSetTimeSlider(float NewValue);
	float GetTimeSlider() const;
	float GetTimeSliderLength() const;

	void OnSetSpeedSlider(float NewValue);
	TOptional<float> GetSpeedSlider() const;

	float SpeedMin;
	float SpeedMax;

	FText GetTimeText() const;
	
	const FSlateBrush* GetPlayButtonBrush() const;
	FReply OnPlayPauseButtonClicked();

	//Mouse interaction overrides so it can send input to UUTHUDWidget_SpectatorSlideOut
	virtual FReply OnMouseMove(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
	virtual FReply OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
	virtual FReply OnMouseButtonDoubleClick(const FGeometry& InMyGeometry, const FPointerEvent& InMouseEvent) override;
	bool GetGameMousePosition(FVector2D& MousePosition) const;
	virtual bool MouseClickHUD();
	
	void KillsEnumerated(const FReplayEventList& ReplayEventList, bool bSucceeded);
	void FlagCapsEnumerated(const FReplayEventList& ReplayEventList, bool bSucceeded);
	void FlagReturnsEnumerated(const FReplayEventList& ReplayEventList, bool bSucceeded);
	void FlagDenyEnumerated(const FReplayEventList& ReplayEventList, bool bSucceeded);
	void MultiKillsEnumerated(const FReplayEventList& ReplayEventList, bool bSucceeded);
	void SpreeKillsEnumerated(const FReplayEventList& ReplayEventList, bool bSucceeded);

	TArray<FReplayEventListItem> KillEvents;
	TArray<FReplayEventListItem> FlagCapEvents;
	TArray<FReplayEventListItem> FlagDenyEvents;
	TArray<FReplayEventListItem> FlagReturnEvents;
	TArray<FReplayEventListItem> MultiKillEvents;
	TArray<FReplayEventListItem> SpreeKillEvents;

	bool bDrawTooltip;
	float TooltipTime;
	FVector2D ToolTipPos;

	FText GetTooltipText() const;
	FVector2D GetTimeTooltipSize() const;
	FVector2D GetTimeTooltipPosition() const;

	EVisibility GetVis() const;

private:
	TWeakObjectPtr<class UUTLocalPlayer> PlayerOwner;
	TWeakObjectPtr<class UDemoNetDriver> DemoNetDriver;
};

#endif
