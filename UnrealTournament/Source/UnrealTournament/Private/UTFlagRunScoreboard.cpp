// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.
#include "UnrealTournament.h"
#include "UTFlagRunScoreboard.h"
#include "UTCTFRoundGameState.h"
#include "UTCTFScoring.h"
#include "UTFlagRunMessage.h"
#include "StatNames.h"

UUTFlagRunScoreboard::UUTFlagRunScoreboard(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
	CH_Powerup = NSLOCTEXT("UTFlagRunScoreboard", "ColumnHeader_Powerups", "");

	ColumnHeaderPowerupX = 0.715f;
	ColumnHeaderPowerupEndX = ColumnHeaderPowerupX + 0.0575f;
	ColumnHeaderPowerupXDuringReadyUp = 0.82f;
	bGroupRoundPairs = true;
	bUseRoundKills = true;
}

void UUTFlagRunScoreboard::DrawScoreHeaders(float RenderDelta, float& YOffset)
{
	float XOffset = ScaledEdgeSize;
	float Height = 23.f*RenderScale;

	for (int32 i = 0; i < 2; i++)
	{
		// Draw the background Border
		DrawTexture(UTHUDOwner->ScoreboardAtlas, XOffset, YOffset, ScaledCellWidth, Height, 149, 138, 32, 32, 1.0, FLinearColor(0.72f, 0.72f, 0.72f, 0.85f));
		DrawText(CH_PlayerName, XOffset + (ScaledCellWidth * ColumnHeaderPlayerX), YOffset + ColumnHeaderY, UTHUDOwner->TinyFont, RenderScale, 1.0f, FLinearColor::Black, ETextHorzPos::Left, ETextVertPos::Center);
		if (UTGameState && UTGameState->HasMatchStarted())
		{
			DrawText(CH_Kills, XOffset + (ScaledCellWidth * ColumnHeaderScoreX), YOffset + ColumnHeaderY, UTHUDOwner->TinyFont, RenderScale, 1.0f, FLinearColor::Black, ETextHorzPos::Center, ETextVertPos::Center);
			DrawText(CH_Powerup, XOffset + (ScaledCellWidth * 0.5f * (ColumnHeaderPowerupX + ColumnHeaderPowerupEndX)), YOffset + ColumnHeaderY, UTHUDOwner->TinyFont, RenderScale, 1.0f, FLinearColor::Black, ETextHorzPos::Center, ETextVertPos::Center);
			AUTCTFGameState* CTFState = Cast<AUTCTFGameState>(UTGameState);
			if (CTFState && ((CTFState->bRedToCap == (i==0)) ? CTFState->bAttackerLivesLimited : CTFState->bDefenderLivesLimited))
			{
				DrawText(NSLOCTEXT("UTScoreboard", "LivesRemaining", "Lives"), XOffset + (ScaledCellWidth * 0.5f*(ColumnHeaderPowerupX + ColumnHeaderPingX)), YOffset + ColumnHeaderY, UTHUDOwner->TinyFont, RenderScale, 1.0f, FLinearColor::Black, ETextHorzPos::Center, ETextVertPos::Center);
			}
		}
		else
		{
			DrawText(CH_Powerup, XOffset + (ScaledCellWidth * ColumnHeaderPowerupXDuringReadyUp), YOffset + ColumnHeaderY, UTHUDOwner->TinyFont, RenderScale, 1.0f, FLinearColor::Black, ETextHorzPos::Center, ETextVertPos::Center);
		}
		DrawText((GetWorld()->GetNetMode() == NM_Standalone) ?  CH_Skill : CH_Ping, XOffset + (ScaledCellWidth * ColumnHeaderPingX), YOffset + ColumnHeaderY, UTHUDOwner->TinyFont, RenderScale, 1.0f, FLinearColor::Black, ETextHorzPos::Center, ETextVertPos::Center);
		XOffset = Canvas->ClipX - ScaledCellWidth - ScaledEdgeSize;
	}

	YOffset += Height + 4;
}

