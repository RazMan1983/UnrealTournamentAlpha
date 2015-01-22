// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#pragma once

/** 
 * Displays the name area which is not recreated when the detail view is refreshed
 */
class SDetailNameArea : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS( SDetailNameArea ){}
		SLATE_EVENT( FOnClicked, OnLockButtonClicked )
		SLATE_ARGUMENT( bool, ShowLockButton )
		SLATE_ARGUMENT( bool, ShowActorLabel )
		SLATE_ATTRIBUTE( bool, IsLocked )
		SLATE_ATTRIBUTE( bool, SelectionTip )
	SLATE_END_ARGS()

	void Construct( const FArguments& InArgs, const TArray< TWeakObjectPtr<UObject> >* SelectedObjects );

	/**
	 * Refreshes the name area when selection changes
	 *
	 * @param SelectedObjects	the new list of selected objects
	 */
	void Refresh( const TArray< TWeakObjectPtr<UObject> >& SelectedObjects );
	
	/**
	 * Refreshes the name area when selection changes
	 *
	 * @param SelectedActors	the new list of selected actors
	 */
	void Refresh( const TArray< TWeakObjectPtr<AActor> >& SelectedActors );

private:
	/** @return the Slate brush to use for the lock image */
	const FSlateBrush* OnGetLockButtonImageResource() const;

	TSharedRef< SWidget > BuildObjectNameArea( const TArray< TWeakObjectPtr<UObject> >& SelectedObjects );

	void BuildObjectNameAreaSelectionLabel( TSharedRef< SHorizontalBox > SelectionLabelBox, const TWeakObjectPtr<UObject> ObjectWeakPtr, const int32 NumSelectedObjects );

	void OnEditBlueprintClicked( TWeakObjectPtr<UBlueprint> InBlueprint, TWeakObjectPtr<UObject> InAsset );
private:
	FOnClicked OnLockButtonClicked;
	TAttribute<bool> IsLocked;
	TAttribute<bool> SelectionTip;
	bool bShowLockButton;
	bool bShowActorLabel;
};