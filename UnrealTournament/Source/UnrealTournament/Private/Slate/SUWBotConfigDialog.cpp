// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.
#include "../Public/UnrealTournament.h"
#include "../Public/UTLocalPlayer.h"
#include "../Public/UTBotConfig.h"
#include "SUWBotConfigDialog.h"
#include "SUWInputBox.h"

SVerticalBox::FSlot& SUWBotConfigDialog::CreateBotSlider(const FText& Desc, TSharedPtr<SSlider>& Slider)
{
	return SVerticalBox::Slot()
		.AutoHeight()
		.Padding(FMargin(10.0f, 5.0f, 10.0f, 5.0f))
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			.Padding(FMargin(10.0f, 5.0f, 10.0f, 5.0f))
			[
				SNew(STextBlock)
				.ColorAndOpacity(FLinearColor::White)
				.Text(Desc)
			]
			+ SHorizontalBox::Slot()
			.Padding(FMargin(10.0f, 5.0f, 10.0f, 5.0f))
			[
				SAssignNew(Slider, SSlider)
			]
		];
}

void SUWBotConfigDialog::Construct(const FArguments& InArgs)
{
	UUTBotConfig::StaticClass()->GetDefaultObject<UUTBotConfig>()->Load();

	CurrentlyEditedIndex = INDEX_NONE;

	SUWDialog::Construct(SUWDialog::FArguments().PlayerOwner(InArgs._PlayerOwner));

	GameClass = InArgs._GameClass;
	MaxSelectedBots = InArgs._NumBots;

	// list of weapons for favorite weapon selection
	{
		TArray<FAssetData> AssetList;
		GetAllBlueprintAssetData(AUTWeapon::StaticClass(), AssetList);
		for (const FAssetData& Asset : AssetList)
		{
			static FName NAME_GeneratedClass(TEXT("GeneratedClass"));
			const FString* ClassPath = Asset.TagsAndValues.Find(NAME_GeneratedClass);
			if (ClassPath != NULL)
			{
				UClass* TestClass = LoadObject<UClass>(NULL, **ClassPath);
				if (TestClass != NULL && !TestClass->HasAnyClassFlags(CLASS_Abstract) && TestClass->IsChildOf(AUTWeapon::StaticClass()))
				{
					WeaponList.Add(TestClass);
				}
			}
		}
	}
	WeaponList.Add(NULL); // we want a NULL for no favorite weapon

	const TArray<FBotCharacter>& AllBots = GetDefault<UUTBotConfig>()->BotCharacters;
	for (const FBotCharacter& Bot : AllBots)
	{
		BotNames.Add(MakeShareable(new FString(Bot.PlayerName)));
	}
	// failsafe
	if (BotNames.Num() == 0)
	{
		BotNames.Add(MakeShareable(new FString(TEXT("Malcolm"))));
	}

	static FSlateImageBrush WhiteBar(FPaths::GameContentDir() / TEXT("RestrictedAssets/Slate") / TEXT("UWindows.Standard.White.png"), FVector2D(32, 4), FLinearColor(0.75f, 0.75f, 0.75f, 0.75f));

	ChildSlot
	.VAlign(VAlign_Center)
	.HAlign(HAlign_Center)
	[
		SNew(SBorder)
		.VAlign(VAlign_Center)
		.HAlign(HAlign_Center)
		.BorderImage(SUWindowsStyle::Get().GetBrush("UWindows.Standard.MenuBar.Background"))
		[
			SNew(SScrollBox)
			+ SScrollBox::Slot()
			.Padding(FMargin(0.0f, 5.0f, 0.0f, 5.0f))
			[
				SNew(SVerticalBox)
				+ SVerticalBox::Slot()
				.AutoHeight()
				[
					SNew(SHorizontalBox)
					+ SHorizontalBox::Slot()
					.Padding(FMargin(10.0f, 5.0f, 10.0f, 5.0f))
					.AutoWidth()
					.VAlign(VAlign_Top)
					.HAlign(HAlign_Center)
					[
						SNew(SVerticalBox)
						+ SVerticalBox::Slot()
						.AutoHeight()
						.VAlign(VAlign_Top)
						.HAlign(HAlign_Center)
						.Padding(FMargin(10.0f, 5.0f, 10.0f, 5.0f))
						[
							SNew(STextBlock)
							.ColorAndOpacity(FLinearColor::White)
							.Text(NSLOCTEXT("SUWBotConfigDialog", "BotsToAdd", "Bots to Include:"))
						]
						+ SVerticalBox::Slot()
						.HAlign(HAlign_Center)
						[
							SNew(SBox)
							.WidthOverride(200.0f)
							.HeightOverride(800.0f)
							[
								SNew(SBorder)
								.Content()
								[
									SAssignNew(BotIncludeList, SListView< TSharedPtr<FString> >)
									.SelectionMode(ESelectionMode::Multi)
									.ListItemsSource(&BotNames)
									.OnGenerateRow(this, &SUWBotConfigDialog::GenerateBotListRow)
								]
							]
						]
					]
					+ SHorizontalBox::Slot()
					[
						SNew(SVerticalBox)
						+ SVerticalBox::Slot()
						.AutoHeight()
						.HAlign(HAlign_Center)
						.Padding(FMargin(10.0f, 5.0f, 10.0f, 5.0f))
						[
							SNew(STextBlock)
							.ColorAndOpacity(FLinearColor::White)
							.Text(NSLOCTEXT("SUWBotConfigDialog", "ConfigureBot", "Configure Bot"))
						]
						+ SVerticalBox::Slot()
						.AutoHeight()
						.Padding(FMargin(10.0f, 5.0f, 10.0f, 5.0f))
						[
							SAssignNew(BotToConfigure, SComboBox< TSharedPtr<FString> >)
							.Method(SMenuAnchor::UseCurrentWindow)
							.InitiallySelectedItem(BotNames[0])
							.ComboBoxStyle(SUWindowsStyle::Get(), "UWindows.Standard.Button")
							.ButtonStyle(SUWindowsStyle::Get(), "UWindows.Standard.Button")
							.OptionsSource(&BotNames)
							.OnGenerateWidget(this, &SUWBotConfigDialog::GenerateBotNameWidget)
							.OnSelectionChanged(this, &SUWBotConfigDialog::OnCustomizedBotChange)
							.Content()
							[
								SAssignNew(CustomizedBotName, STextBlock)
								.MinDesiredWidth(200.0f)
							]
						]
						+ SVerticalBox::Slot()
						.AutoHeight()
						.Padding(FMargin(10.0f, 5.0f, 10.0f, 5.0f))
						[
							SNew(SHorizontalBox)
							+ SHorizontalBox::Slot()
							.Padding(FMargin(10.0f, 5.0f, 10.0f, 5.0f))
							[
								SNew(SButton)
								.HAlign(HAlign_Center)
								.ButtonStyle(SUWindowsStyle::Get(), "UWindows.Standard.Button")
								.ContentPadding(FMargin(5.0f, 5.0f, 5.0f, 5.0f))
								.Text(NSLOCTEXT("SUWBotConfigDialog", "CreateNew", "Create New Bot").ToString())
								.OnClicked(this, &SUWBotConfigDialog::NewBotClick)
							]
							+ SHorizontalBox::Slot()
							.Padding(FMargin(10.0f, 5.0f, 10.0f, 5.0f))
							[
								SNew(SButton)
								.HAlign(HAlign_Center)
								.ButtonStyle(SUWindowsStyle::Get(), "UWindows.Standard.Button")
								.ContentPadding(FMargin(5.0f, 5.0f, 5.0f, 5.0f))
								.Text(NSLOCTEXT("SUWBotConfigDialog", "Delete", "Delete Bot").ToString())
								.OnClicked(this, &SUWBotConfigDialog::DeleteBotClick)
							]
						]
						+ SVerticalBox::Slot()
						.AutoHeight()
						.Padding(FMargin(10.0f, 5.0f, 10.0f, 5.0f))
						[
							/*SNew(STextBlock)
							.ColorAndOpacity(FLinearColor::White)
							.Text(FString(TEXT("-------------------------------------------")))*/
							SNew(SImage)
							.Image(&WhiteBar)
						]
						+ CreateBotSlider(NSLOCTEXT("SUWBotConfigDialog", "SkillModifier", "Skill Modifier"), SkillModifier)
						+ CreateBotSlider(NSLOCTEXT("SUWBotConfigDialog", "Aggressiveness", "Aggresiveness"), Aggressiveness)
						+ CreateBotSlider(NSLOCTEXT("SUWBotConfigDialog", "Tactics", "Tactics"), Tactics)
						+ CreateBotSlider(NSLOCTEXT("SUWBotConfigDialog", "Jumpiness", "Jumpiness"), Jumpiness)
						+ CreateBotSlider(NSLOCTEXT("SUWBotConfigDialog", "MovementAbility", "Movement Ability"), MovementAbility)
						+ CreateBotSlider(NSLOCTEXT("SUWBotConfigDialog", "ReactionTime", "Reactions"), ReactionTime)
						+ CreateBotSlider(NSLOCTEXT("SUWBotConfigDialog", "Accuracy", "Accuracy"), Accuracy)
						+ CreateBotSlider(NSLOCTEXT("SUWBotConfigDialog", "Alertness", "Alertness"), Alertness)
						+ CreateBotSlider(NSLOCTEXT("SUWBotConfigDialog", "MapAwareness", "Map Awareness"), MapAwareness)
						+ SVerticalBox::Slot()
						.AutoHeight()
						.Padding(FMargin(10.0f, 5.0f, 10.0f, 5.0f))
						[
							SNew(SHorizontalBox)
							+ SHorizontalBox::Slot()
							.Padding(FMargin(10.0f, 5.0f, 10.0f, 5.0f))
							[
								SNew(STextBlock)
								.ColorAndOpacity(FLinearColor::White)
								.Text(NSLOCTEXT("SUWBotConfigDialog", "FavoriteWeapon", "Favorite Weapon"))
							]
							+ SHorizontalBox::Slot()
							.Padding(FMargin(10.0f, 5.0f, 10.0f, 5.0f))
							[
								SAssignNew(FavoriteWeapon, SComboBox<UClass*>)
								.Method(SMenuAnchor::UseCurrentWindow)
								.InitiallySelectedItem(WeaponList[0])
								.ComboBoxStyle(SUWindowsStyle::Get(), "UWindows.Standard.Button")
								.ButtonStyle(SUWindowsStyle::Get(), "UWindows.Standard.Button")
								.OptionsSource(&WeaponList)
								.OnGenerateWidget(this, &SUWBotConfigDialog::GenerateWeaponListWidget)
								.OnSelectionChanged(this, &SUWBotConfigDialog::OnFavoriteWeaponChange)
								.Content()
								[
									SAssignNew(SelectedFavWeapon, STextBlock)
									.MinDesiredWidth(200.0f)
								]
							]
						]
					]
				]
				+ SVerticalBox::Slot()
				.Padding(FMargin(10.0f, 5.0f, 10.0f, 5.0f))
				.AutoHeight()
				.HAlign(HAlign_Center)
				.VAlign(VAlign_Bottom)
				[
					SNew(SButton)
					.HAlign(HAlign_Center)
					.ButtonStyle(SUWindowsStyle::Get(), "UWindows.Standard.Button")
					.ContentPadding(FMargin(5.0f, 5.0f, 5.0f, 5.0f))
					.Text(NSLOCTEXT("SUWBotConfigDialog", "OK", "OK").ToString())
					.OnClicked(this, &SUWBotConfigDialog::OKClick)
				]
			]
		]
	];

	OnCustomizedBotChange(BotNames[0], ESelectInfo::Direct);
}