void UUTFlagRunScoreboard::DrawPlayerScore(AUTPlayerState* PlayerState, float XOffset, float YOffset, float Width, FLinearColor DrawColor)
{
	DrawText(FText::AsNumber(int32(PlayerState->RoundKills)), XOffset + (Width * ColumnHeaderScoreX), YOffset + ColumnY, UTHUDOwner->SmallFont, RenderScale, 1.0f, DrawColor, ETextHorzPos::Center, ETextVertPos::Center);
	AUTCTFGameState* CTFState = Cast<AUTCTFGameState>(UTGameState);
	if (CTFState)
	{
		AUTInventory* SelectedPowerup = PlayerState && PlayerState->BoostClass ? PlayerState->BoostClass->GetDefaultObject<AUTInventory>() : nullptr;
		const float LivesXOffset = (Width * 0.5f*(ColumnHeaderPowerupX + ColumnHeaderPingX));

		//Only display powerups from your team
		if (SelectedPowerup && (ShouldShowPowerupForPlayer(PlayerState)))
		{
			const float U = SelectedPowerup->HUDIcon.U;
			const float V = SelectedPowerup->HUDIcon.V;
			const float UL = SelectedPowerup->HUDIcon.UL;
			const float VL = SelectedPowerup->HUDIcon.VL;
				
			const float AspectRatio = UL / VL;
			const float TextureSize = (Width * (ColumnHeaderPowerupEndX - ColumnHeaderPowerupX));

			//Draw a Red Dash if they have used their powerup, or an icon if they have not yet used it
			AUTCTFRoundGameState* RCTFGameState = Cast<AUTCTFRoundGameState>(CTFState);
			if (RCTFGameState)
			{
				const bool bIsAbleToEarnMorePowerups = (RCTFGameState->IsTeamOnOffense(PlayerState->GetTeamNum()) ? RCTFGameState->bIsOffenseAbleToGainPowerup : RCTFGameState->bIsDefenseAbleToGainPowerup);				
				
				//Draw Dash
				if (!bIsAbleToEarnMorePowerups && !RCTFGameState->IsMatchIntermission() && (PlayerState->GetRemainingBoosts() <= 0))
				{
					DrawText(NSLOCTEXT("UTScoreboard", "Dash", "-"), XOffset + (Width * ((ColumnHeaderPowerupEndX + ColumnHeaderPowerupX) / 2)), YOffset + ColumnY, UTHUDOwner->TinyFont, RenderScale, 1.0f, DrawColor, ETextHorzPos::Center, ETextVertPos::Center);
				}
				//Draw powerup symbol
				else
				{
					UTexture* PowerupTexture = SelectedPowerup->HUDIcon.Texture;
					if (PowerupTexture)
					{
						DrawTexture(PowerupTexture, XOffset + (Width * ColumnHeaderPowerupX), YOffset, TextureSize, TextureSize * AspectRatio, U, V, UL, VL);
					}
				}
			}
		}
		else
		{
			DrawText(NSLOCTEXT("UTScoreboard", "Dash", "-"), XOffset + (Width * ColumnHeaderPowerupX), YOffset + ColumnY, UTHUDOwner->TinyFont, RenderScale, 1.0f, DrawColor, ETextHorzPos::Center, ETextVertPos::Center);
		}

		if ((CTFState->bAttackerLivesLimited || CTFState->bDefenderLivesLimited) && PlayerState->bHasLifeLimit && (PlayerState->RemainingLives > 0))
		{
			DrawText(FText::AsNumber(PlayerState->RemainingLives), XOffset + LivesXOffset, YOffset + ColumnY, UTHUDOwner->TinyFont, RenderScale, 1.0f, DrawColor, ETextHorzPos::Center, ETextVertPos::Center);
		}
	}
	else
	{
		DrawText(FText::AsNumber(PlayerState->FlagCaptures), XOffset + (Width * ColumnHeaderCapsX), YOffset + ColumnY, UTHUDOwner->TinyFont, RenderScale, 1.0f, DrawColor, ETextHorzPos::Center, ETextVertPos::Center);
		DrawText(FText::AsNumber(PlayerState->Assists), XOffset + (Width * ColumnHeaderAssistsX), YOffset + ColumnY, UTHUDOwner->TinyFont, RenderScale, 1.0f, DrawColor, ETextHorzPos::Center, ETextVertPos::Center);
		DrawText(FText::AsNumber(PlayerState->FlagReturns), XOffset + (Width * ColumnHeaderReturnsX), YOffset + ColumnY, UTHUDOwner->TinyFont, RenderScale, 1.0f, DrawColor, ETextHorzPos::Center, ETextVertPos::Center);
	}
}

