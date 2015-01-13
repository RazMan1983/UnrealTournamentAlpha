// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.
#pragma once

#include "TAttributeProperty.h"
#include "SlateBasics.h"
#include "SUWDialog.h"

#if !UE_SERVER

class SUWCreateGameDialog : public SUWDialog, public FGCObject
{
public:
	SLATE_BEGIN_ARGS(SUWCreateGameDialog)
	{}
	SLATE_ARGUMENT(TWeakObjectPtr<UUTLocalPlayer>, PlayerOwner)
	SLATE_ARGUMENT(bool, IsOnline) // whether the dialog is for a server or a local botmatch
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);
protected:
	enum EServerStartMode
	{
		SERVER_Standalone,
		SERVER_Listen,
		SERVER_Dedicated,
	};

	TSharedPtr<SVerticalBox> GameConfigPanel;

	TArray< TSharedPtr<FString> > AllMaps;
	TSharedPtr< SComboBox< TSharedPtr<FString> > > MapList;
	TSharedPtr<STextBlock> SelectedMap;
	TArray<UClass*> AllGametypes;
	TSharedPtr< SComboBox<UClass*> > GameList;
	TSharedPtr<STextBlock> SelectedGameName;
	TSubclassOf<AUTGameMode> SelectedGameClass;
	TSharedPtr<STextBlock> MapAuthor;
	TSharedPtr<STextBlock> MapRecommendedPlayers;
	TSharedPtr<STextBlock> MapDesc;
	TArray<UClass*> MutatorListAvailable, MutatorListEnabled;
	TSharedPtr< SListView<UClass*> > AvailableMutators;
	TSharedPtr< SListView<UClass*> > EnabledMutators;

	// container for pointers to TAttributeProperty objects linked directly to setting properties
	TArray< TSharedPtr<TAttributePropertyBase> > PropertyLinks;

	// holders for pointers to game config properties so the objects don't die and invalidate the delegates
	TArray< TSharedPtr<TAttributePropertyBase> > GameConfigProps;

	void OnMapSelected(TSharedPtr<FString> NewSelection, ESelectInfo::Type SelectInfo);
	TSharedRef<SWidget> GenerateGameNameWidget(UClass* InItem);
	void OnGameSelected(UClass* NewSelection, ESelectInfo::Type SelectInfo);
	virtual FReply StartClick(EServerStartMode Mode);
	FReply StartListenClick();
	FReply StartDedicatedClick();
	FReply CancelClick();
	TSharedRef<ITableRow> GenerateMutatorListRow(UClass* MutatorType, const TSharedRef<STableViewBase>& OwningList);
	FReply AddMutator();
	FReply RemoveMutator();
	FReply ConfigureMutator();
	FReply ConfigureBots();

	virtual void AddReferencedObjects(FReferenceCollector& Collector) override
	{
		Collector.AddReferencedObjects(AllGametypes);
	}
};

#endif