TSharedRef<ITableRow> SUWBotConfigDialog::GenerateBotListRow(TSharedPtr<FString> BotName, const TSharedRef<STableViewBase>& OwningList)
{
	return SNew(SSimpleMultiSelectTableRow< TSharedPtr<FString> >, OwningList)
		.Padding(5)
		[
			SNew(STextBlock)
			.ColorAndOpacity(FLinearColor::White)
			.Text(*BotName.Get())
		];
}
TSharedRef<SWidget> SUWBotConfigDialog::GenerateBotNameWidget(TSharedPtr<FString> BotName)
{
	return SNew(SBox)
		.Padding(5)
		[
			SNew(STextBlock)
			.ColorAndOpacity(FLinearColor::Black)
			.Text(*BotName.Get())
		];
}
TSharedRef<SWidget> SUWBotConfigDialog::GenerateWeaponListWidget(UClass* WeaponClass)
{
	checkSlow(WeaponClass == NULL || WeaponClass->IsChildOf(AUTWeapon::StaticClass()));
	return SNew(SBox)
		.Padding(5)
		[
			SNew(STextBlock)
			.ColorAndOpacity(FLinearColor::Black)
			.Text((WeaponClass == NULL) ? GNone : WeaponClass->GetDefaultObject<AUTWeapon>()->DisplayName)
		];
}