bool UUTFlagRunScoreboard::ShouldShowPowerupForPlayer(AUTPlayerState* PlayerState)
{
	if (UTGameState->IsMatchInProgress() && !UTGameState->IsMatchIntermission())
	{
		return true;
	}

	else if (UTPlayerOwner->GetTeamNum() == PlayerState->GetTeamNum())
	{
		return true;
	}

	return false;
}

void UUTFlagRunScoreboard::DrawReadyText(AUTPlayerState* PlayerState, float XOffset, float YOffset, float Width)
{
	Super::DrawReadyText(PlayerState, XOffset, YOffset, Width);

	AUTInventory* SelectedPowerup = PlayerState && PlayerState->BoostClass ? PlayerState->BoostClass->GetDefaultObject<AUTInventory>() : nullptr;
	AUTCTFGameState* CTFState = Cast<AUTCTFGameState>(UTGameState);
	if (CTFState && SelectedPowerup && SelectedPowerup->HUDIcon.Texture && ShouldShowPowerupForPlayer(PlayerState))
	{
		const float U = SelectedPowerup->HUDIcon.U;
		const float V = SelectedPowerup->HUDIcon.V;
		const float UL = SelectedPowerup->HUDIcon.UL;
		const float VL = SelectedPowerup->HUDIcon.VL;

		const float AspectRatio = UL / VL;
		const float TextureSize = Width * (ColumnHeaderPowerupEndX - ColumnHeaderPowerupX);

		DrawTexture(SelectedPowerup->HUDIcon.Texture, XOffset + (ScaledCellWidth * ColumnHeaderPowerupXDuringReadyUp) - (TextureSize / 2), YOffset, TextureSize, TextureSize * AspectRatio, U, V, UL, VL);
	}
	else
	{
		DrawText(NSLOCTEXT("UTScoreboard", "Dash", "-"), XOffset + (ScaledCellWidth * ColumnHeaderPowerupXDuringReadyUp), YOffset + ColumnHeaderY, UTHUDOwner->TinyFont, RenderScale, 1.0f, FLinearColor::Black, ETextHorzPos::Center, ETextVertPos::Center);
	}
}

void UUTFlagRunScoreboard::DrawTeamStats(float DeltaTime, float& YPos, float XOffset, float ScoreWidth, float MaxHeight, const FStatsFontInfo& StatsFontInfo)
{
	DrawStatsLine(NSLOCTEXT("UTScoreboard", "TeamKills", "Kills"), UTGameState->Teams[0]->GetStatsValue(NAME_TeamKills), UTGameState->Teams[1]->GetStatsValue(NAME_TeamKills), DeltaTime, XOffset, YPos, StatsFontInfo, ScoreWidth);

	float SectionSpacing = 0.6f * StatsFontInfo.TextHeight;
	YPos += SectionSpacing;

	// find top kills
	AUTPlayerState* TopKillerRed = FindTopTeamKillerFor(0);
	AUTPlayerState* TopKillerBlue = FindTopTeamKillerFor(1);

	DrawPlayerStatsLine(NSLOCTEXT("UTScoreboard", "TopFlagRunner", "Top Flag Runner"), UTGameState->Teams[0]->TopAttacker, UTGameState->Teams[1]->TopAttacker, DeltaTime, XOffset, YPos, StatsFontInfo, ScoreWidth, 0);
	DrawPlayerStatsLine(NSLOCTEXT("UTScoreboard", "TopKills", "Top Kills"), TopKillerRed, TopKillerBlue, DeltaTime, XOffset, YPos, StatsFontInfo, ScoreWidth, 0);
	YPos += SectionSpacing;

	DrawStatsLine(NSLOCTEXT("UTScoreboard", "BeltPickups", "Shield Belt Pickups"), UTGameState->Teams[0]->GetStatsValue(NAME_ShieldBeltCount), UTGameState->Teams[1]->GetStatsValue(NAME_ShieldBeltCount), DeltaTime, XOffset, YPos, StatsFontInfo, ScoreWidth);
	DrawStatsLine(NSLOCTEXT("UTScoreboard", "LargeArmorPickups", "Large Armor Pickups"), UTGameState->Teams[0]->GetStatsValue(NAME_ArmorVestCount), UTGameState->Teams[1]->GetStatsValue(NAME_ArmorVestCount), DeltaTime, XOffset, YPos, StatsFontInfo, ScoreWidth);
	DrawStatsLine(NSLOCTEXT("UTScoreboard", "MediumArmorPickups", "Medium Armor Pickups"), UTGameState->Teams[0]->GetStatsValue(NAME_ArmorPadsCount), UTGameState->Teams[1]->GetStatsValue(NAME_ArmorPadsCount), DeltaTime, XOffset, YPos, StatsFontInfo, ScoreWidth);

	int32 OptionalLines = 0;
	int32 TeamStat0 = UTGameState->Teams[0]->GetStatsValue(NAME_UDamageCount);
	int32 TeamStat1 = UTGameState->Teams[1]->GetStatsValue(NAME_UDamageCount);
	if (TeamStat0 > 0 || TeamStat1 > 0)
	{
		DrawStatsLine(NSLOCTEXT("UTScoreboard", "UDamagePickups", "UDamage Pickups"), TeamStat0, TeamStat1, DeltaTime, XOffset, YPos, StatsFontInfo, ScoreWidth);
		DrawClockTeamStatsLine(NSLOCTEXT("UTScoreboard", "UDamage", "UDamage Control"), NAME_UDamageTime, DeltaTime, XOffset, YPos, StatsFontInfo, ScoreWidth, true);
		OptionalLines += 2;
	}
	TeamStat0 = UTGameState->Teams[0]->GetStatsValue(NAME_BerserkCount);
	TeamStat1 = UTGameState->Teams[1]->GetStatsValue(NAME_BerserkCount);
	if (TeamStat0 > 0 || TeamStat1 > 0)
	{
		DrawStatsLine(NSLOCTEXT("UTScoreboard", "BerserkPickups", "Berserk Pickups"), TeamStat0, TeamStat1, DeltaTime, XOffset, YPos, StatsFontInfo, ScoreWidth);
		DrawClockTeamStatsLine(NSLOCTEXT("UTScoreboard", "Berserk", "Berserk Control"), NAME_BerserkTime, DeltaTime, XOffset, YPos, StatsFontInfo, ScoreWidth, true);
		OptionalLines += 2;
	}
	TeamStat0 = UTGameState->Teams[0]->GetStatsValue(NAME_InvisibilityCount);
	TeamStat1 = UTGameState->Teams[1]->GetStatsValue(NAME_InvisibilityCount);
	if (TeamStat0 > 0 || TeamStat1 > 0)
	{
		DrawStatsLine(NSLOCTEXT("UTScoreboard", "InvisibilityPickups", "Invisibility Pickups"), TeamStat0, TeamStat1, DeltaTime, XOffset, YPos, StatsFontInfo, ScoreWidth);
		DrawClockTeamStatsLine(NSLOCTEXT("UTScoreboard", "Invisibility", "Invisibility Control"), NAME_InvisibilityTime, DeltaTime, XOffset, YPos, StatsFontInfo, ScoreWidth, true);
		OptionalLines += 2;
	}

	TeamStat0 = UTGameState->Teams[0]->GetStatsValue(NAME_KegCount);
	TeamStat1 = UTGameState->Teams[1]->GetStatsValue(NAME_KegCount);
	if (TeamStat0 > 0 || TeamStat1 > 0)
	{
		DrawStatsLine(NSLOCTEXT("UTScoreboard", "KegPickups", "Keg Pickups"), TeamStat0, TeamStat1, DeltaTime, XOffset, YPos, StatsFontInfo, ScoreWidth);
		OptionalLines += 1;
	}

	if (OptionalLines < 7)
	{
		TeamStat0 = UTGameState->Teams[0]->GetStatsValue(NAME_HelmetCount);
		TeamStat1 = UTGameState->Teams[1]->GetStatsValue(NAME_HelmetCount);
		if (TeamStat0 > 0 || TeamStat1 > 0)
		{
			DrawStatsLine(NSLOCTEXT("UTScoreboard", "SmallArmorPickups", "Small Armor Pickups"), TeamStat0, TeamStat1, DeltaTime, XOffset, YPos, StatsFontInfo, ScoreWidth);
			OptionalLines += 1;
		}
	}

	if (OptionalLines < 7)
	{
		int32 BootJumpsRed = UTGameState->Teams[0]->GetStatsValue(NAME_BootJumps);
		int32 BootJumpsBlue = UTGameState->Teams[1]->GetStatsValue(NAME_BootJumps);
		if ((BootJumpsRed != 0) || (BootJumpsBlue != 0))
		{
			DrawStatsLine(NSLOCTEXT("UTScoreboard", "JumpBootJumps", "JumpBoot Jumps"), BootJumpsRed, BootJumpsBlue, DeltaTime, XOffset, YPos, StatsFontInfo, ScoreWidth);
			OptionalLines += 1;
		}
	}
	// later do redeemer shots -and all these also to individual
	// track individual movement stats as well
	// @TODO FIXMESTEVE make all the loc text into properties instead of recalc
}