void SUWBotConfigDialog::SaveCustomizedBot()
{
	if (CurrentlyEditedIndex >= 0)
	{
		FBotCharacter& SaveBot = UUTBotConfig::StaticClass()->GetDefaultObject<UUTBotConfig>()->BotCharacters[CurrentlyEditedIndex];
		SaveBot.SkillModifier = ConvertFromSliderValue(SkillModifier->GetValue());
		SaveBot.Aggressiveness = ConvertFromSliderValue(Aggressiveness->GetValue());
		SaveBot.Tactics = ConvertFromSliderValue(Tactics->GetValue());
		SaveBot.Jumpiness = ConvertFromSliderValue(Jumpiness->GetValue());
		SaveBot.MovementAbility = ConvertFromSliderValue(MovementAbility->GetValue());
		SaveBot.ReactionTime = ConvertFromSliderValue(ReactionTime->GetValue());
		SaveBot.Accuracy = ConvertFromSliderValue(Accuracy->GetValue());
		SaveBot.Alertness = ConvertFromSliderValue(Alertness->GetValue());
		SaveBot.MapAwareness = ConvertFromSliderValue(MapAwareness->GetValue());
		SaveBot.FavoriteWeapon = (FavoriteWeapon->GetSelectedItem() != NULL) ? FavoriteWeapon->GetSelectedItem()->GetFName() : NAME_None;
		UUTBotConfig::StaticClass()->GetDefaultObject<UUTBotConfig>()->Save();
	}
}