bool UUTFlagRunScoreboard::ShouldDrawScoringStats()
{
	AUTCTFGameState* CTFState = Cast<AUTCTFGameState>(UTGameState);
	return Super::ShouldDrawScoringStats() && CTFState && (CTFState->GetScoringPlays().Num() > 0);
}

void UUTFlagRunScoreboard::DrawStatsRight(float DeltaTime, float& YPos, float XOffset, float ScoreWidth, float PageBottom)
{
}

void UUTFlagRunScoreboard::DrawScoringPlays(float DeltaTime, float& YPos, float XOffset, float ScoreWidth, float MaxHeight)
{
	Super::DrawScoringPlays(DeltaTime, YPos, XOffset, ScoreWidth, MaxHeight);

	if (TimeLineOffset < 1.f)
	{
		return;
	}
	AUTCTFRoundGameState* GS = GetWorld()->GetGameState<AUTCTFRoundGameState>();
	if (!GS || GS->HasMatchEnded() || (GS->Teams.Num() < 2) || !GS->Teams[0] || !GS->Teams[1])
	{
		return;
	}

	Canvas->SetLinearDrawColor(FLinearColor::White);
	FFontRenderInfo TextRenderInfo;
	TextRenderInfo.bEnableShadow = true;
	TextRenderInfo.bClipText = true;
	bool bDrawBlueFirst = false;
	FText ScorePreamble = NSLOCTEXT("UTFlagRun", "ScoreTied", "Score Tied ");
	if (GS->Teams[0]->Score > GS->Teams[1]->Score)
	{
		ScorePreamble = NSLOCTEXT("UTFlagRun", "RedLeads", "Red Team leads  ");
	}
	else if (GS->Teams[1]->Score > GS->Teams[0]->Score)
	{
		ScorePreamble = NSLOCTEXT("UTFlagRun", "BlueLeads", "Blue Team leads  ");
		bDrawBlueFirst = true;
	}
	FFormatNamedArguments Args;
	Args.Add("Preamble", ScorePreamble);
	Args.Add("RedScore", FText::AsNumber(GS->Teams[0]->Score));
	Args.Add("BlueScore", FText::AsNumber(GS->Teams[1]->Score));

	FText CurrentScoreText = FText::Format(NSLOCTEXT("UTFlagRun", "ScoreText", "{Preamble} {RedScore} - {BlueScore}"), Args);
	float ScoringOffsetX, ScoringOffsetY;
	Canvas->TextSize(UTHUDOwner->MediumFont, CurrentScoreText.ToString(), ScoringOffsetX, ScoringOffsetY, RenderScale, RenderScale);

	FLinearColor DrawColor = FLinearColor::White;
	float CurrentScoreHeight = (GS->CTFRound >= GS->NumRounds - 2) ? 3.f*ScoringOffsetY : 2.f*ScoringOffsetY;
	float BackAlpha = 0.3f;
	DrawTexture(UTHUDOwner->ScoreboardAtlas, XOffset - 0.0175f*ScoreWidth, YPos, 1.06f*ScoreWidth, CurrentScoreHeight, 149, 138, 32, 32, BackAlpha, DrawColor);

	float SingleXL, SingleYL;
	float ScoreX = XOffset + 0.99f*ScoreWidth - ScoringOffsetX;
	FString PreambleString = ScorePreamble.ToString();
	Canvas->TextSize(UTHUDOwner->MediumFont, PreambleString, SingleXL, SingleYL, RenderScale, RenderScale);
	YPos -= 0.1f * SingleYL;
	Canvas->DrawText(UTHUDOwner->MediumFont, PreambleString, ScoreX, YPos, RenderScale, RenderScale, TextRenderInfo);
	Canvas->SetLinearDrawColor(FLinearColor::White);
	ScoreX += SingleXL;

	int32 TeamIndex = bDrawBlueFirst ? 1 : 0;
	Canvas->SetLinearDrawColor(GS->Teams[TeamIndex]->TeamColor);
	FString SingleScorePart = FString::Printf(TEXT("%i"), GS->Teams[TeamIndex]->Score);
	Canvas->TextSize(UTHUDOwner->MediumFont, SingleScorePart, SingleXL, SingleYL, RenderScale, RenderScale);
	Canvas->DrawText(UTHUDOwner->MediumFont, SingleScorePart, ScoreX, YPos, RenderScale, RenderScale, TextRenderInfo);
	Canvas->SetLinearDrawColor(FLinearColor::White);
	ScoreX += SingleXL;
	Canvas->TextSize(UTHUDOwner->MediumFont, " - ", SingleXL, SingleYL, RenderScale, RenderScale);
	Canvas->DrawText(UTHUDOwner->MediumFont, " - ", ScoreX, YPos, RenderScale, RenderScale, TextRenderInfo);
	ScoreX += SingleXL;
	TeamIndex = bDrawBlueFirst ? 0 : 1;
	Canvas->SetLinearDrawColor(GS->Teams[TeamIndex]->TeamColor);
	SingleScorePart = FString::Printf(TEXT("%i"), GS->Teams[TeamIndex]->Score);
	Canvas->DrawText(UTHUDOwner->MediumFont, SingleScorePart, ScoreX, YPos, RenderScale, RenderScale, TextRenderInfo);
	YPos += (GS->CTFRound == GS->NumRounds - 2) ? 0.9f*SingleYL : 0.8f*SingleYL;

	bool bRedLeadsTiebreak = (GS->TiebreakValue > 0);
	FText TiebreakPreamble = NSLOCTEXT("UTFlagRun", "TiebreakPreamble", "Tiebreak ");
	Args.Add("TBPre", TiebreakPreamble);
	Args.Add("Bonus", FText::AsNumber(FMath::Abs(GS->TiebreakValue)));
	Args.Add("Team", bRedLeadsTiebreak ? RedTeamText : BlueTeamText);
	FText TiebreakBonusPattern = NSLOCTEXT("UTFlagRun", "TiebreakPattern", "{TBPre}{Team} +{Bonus}");
	FText TiebreakBonusText = FText::Format(TiebreakBonusPattern, Args);
	Canvas->TextSize(UTHUDOwner->SmallFont, TiebreakBonusText.ToString(), ScoringOffsetX, ScoringOffsetY, RenderScale, RenderScale);
	ScoreX = XOffset + ScoreWidth - ScoringOffsetX;

	Canvas->SetLinearDrawColor(FLinearColor::White);
	PreambleString = TiebreakPreamble.ToString();
	Canvas->TextSize(UTHUDOwner->SmallFont, PreambleString, SingleXL, SingleYL, RenderScale, RenderScale);
	Canvas->DrawText(UTHUDOwner->SmallFont, PreambleString, ScoreX, YPos, RenderScale, RenderScale, TextRenderInfo);
	ScoreX += SingleXL;

	Canvas->SetLinearDrawColor(bRedLeadsTiebreak ? GS->Teams[0]->TeamColor : GS->Teams[1]->TeamColor);
	FString TeamString = bRedLeadsTiebreak ? RedTeamText.ToString() : BlueTeamText.ToString();
	Canvas->TextSize(UTHUDOwner->SmallFont, TeamString, SingleXL, SingleYL, RenderScale, RenderScale);
	Canvas->DrawText(UTHUDOwner->SmallFont, TeamString, ScoreX, YPos, RenderScale, RenderScale, TextRenderInfo);
	ScoreX += SingleXL;

	Canvas->SetLinearDrawColor(FLinearColor::White);
	FText TiebreakValue = FText::Format(NSLOCTEXT("UTFlagRun", "TiebreakValue", " +{Bonus}"), Args);
	Canvas->DrawText(UTHUDOwner->SmallFont, TiebreakValue, ScoreX, YPos, RenderScale, RenderScale, TextRenderInfo);

	YPos += (GS->CTFRound == GS->NumRounds - 2) ? 0.9f*SingleYL : 0.8f*SingleYL;
	if (GS->FlagRunMessageTeam != nullptr)
	{
		ScoreX = XOffset;
		FText PrefixText, EmphasisText, PostfixText;
		FText SecondPostfixText = FText::GetEmpty();
		FLinearColor EmphasisColor;
		UUTFlagRunMessage::StaticClass()->GetDefaultObject<UUTFlagRunMessage>()->GetEmphasisText(PrefixText, EmphasisText, PostfixText, EmphasisColor, GS->FlagRunMessageSwitch, nullptr, nullptr, GS->FlagRunMessageTeam);

		float XL, YL, PreXL, EmphasisXL;
		Canvas->StrLen(UTHUDOwner->SmallFont, PrefixText.ToString(), PreXL, YL);
		Canvas->StrLen(UTHUDOwner->SmallFont, EmphasisText.ToString(), EmphasisXL, YL);

		ScoreMessageText = UUTFlagRunMessage::StaticClass()->GetDefaultObject<UUTFlagRunMessage>()->GetText(GS->FlagRunMessageSwitch, false, nullptr, nullptr, GS->FlagRunMessageTeam);
		Canvas->StrLen(UTHUDOwner->SmallFont, ScoreMessageText.ToString(), XL, YL);
		if (XL > ScoreWidth)
		{
			// get split postfix text, set offset considering first part
			UUTFlagRunMessage::StaticClass()->GetDefaultObject<UUTFlagRunMessage>()->SplitPostfixText(PostfixText, SecondPostfixText, GS->FlagRunMessageSwitch, GS->FlagRunMessageTeam);
			float PostXL;
			Canvas->StrLen(UTHUDOwner->SmallFont, PostfixText.ToString(), PostXL, YL);
			ScoreX = XOffset + ScoreWidth - RenderScale * (PreXL + EmphasisXL + PostXL);
		}
		else
		{
			ScoreX = XOffset + ScoreWidth - RenderScale * XL;
		}
		Canvas->SetLinearDrawColor(FLinearColor::White);
		Canvas->DrawText(UTHUDOwner->SmallFont, PrefixText, ScoreX, YPos, RenderScale, RenderScale, TextRenderInfo);
		ScoreX += PreXL*RenderScale;

		Canvas->SetLinearDrawColor(GS->FlagRunMessageTeam->TeamIndex == 0 ? FLinearColor::Red : FLinearColor::Blue);
		Canvas->DrawText(UTHUDOwner->SmallFont, EmphasisText, ScoreX, YPos - 0.1f*YL*RenderScale, 1.1f*RenderScale, 1.1f*RenderScale, TextRenderInfo);
		ScoreX += 1.1f*EmphasisXL*RenderScale;

		Canvas->SetLinearDrawColor(FLinearColor::White);
		Canvas->DrawText(UTHUDOwner->SmallFont, PostfixText, ScoreX, YPos, RenderScale, RenderScale, TextRenderInfo);
		
		if (!SecondPostfixText.IsEmpty())
		{
			Canvas->StrLen(UTHUDOwner->SmallFont, SecondPostfixText.ToString(), XL, YL);
			ScoreX = XOffset + ScoreWidth - RenderScale * XL;
			YPos += 0.9f*SingleYL;
			Canvas->DrawText(UTHUDOwner->SmallFont, SecondPostfixText, ScoreX, YPos, RenderScale, RenderScale, TextRenderInfo);
		}
	}
}

int32 UUTFlagRunScoreboard::GetSmallPlaysCount(int32 NumPlays) const
{
	return  (NumPlays > 6) ? NumPlays : 0;
}

void UUTFlagRunScoreboard::DrawScoringPlayInfo(const FCTFScoringPlay& Play, float CurrentScoreHeight, float SmallYL, float MedYL, float DeltaTime, float& YPos, float XOffset, float ScoreWidth, FFontRenderInfo TextRenderInfo, bool bIsSmallPlay)
{
	AUTCTFGameState* CTFState = Cast<AUTCTFGameState>(UTGameState);
	float BoxYPos = YPos;

	// draw scoring team icon
	int32 IconIndex = Play.Team->TeamIndex;
	IconIndex = FMath::Min(IconIndex, 1);
	float IconHeight = 0.9f*CurrentScoreHeight;
	float IconOffset = 0.13f*ScoreWidth;
	DrawTexture(UTHUDOwner->HUDAtlas, XOffset + IconOffset, YPos + 0.1f*CurrentScoreHeight, IconHeight, IconHeight, UTHUDOwner->TeamIconUV[IconIndex].X, UTHUDOwner->TeamIconUV[IconIndex].Y, 72, 72, 1.f, Play.Team->TeamColor);

	FString ScoredByLine;
	if (Play.bDefenseWon)
	{
		ScoredByLine = FString::Printf(TEXT("Defense Won"));
	}
	else if (Play.bAnnihilation)
	{
		ScoredByLine = FString::Printf(TEXT("Annihilation"));
	}
	else
	{
		ScoredByLine = Play.ScoredBy.GetPlayerName();
		if (Play.ScoredByCaps > 1)
		{
			ScoredByLine += FString::Printf(TEXT(" (%i)"), Play.ScoredByCaps);
		}
	}

	// time of game
	FString TimeStampLine = UTHUDOwner->ConvertTime(FText::GetEmpty(), FText::GetEmpty(), Play.RemainingTime, false, true, false).ToString();
	Canvas->SetLinearDrawColor(FLinearColor::White);
	Canvas->DrawText(UTHUDOwner->SmallFont, TimeStampLine, XOffset + 0.01f*ScoreWidth, YPos + 0.5f*CurrentScoreHeight - 0.5f*SmallYL, RenderScale, RenderScale, TextRenderInfo);

	// scored by
	Canvas->SetLinearDrawColor(Play.Team->TeamColor);
	float NameSizeX, NameSizeY;
	Canvas->TextSize(UTHUDOwner->MediumFont, ScoredByLine, NameSizeX, NameSizeY, RenderScale, RenderScale);
	Canvas->DrawText(UTHUDOwner->MediumFont, ScoredByLine, XOffset + 0.32f*ScoreWidth, YPos + 0.5f*CurrentScoreHeight - 0.6f*MedYL, RenderScale * FMath::Min(1.f, ScoreWidth*0.35f / FMath::Max(NameSizeX, 1.f)), RenderScale, TextRenderInfo);

	int32 RoundBonus = FMath::Max(Play.RedBonus, Play.BlueBonus);
	FString BonusString = TEXT("\u2605");
	FLinearColor BonusColor = FLinearColor(0.48f, 0.25f, 0.18f);
	if ((RoundBonus > 0) && !Play.bDefenseWon)
	{
		if (RoundBonus >= 60)
		{
			BonusString = (RoundBonus >= 120) ? TEXT("\u2605 \u2605 \u2605") : TEXT("\u2605 \u2605");
			BonusColor = (RoundBonus >= 120) ? FLinearColor(1.f, 0.9f, 0.15f) : FLinearColor::White;
		}
	}
	float ScoringOffsetX, ScoringOffsetY;
	Canvas->TextSize(UTHUDOwner->MediumFont, BonusString, ScoringOffsetX, ScoringOffsetY, RenderScale, RenderScale);
	float ScoreX = XOffset + 0.99f*ScoreWidth - ScoringOffsetX;

	Canvas->SetLinearDrawColor(BonusColor);
	Canvas->DrawText(UTHUDOwner->MediumFont, BonusString, ScoreX, YPos + 0.5f*CurrentScoreHeight - 0.6f*MedYL, RenderScale, RenderScale, TextRenderInfo);

	if ((RoundBonus > 0) && !Play.bDefenseWon && !bIsSmallPlay)
	{
		YPos += 0.82f* MedYL;
		int32 NetBonus = RoundBonus;
		while (NetBonus > 60)
		{
			NetBonus -= 60;
		}
		FString BonusLine = FString::Printf(TEXT("Bonus Time: %d"), NetBonus);
		Canvas->TextSize(UTHUDOwner->MediumFont, BonusLine, ScoringOffsetX, ScoringOffsetY, RenderScale, RenderScale);
		ScoreX = XOffset + ScoreWidth - 0.5f*ScoringOffsetX;
		Canvas->SetLinearDrawColor(FLinearColor::White);
		Canvas->DrawText(UTHUDOwner->TinyFont, BonusLine, ScoreX, YPos, 0.75f*RenderScale, RenderScale, TextRenderInfo);
	}
}