void SUWBotConfigDialog::OnFavoriteWeaponChange(UClass* WeaponClass, ESelectInfo::Type SelectInfo)
{
	checkSlow(WeaponClass == NULL || WeaponClass->IsChildOf(AUTWeapon::StaticClass()));
	SelectedFavWeapon->SetText((WeaponClass == NULL) ? GNone : WeaponClass->GetDefaultObject<AUTWeapon>()->DisplayName);
}
void SUWBotConfigDialog::OnCustomizedBotChange(TSharedPtr<FString> BotName, ESelectInfo::Type SelectInfo)
{
	// save previously edited bot, unless this is startup
	SaveCustomizedBot();

	CustomizedBotName->SetText(*BotName.Get());

	CurrentlyEditedIndex = BotNames.Find(BotName);
	TArray<FBotCharacter>& AllBots = UUTBotConfig::StaticClass()->GetDefaultObject<UUTBotConfig>()->BotCharacters;
	if (!AllBots.IsValidIndex(CurrentlyEditedIndex)) // this is used for adding new items or when the .ini is invalid/cleared
	{
		AllBots.SetNumZeroed(CurrentlyEditedIndex + 1);
		AllBots[CurrentlyEditedIndex].PlayerName = *BotName.Get();
	}

	FBotCharacter& LoadBot = AllBots[CurrentlyEditedIndex];
	SkillModifier->SetValue(ConvertToSliderValue(LoadBot.SkillModifier));
	Aggressiveness->SetValue(ConvertToSliderValue(LoadBot.Aggressiveness));
	Tactics->SetValue(ConvertToSliderValue(LoadBot.Tactics));
	Jumpiness->SetValue(ConvertToSliderValue(LoadBot.Jumpiness));
	MovementAbility->SetValue(ConvertToSliderValue(LoadBot.MovementAbility));
	ReactionTime->SetValue(ConvertToSliderValue(LoadBot.ReactionTime));
	Accuracy->SetValue(ConvertToSliderValue(LoadBot.Accuracy));
	Alertness->SetValue(ConvertToSliderValue(LoadBot.Alertness));
	MapAwareness->SetValue(ConvertToSliderValue(LoadBot.MapAwareness));
	UClass* FavWeapon = NULL;
	for (UClass* TestWeapon : WeaponList)
	{
		// check inheritance chain in case name in .ini is a base class
		for (UClass* TestClass = TestWeapon; TestClass != NULL; TestClass = TestClass->GetSuperClass())
		{
			if (TestClass->GetFName() == LoadBot.FavoriteWeapon)
			{
				FavWeapon = TestWeapon;
				break;
			}
		}
		if (FavWeapon != NULL)
		{
			break;
		}
	}
	FavoriteWeapon->SetSelectedItem(FavWeapon);
}

void SUWBotConfigDialog::NewBotNameResult(TSharedPtr<SCompoundWidget> Dialog, uint16 ButtonPressed)
{
	if (ButtonPressed & UTDIALOG_BUTTON_OK)
	{
		TSharedPtr<SUWInputBox> Box = StaticCastSharedPtr<SUWInputBox>(Dialog);
		if (Box.IsValid())
		{
			FString InputText = Box->GetInputText();
			if (InputText.Len() > 0)
			{
				bool bFound = false;
				for (TSharedPtr<FString> TestName : BotNames)
				{
					if (*TestName.Get() == InputText)
					{
						bFound = true;
						break;
					}
				}
				if (bFound)
				{
					GetPlayerOwner()->OpenDialog( SNew(SUWDialog)
												.PlayerOwner(GetPlayerOwner())
												.DialogTitle(NSLOCTEXT("SUWBotConfigDialog", "NameInUse", "That bot name is already in use."))
												);
				}
				else
				{
					TSharedPtr<FString> NewName = MakeShareable(new FString(InputText));
					BotNames.Add(NewName);
					BotIncludeList->RequestListRefresh();
					BotToConfigure->SetSelectedItem(NewName);
				}
			}
		}
	}
}

FReply SUWBotConfigDialog::NewBotClick()
{
	GetPlayerOwner()->OpenDialog( SNew(SUWInputBox)
								.PlayerOwner(GetPlayerOwner())
								.MessageText(NSLOCTEXT("SUWBotConfigDialog", "NewBotName", "Enter a name for the new bot profile:"))
								.OnDialogResult(this, &SUWBotConfigDialog::NewBotNameResult)
								 );
	return FReply::Handled();
}

FReply SUWBotConfigDialog::DeleteBotClick()
{
	if (BotNames.Num() <= 1)
	{
		GetPlayerOwner()->OpenDialog( SNew(SUWDialog)
									.PlayerOwner(GetPlayerOwner())
									.DialogTitle(NSLOCTEXT("SUWBotConfigDialog", "NeedABot", "Can't delete the last defined bot!"))
									);
	}
	else
	{
		BotNames.RemoveAt(CurrentlyEditedIndex);
		UUTBotConfig::StaticClass()->GetDefaultObject<UUTBotConfig>()->BotCharacters.RemoveAt(CurrentlyEditedIndex);
		UUTBotConfig::StaticClass()->GetDefaultObject<UUTBotConfig>()->Save();
		int32 NewEditIndex = BotNames.IsValidIndex(CurrentlyEditedIndex) ? CurrentlyEditedIndex : 0;
		CurrentlyEditedIndex = INDEX_NONE;
		BotIncludeList->RequestListRefresh();
		BotToConfigure->SetSelectedItem(BotNames[NewEditIndex]);
	}

	return FReply::Handled();
}

FReply SUWBotConfigDialog::OKClick()
{
	SaveCustomizedBot();
	if (GameClass != NULL)
	{
		AUTGameMode* DefGame = GameClass.GetDefaultObject();
		DefGame->SelectedBots.Empty();
		TArray< TSharedPtr<FString> > DesiredBots = BotIncludeList->GetSelectedItems();
		for (TSharedPtr<FString> Bot : DesiredBots)
		{
			if (Bot.IsValid())
			{
				new(DefGame->SelectedBots) FSelectedBot(FString(*Bot.Get()), 255);
			}
		}
	}
	GetPlayerOwner()->CloseDialog(SharedThis(this));
	return FReply::Handled();
}