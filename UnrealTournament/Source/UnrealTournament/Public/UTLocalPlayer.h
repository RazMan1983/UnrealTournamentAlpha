#pragma once

#include "SlateBasics.h"
#include "Slate/SlateGameResources.h"
#include "Online.h"
#include "OnlineSubsystemTypes.h"
#include "../Private/Slate/Base/SUTToastBase.h"
#include "../Private/Slate/Base/SUTDialogBase.h"
#include "UTProfileSettings.h"
#include "UTProgressionStorage.h"
#include "OnlinePresenceInterface.h"
#include "Http.h"
#include "UTProfileItem.h"

#include "UTLocalPlayer.generated.h"

class SUTMenuBase;
class SUTWindowBase;
class SUTServerBrowserPanel;
class SUTFriendsPopupWindow;
class SUTQuickMatchWindow;
class SUTLoginDialog;
class SUTDownloadAllDialog;
class SUTMapVoteDialog;
class SUTAdminDialog;
class SUTReplayWindow;
class FFriendsAndChatMessage;
class AUTPlayerState;
class SUTJoinInstanceWindow;
class FServerData;
class AUTRconAdminInfo;
class SUTSpectatorWindow;
class SUTMatchSummaryPanel;
class SUTChatEditBox;
class SUTQuickChatWindow;
class UUTKillcamPlayback;

enum class EMatchmakingCompleteResult : uint8;

DECLARE_MULTICAST_DELEGATE_ThreeParams(FPlayerOnlineStatusChanged, class UUTLocalPlayer*, ELoginStatus::Type, const FUniqueNetId&);

DECLARE_DELEGATE_TwoParams(FUTProfilesLoaded, bool, const FText&);

DECLARE_MULTICAST_DELEGATE(FOnRankedPlaylistsChangedDelegate);

// This delegate will be triggered whenever a chat message is updated.

class FStoredChatMessage : public TSharedFromThis<FStoredChatMessage>
{
public:
	// Where did this chat come from
	UPROPERTY()
	FName Type;

	// The Name of the server
	UPROPERTY()
	FString Sender;

	// What was the message
	UPROPERTY()
	FString Message;

	// What color should we display this chat in
	UPROPERTY()
	FLinearColor Color;

	UPROPERTY()
	int32 Timestamp;

	// If this chat message was owned by the player
	UPROPERTY()
	bool bMyChat;

	UPROPERTY()
	uint8 TeamNum;

	FStoredChatMessage(FName inType, FString inSender, FString inMessage, FLinearColor inColor, int32 inTimestamp, bool inbMyChat, uint8 inTeamNum)
		: Type(inType), Sender(inSender), Message(inMessage), Color(inColor), Timestamp(inTimestamp), bMyChat(inbMyChat), TeamNum(inTeamNum)
	{}

	static TSharedRef<FStoredChatMessage> Make(FName inType, FString inSender, FString inMessage, FLinearColor inColor, int32 inTimestamp, bool inbMyChat, uint8 inTeamNum)
	{
		return MakeShareable( new FStoredChatMessage( inType, inSender, inMessage, inColor, inTimestamp, inbMyChat, inTeamNum ) );
	}
};

DECLARE_MULTICAST_DELEGATE_TwoParams(FChatArchiveChanged, class UUTLocalPlayer*, TSharedPtr<FStoredChatMessage>);

USTRUCT()
struct FUTFriend
{
	GENERATED_USTRUCT_BODY()
public:
	FUTFriend()
	{}

	FUTFriend(FString inUserId, FString inDisplayName, bool inActualFriend, bool inIsOnline, bool inIsPlayingThisGame)
		: UserId(inUserId), DisplayName(inDisplayName), bActualFriend(inActualFriend), bIsOnline(inIsOnline), bIsPlayingThisGame(inIsPlayingThisGame)
	{}


	UPROPERTY()
	FString UserId;

	UPROPERTY()
	FString DisplayName;

	UPROPERTY()
	bool bActualFriend;
	
	UPROPERTY()
	bool bIsOnline;
	
	UPROPERTY()
	bool bIsPlayingThisGame;
};

/** profile notification data from the backend */
USTRUCT()
struct FXPProgressNotifyPayload
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY()
	int64 PrevXP;
	UPROPERTY()
	int64 XP;
	UPROPERTY()
	int32 PrevLevel;
	UPROPERTY()
	int32 Level;
};
USTRUCT()
struct FLevelUpRewardNotifyPayload
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY()
	int32 Level;
	UPROPERTY()
	FString RewardID;
};

USTRUCT()
struct FMMREntry
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY()
	int32 MMR;

	UPROPERTY()
	int32 MatchesPlayed;
};

USTRUCT()
struct FRankedLeagueProgress
{
	GENERATED_USTRUCT_BODY()

	FRankedLeagueProgress() :
		LeaguePlacementMatches(0), 
		LeaguePoints(0), 
		LeagueTier(0), 
		LeagueDivision(0), 
		LeaguePromotionMatchesAttempted(0), 
		LeaguePromotionMatchesWon(0), 
		bLeaguePromotionSeries(false)
	{
	}

	UPROPERTY()
	int32 LeaguePlacementMatches;

	UPROPERTY()
	int32 LeaguePoints;

	UPROPERTY()
	int32 LeagueTier;

	UPROPERTY()
	int32 LeagueDivision;

	UPROPERTY()
	int32 LeaguePromotionMatchesAttempted;

	UPROPERTY()
	int32 LeaguePromotionMatchesWon;

	UPROPERTY()
	bool bLeaguePromotionSeries;
};

UCLASS(config=Engine)
class UNREALTOURNAMENT_API UUTLocalPlayer : public ULocalPlayer
{
	GENERATED_UCLASS_BODY()

public:
	virtual ~UUTLocalPlayer();

	virtual bool IsMenuGame();

	virtual FString GetNickname() const;
	virtual FText GetAccountSummary() const;
	virtual FString GetAccountName() const;
	virtual FText GetAccountDisplayName() const;

	virtual void PlayerAdded(class UGameViewportClient* InViewportClient, int32 InControllerID) override;
	virtual void PlayerRemoved() override;

	virtual void ShowMenu(const FString& Parameters);
	virtual void HideMenu();
	virtual void OpenTutorialMenu();
	virtual void ShowToast(FText ToastText, float Lifetime=3.0f);	// NOTE: Need to add a type/etc so that they can be skinned better.
	virtual void ShowAdminMessage(FString Message);

	virtual void MessageBox(FText MessageTitle, FText MessageText);

	virtual bool Exec(UWorld* InWorld, const TCHAR* Cmd, FOutputDevice& Ar) override;

#if !UE_SERVER

	TArray<TSharedPtr<SUTWindowBase>> WindowStack;
	virtual void OpenWindow(TSharedPtr<SUTWindowBase> WindowToOpen);
	virtual bool CloseWindow(TSharedPtr<SUTWindowBase> WindowToClose);
	virtual void WindowClosed(TSharedPtr<SUTWindowBase> WindowThatWasClosed);

	virtual TSharedPtr<class SUTDialogBase> ShowMessage(FText MessageTitle, FText MessageText, uint16 Buttons, const FDialogResultDelegate& Callback = FDialogResultDelegate(), FVector2D DialogSize = FVector2D(0.0, 0.0f));
	virtual TSharedPtr<class SUTDialogBase> ShowSupressableConfirmation(FText MessageTitle, FText MessageText, FVector2D DialogSize, bool &InOutShouldSuppress, const FDialogResultDelegate& Callback = FDialogResultDelegate());

	/** utilities for opening and closing dialogs */
	virtual void OpenDialog(TSharedRef<class SUTDialogBase> Dialog, int32 ZOrder = 255);
	virtual void CloseDialog(TSharedRef<class SUTDialogBase> Dialog);
	TSharedPtr<class SUTServerBrowserPanel> GetServerBrowser();
	TSharedPtr<class SUTReplayBrowserPanel> GetReplayBrowser();
	TSharedPtr<class SUTStatsViewerPanel> GetStatsViewer();
	TSharedPtr<class SUTCreditsPanel> GetCreditsPanel();
	
	FString CurrentQuickMatchType;

	void StartQuickMatch(FString QuickMatchType);
	void CloseQuickMatch();

	TSharedPtr<class SUTMenuBase> GetCurrentMenu()
	{
		return DesktopSlateWidget;
	}

#endif

	UFUNCTION(BlueprintCallable, Category = UI)
	virtual bool AreMenusOpen();

	UFUNCTION()
	virtual void ChangeStatsViewerTarget(FString InStatsID);

	// Holds all of the chat this client has received.
	TArray<TSharedPtr<FStoredChatMessage>> ChatArchive;
	virtual void SaveChat(FName Type, FString Sender, FString Message, FLinearColor Color, bool bMyChat, uint8 TeamNum);

	UPROPERTY(Config)
		FString TutorialLaunchParams;

	UPROPERTY(Config)
		bool bFragCenterAutoPlay;

	UPROPERTY(Config)
		bool bFragCenterAutoMute;

	UPROPERTY(config)
		FString YoutubeAccessToken;

	UPROPERTY(config)
		FString YoutubeRefreshToken;

protected:

#if !UE_SERVER
	TSharedPtr<class SUTMenuBase> DesktopSlateWidget;
	TSharedPtr<class SUTSpectatorWindow> SpectatorWidget;

	// Holds a persistent reference to the server browser.
	TSharedPtr<class SUTServerBrowserPanel> ServerBrowserWidget;

	TSharedPtr<class SUTReplayBrowserPanel> ReplayBrowserWidget;
	TSharedPtr<class SUTStatsViewerPanel> StatsViewerWidget;
	TSharedPtr<class SUTCreditsPanel> CreditsPanelWidget;

	/** stores a reference to open dialogs so they don't get destroyed */
	TArray< TSharedPtr<class SUTDialogBase> > OpenDialogs;
	TArray<TSharedPtr<class SUTToastBase>> ToastList;

	virtual void AddToastToViewport(TSharedPtr<SUTToastBase> ToastToDisplay);
	void WelcomeDialogResult(TSharedPtr<SCompoundWidget> Widget, uint16 ButtonID);
	void OnSwitchUserResult(TSharedPtr<SCompoundWidget> Widget, uint16 ButtonID);
	TSharedPtr<class SUTQuickMatchWindow> QuickMatchDialog;
	TSharedPtr<class SUTLoginDialog> LoginDialog;

	TSharedPtr<class SUTAdminDialog> AdminDialog;

	virtual TSharedPtr<class SUTDialogBase> ShowLeagueMatchResultDialog(int32 Tier, int32 Division, FText MessageTitle, FText MessageText, uint16 Buttons, const FDialogResultDelegate& Callback = FDialogResultDelegate(), FVector2D DialogSize = FVector2D(0.0, 0.0f));
#endif

	bool bWantsToConnectAsSpectator;
	int32 ConnectDesiredTeam;
	int32 CurrentSessionTrustLevel;

public:
	FProcHandle DedicatedServerProcessHandle;

	UFUNCTION(BlueprintCallable, Category = Game)
	bool bIsQuickmatchDialogOpen()
	{
#if UE_SERVER
		return false;
#else
		return QuickMatchDialog.IsValid();
#endif
	}

	// Last text entered in Connect To IP
	UPROPERTY(config)
		FString LastConnectToIP;

	UPROPERTY(config)
		uint32 bNoMidGameMenu : 1;

	/** returns path for player's cosmetic item
	 * profile settings takes priority over .ini
	 */
	virtual FString GetHatPath() const;
	virtual void SetHatPath(const FString& NewHatPath);
	virtual FString GetLeaderHatPath() const;
	virtual void SetLeaderHatPath(const FString& NewLeaderHatPath);
	virtual FString GetEyewearPath() const;
	virtual void SetEyewearPath(const FString& NewEyewearPath);
	virtual int32 GetHatVariant() const;
	virtual void SetHatVariant(int32 NewVariant);
	virtual int32 GetEyewearVariant() const;
	virtual void SetEyewearVariant(int32 NewVariant);
	virtual FString GetGroupTauntPath() const;
	virtual void SetGroupTauntPath(const FString& NewGroupTauntPath);
	virtual FString GetTauntPath() const;
	virtual void SetTauntPath(const FString& NewTauntPath);
	virtual FString GetTaunt2Path() const;
	virtual void SetTaunt2Path(const FString& NewTauntPath);
	/** returns path for player's character (visual only data) */
	virtual FString GetCharacterPath() const;
	virtual void SetCharacterPath(const FString& NewCharacterPath);

	/** accessors for default URL options */
	virtual FString GetDefaultURLOption(const TCHAR* Key) const;
	virtual void SetDefaultURLOption(const FString& Key, const FString& Value);
	virtual void ClearDefaultURLOption(const FString& Key);

	// ONLINE ------

	// Called after creation on non-default objects to setup the online Subsystem
	virtual void InitializeOnlineSubsystem();

	// Returns true if this player is logged in to the UT Online Services.  If bIncludeProfile is true, then it will check to see
	// if the initial read of the profile and progression has completed as well
	virtual bool IsLoggedIn() const;

	virtual FString GetOnlinePlayerNickname();

	/**
	 *	Login to the Epic online serivces.
	 *
	 *  EpicID = the ID to login with.  Right now it's the's players UT forum id
	 *  Auth = is the auth-token or password to login with
	 *  bIsRememberToken = If true, Auth will be considered a remember me token
	 *  bSilentlyFail = If true, failure to login will not trigger a request for auth
	 *
	 **/
	virtual void LoginOnline(FString EpicID, FString Auth, bool bIsRememberToken = false, bool bSilentlyFail = false);

	// Log this local player out
	virtual void Logout();

	/** Called when the player has completed logging in, all data is downloaded but UI may still be showing */
	DECLARE_MULTICAST_DELEGATE(FPlayerLoggedInDelegate);
	FORCEINLINE FPlayerLoggedInDelegate& OnPlayerLoggedIn() { return PlayerLoggedIn; }

	/** Called when the player has completed logging out, may be about to return to main menu or fail login */
	DECLARE_MULTICAST_DELEGATE(FPlayerLoggedOutDelegate);
	FORCEINLINE FPlayerLoggedOutDelegate& OnPlayerLoggedOut() { return PlayerLoggedOut; }

	/**
	 *	Gives a call back to an object looking to know when a player's status changed.
	 **/
	virtual FDelegateHandle RegisterPlayerOnlineStatusChangedDelegate(const FPlayerOnlineStatusChanged::FDelegate& NewDelegate);

	/**
	 *	Removes the  call back to an object looking to know when a player's status changed.
	 **/
	virtual void RemovePlayerOnlineStatusChangedDelegate(FDelegateHandle DelegateHandle);


	/**
	 *	Registeres a delegate to get notifications of chat messages
	 **/
	virtual FDelegateHandle RegisterChatArchiveChangedDelegate(const FChatArchiveChanged::FDelegate& NewDelegate);

	/**
	 *	Removes a chat notification delegate.
	 **/
	virtual void RemoveChatArchiveChangedDelegate(FDelegateHandle DelegateHandle);

	virtual bool SkipWorldRender();

#if !UE_SERVER
	virtual void ToastCompleted();
	virtual void CloseAuth();
#endif

protected:

	bool bInitialSignInAttempt;

	// Holds the local copy of the player nickname.
	UPROPERTY(config)
	FString PlayerNickname;

	// What is the Epic ID associated with this player.
	UPROPERTY(config)
	FString LastEpicIDLogin;

	// The RememberMe Token for this player. 
	UPROPERTY(config)
	FString LastEpicRememberMeToken;
	
	// Called to insure the OSS is cleaned up properly
	virtual void CleanUpOnlineSubSystyem();

	// Out Delegates
	virtual void OnLoginComplete(int32 LocalUserNum, bool bWasSuccessful, const FUniqueNetId& UniqueID, const FString& ErrorMessage);
	virtual void OnLoginStatusChanged(int32 LocalUserNum, ELoginStatus::Type PreviousLoginStatus, ELoginStatus::Type LoginStatus, const FUniqueNetId& UniqueID);
	virtual void OnLogoutComplete(int32 LocalUserNum, bool bWasSuccessful);

	FPlayerOnlineStatusChanged PlayerOnlineStatusChanged;
	FChatArchiveChanged ChatArchiveChanged;

	double LastProfileCloudWriteTime;
	double ProfileCloudWriteCooldownTime;
	FTimerHandle ProfileWriteTimerHandle;

	// Hopefully the only magic number needed for profile versions, but being safe
	uint32 CloudProfileMagicNumberVersion1;

	// The last released serialization version
	int32 CloudProfileUE4VerForUnversionedProfile;

#if !UE_SERVER
	virtual void AuthDialogClosed(TSharedPtr<SCompoundWidget> Widget, uint16 ButtonID);
#endif

public:
	// Call this function to Attempt to load the Online Profile Settings for this user.
	virtual void GetAuth(FString ErrorMessage = TEXT(""));
	virtual void ShowAuth();

	UPROPERTY(config)
	FString LastRankedMatchUniqueId;

	UPROPERTY(config)
	FString LastRankedMatchSessionId;

	UPROPERTY(config)
	FString LastRankedMatchTimeString;

	UFUNCTION()
	void ShowRankedReconnectDialog(const FString& UniqueID);

#if !UE_SERVER
	virtual void RankedReconnectResult(TSharedPtr<SCompoundWidget> Widget, uint16 ButtonID);
	virtual void ContentAcceptResult(TSharedPtr<SCompoundWidget> Widget, uint16 ButtonID);
#endif

private:

	// Holds the Username of the pending user.  It's set in LoginOnline and cleared when there is a successful connection
	FString PendingLoginUserName;

	// If true, a login failure will not prompt for a password.  This is for the initial auto-login sequence
	bool bSilentLoginFail;

	bool bIsPendingProfileLoadFromMCP;
	bool bIsPendingProgressionLoadFromMCP;

	IOnlineSubsystem* OnlineSubsystem;
	IOnlineIdentityPtr OnlineIdentityInterface;
	IOnlineUserCloudPtr OnlineUserCloudInterface;
	IOnlineSessionPtr OnlineSessionInterface;
	IOnlinePresencePtr OnlinePresenceInterface;
	IOnlineFriendsPtr OnlineFriendsInterface;
	IOnlineTitleFilePtr OnlineTitleFileInterface;
	IOnlineUserPtr OnlineUserInterface;
	IOnlinePartyPtr OnlinePartyInterface;

	// Our delegate references....
	FDelegateHandle OnLoginCompleteDelegate;
	FDelegateHandle OnLoginStatusChangedDelegate;
	FDelegateHandle OnLogoutCompleteDelegate;

	FDelegateHandle OnEnumerateUserFilesCompleteDelegate;
	FDelegateHandle OnReadUserFileCompleteDelegate;
	FDelegateHandle OnWriteUserFileCompleteDelegate;
	FDelegateHandle OnDeleteUserFileCompleteDelegate;

	FDelegateHandle OnJoinSessionCompleteDelegate;
	FDelegateHandle OnEndSessionCompleteDelegate;
	FDelegateHandle OnDestroySessionCompleteDelegate;
	FDelegateHandle OnFindFriendSessionCompleteDelegate;

	FDelegateHandle OnReadTitleFileCompleteDelegate;
	FDelegateHandle OnEnumerateTitleFilesCompleteDelegate;

public:
	virtual void LoadProfileSettings();
	virtual void SaveProfileSettings();
	virtual void ApplyProfileSettings();
	virtual void ClearProfileSettings();

	UFUNCTION(BlueprintCallable, Category = Profile)
	virtual UUTProfileSettings* GetProfileSettings() { return CurrentProfileSettings; };

	virtual void SetNickname(FString NewName);

	FName TeamStyleRef(FName InName);

	virtual void LoadProgression();
	UFUNCTION()
		virtual void SaveProgression();
	virtual UUTProgressionStorage* GetProgressionStorage() { return CurrentProgression; }

	bool IsPendingMCPLoad() const;

protected:

	// Holds the current profile settings.  
	UPROPERTY()
		UUTProfileSettings* CurrentProfileSettings;

	UPROPERTY()
		UUTProgressionStorage* CurrentProgression;

	virtual FString GetProfileFilename();
	virtual FString GetProgressionFilename();
	virtual void ClearProfileWarnResults(TSharedPtr<SCompoundWidget> Widget, uint16 ButtonID);
	virtual void OnReadUserFileComplete(bool bWasSuccessful, const FUniqueNetId& InUserId, const FString& FileName);
	virtual void OnWriteUserFileComplete(bool bWasSuccessful, const FUniqueNetId& InUserId, const FString& FileName);
	virtual void OnDeleteUserFileComplete(bool bWasSuccessful, const FUniqueNetId& InUserId, const FString& FileName);
	virtual void OnEnumerateUserFilesComplete(bool bWasSuccessful, const FUniqueNetId& InUserId);

	void EnumerateTitleFiles();
	virtual void OnReadTitleFileComplete(bool bWasSuccessful, const FString& Filename);
	virtual void OnEnumerateTitleFilesComplete(bool bWasSuccessful);

#if !UE_SERVER
	TSharedPtr<class SUTDialogBase> HUDSettings;
#endif

public:
	virtual void ShowHUDSettings();
	virtual void HideHUDSettings();

	FString StripOptionsFromAddress(FString HostAddress) const;

	// NOTE: These functions are for getting the user's ELO rating from the cloud.  This
	// is temp code and will be changed so don't rely on it staying as is.
private:
	
	TMap<FString, FMMREntry> MMREntries;
	virtual void UpdateMMREntry(const FString& RatingName, const FMMREntry& InFMMREntry);

	TMap<FString, FRankedLeagueProgress> LeagueRecords;
	virtual void UpdateLeagueProgress(const FString& LeagueName, const FRankedLeagueProgress& InLeagueProgress);
		
	bool bProgressionReadFromCloud;
	void ReadMMRFromBackend();
	void ReadLeagueFromBackend(const FString& MatchRatingType);

	void ReportStarsToServer();

	void ReadCloudFileListing();
public:

	void ReadSpecificELOFromBackend(const FString& MatchRatingType);

	// Returns the filename for stats.
	static FString GetStatsFilename() { return TEXT("stats.json"); }

	// Returns the base ELO Rank with any type of processing we need.
	virtual int32 GetBaseELORank();
	
	virtual bool GetLeagueProgress(const FString& LeagueName, FRankedLeagueProgress& LeagueProgress);
	virtual bool GetMMREntry(const FString& RatingName, FMMREntry& MMREntry);

	// Returns the # of stars to show based on XP value. 
	UFUNCTION(BlueprintCallable, Category = Badge)
	static void GetStarsFromXP(int32 XPValue, int32& StarLevel);


	// Connect to a server via the session id.  Returns TRUE if the join continued, or FALSE if it failed to start
	virtual bool JoinSession(const FOnlineSessionSearchResult& SearchResult, bool bSpectate, int32 DesiredTeam = -1, FString InstanceId=TEXT(""));
	virtual void CancelJoinSession();
	virtual void LeaveSession();
	virtual void ReturnToMainMenu();

	// Updates this user's online presence
	void UpdatePresence(FString NewPresenceString, bool bAllowInvites, bool bAllowJoinInProgress, bool bAllowJoinViaPresence, bool bAllowJoinViaPresenceFriendsOnly);

	// Does the player have pending social notifications - should the social bang be shown?
	bool IsPlayerShowingSocialNotification() const;
	
protected:
	virtual void JoinPendingSession();
	virtual void OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result);
	virtual void OnEndSessionComplete(FName SessionName, bool bWasSuccessful);
	virtual void OnDestroySessionComplete(FName SessionName, bool bWasSuccessful);

	virtual void OnPresenceUpdated(const FUniqueNetId& UserId, const bool bWasSuccessful);
	virtual void OnPresenceReceived(const FUniqueNetId& UserId, const TSharedRef<FOnlineUserPresence>& Presence);

	FString PendingInstanceID;

	// Set to true if we have delayed joining a session (due to already being in a session or some other reason.  PendingSession will contain the session data.
	bool bDelayedJoinSession;
	FOnlineSessionSearchResult PendingSession;

public:

	// Holds the session info of the last session this player tried to join.  If there is a join failure, or the reconnect command is used, this session info
	// will be used to attempt the reconnection.
	FOnlineSessionSearchResult LastSession;

	// Will be true if the last session was a join as spectator
	bool bLastSessionWasASpectator;

protected:

	// friend join functionality
	virtual void JoinFriendSession(const FUniqueNetId& FriendId, const FUniqueNetId& SessionId);
	virtual void OnFindFriendSessionComplete(int32 LocalUserNum, bool bWasSuccessful, const FOnlineSessionSearchResult& SearchResult);
	virtual void HandleFriendsJoinGame(const FUniqueNetId& FriendId, const FUniqueNetId& SessionId);
	virtual bool AllowFriendsJoinGame();
	virtual void HandleFriendsNotificationAvail(bool bAvailable);
	virtual void HandleFriendsActionNotification(TSharedRef<FFriendsAndChatMessage> FriendsAndChatMessage);

	FString PendingFriendInviteSessionId;	
	FString PendingFriendInviteFriendId;
	bool bShowSocialNotification;

#if !UE_SERVER

	 TSharedPtr<SOverlay> ContentLoadingMessage;

public:
	virtual void ShowContentLoadingMessage();
	virtual void HideContentLoadingMessage();

	virtual TSharedPtr<SUTFriendsPopupWindow> GetFriendsPopup();
	virtual void SetShowingFriendsPopup(bool bShowing);
	bool bShowingFriendsMenu;

protected:
	TSharedPtr<SUTFriendsPopupWindow> FriendsMenu;
#endif

	// If the player is not logged in, then this string will hold the last attempted presence update
	FString LastPresenceUpdate;
	bool bLastAllowInvites;

public:
	virtual FName GetCountryFlag();
	virtual FName GetAvatar();

	// Combined and moved to the MCPPRofile so the backend has access to this information
	virtual void SetCountryFlagAndAvatar(FName NewFlag, FName NewAvatar);


protected:
	bool bPendingLoginCreds;
	FString PendingLoginName;
	FString PendingLoginPassword;

public:
	bool IsAFriend(FUniqueNetIdRepl PlayerId);

	UPROPERTY(Config)
	uint32 bShowBrowserIconOnMainMenu:1;

	bool bSuppressToastsInGame;

protected:

#if !UE_SERVER
	TWeakPtr<SUTDialogBase> ConnectingDialog;
	TSharedPtr<SUTDownloadAllDialog> DownloadAllDialog;
	TSharedPtr<SUTDialogBase> DLCWarningDialog;
#endif

	// Holds a list of servers where the DLC warning has been accepted.  If the current server is in this list
	// then the DLC content warning will not be displayed this run.
	TArray<FString> AcceptedDLCServers;

public:

	virtual void ShowRedirectDownload();
	virtual void HideRedirectDownload();

	// Checks to see if a redirect exists
	virtual bool ContentExists(const FPackageRedirectReference& Redirect);

	// Attempt to Accquire redirect content
	virtual void AcquireContent(TArray<FPackageRedirectReference>& Redirects);

	// Cancel all active downloads.
	virtual void CancelDownload();

	// Returns an FTEXT with the download status
	virtual FText GetDownloadStatusText();

	virtual FText GetDownloadFilename();

	virtual int32 GetNumberOfPendingDownloads();

	virtual int32 GetDownloadBytes();

	virtual float GetDownloadProgress();

	// If set to true, the download dialog will be suppressed and the status information
	// needs to be displayed elsewhere.
	bool bSuppressDownloadDialog;

	// Returns TRUE if downloads are in progress
	virtual bool IsDownloadInProgress();

	// On a hub, this will request that the hub send over all of the content needed to download.
	void RequestServerSendAllRedirects();

	// Shows the DLC warning dialog.  
	void ShowDLCWarning();

	// returns true if the DLC warning is being displayed
	bool IsShowingDLCWarning();

	// returns true if this server requires a DLC warning to be shown
	bool RequiresDLCWarning();

protected:

	virtual void ConnectingDialogCancel(TSharedPtr<SCompoundWidget> Dialog, uint16 ButtonID);
public:
	virtual void ShowConnectingDialog();
	virtual void CloseConnectingDialog();

	virtual int32 GetFriendsList(TArray< FUTFriend >& OutFriendsList);
	virtual int32 GetRecentPlayersList(TArray< FUTFriend >& OutRecentPlayersList);

	// returns true if this player is in a session
	virtual bool IsInSession();

	UPROPERTY(config)
	int32 ServerPingBlockSize;

	virtual void ShowPlayerInfo(TWeakObjectPtr<AUTPlayerState> Target, bool bAllowLogout=false);
	virtual void OnTauntPlayed(AUTPlayerState* PS, TSubclassOf<AUTTaunt> TauntToPlay, float EmoteSpeed);
	virtual void OnEmoteSpeedChanged(AUTPlayerState* PS, float EmoteSpeed);

	// Request someone be my friend...
	UFUNCTION(BlueprintCallable, Category = Friends)
	void SendFriendRequest(AUTPlayerState* DesiredPlayerState);

	virtual void RequestFriendship(TSharedPtr<const FUniqueNetId> FriendID);

	// Holds a list of maps to play in Single player
	TArray<FString> SinglePlayerMapList;

	// Forward any network failure messages to the base player controller so that game specific actions can be taken
	virtual void HandleNetworkFailureMessage(enum ENetworkFailure::Type FailureType, const FString& ErrorString);

protected:
#if !UE_SERVER
	TSharedPtr<SUTMenuBase> LoadoutMenu;
	TSharedPtr<SUTMapVoteDialog> MapVoteMenu;
#endif

public:
	virtual void OpenLoadout(bool bBuyMenu = false);
	virtual void CloseLoadout();

	virtual void OpenMapVote(AUTGameState* GameState);
	virtual void CloseMapVote();

	virtual void OpenMatchSummary(AUTGameState* GameState);
	virtual void CloseMatchSummary();

	// What is your role within the unreal community.
	EUnrealRoles::Type CommunityRole;

	//Replay Stuff
#if !UE_SERVER
	TSharedPtr<SUTReplayWindow> ReplayWindow;
#endif
	void OpenReplayWindow();
	void CloseReplayWindow();
	void ToggleReplayWindow();

	virtual bool IsReplay();

#if !UE_SERVER
	void RecordReplay(float RecordTime);
	void RecordingReplayComplete();
	void GetYoutubeConsentForUpload();
	void ShouldVideoCompressDialogResult(TSharedPtr<SCompoundWidget> Widget, uint16 ButtonID);
	void VideoCompressDialogResult(TSharedPtr<SCompoundWidget> Widget, uint16 ButtonID);
	void ShouldVideoUploadDialogResult(TSharedPtr<SCompoundWidget> Widget, uint16 ButtonID);
	void YoutubeConsentResult(TSharedPtr<SCompoundWidget> Widget, uint16 ButtonID);
	void YoutubeUploadResult(TSharedPtr<SCompoundWidget> Widget, uint16 ButtonID);
	void YoutubeUploadCompleteResult(TSharedPtr<SCompoundWidget> Widget, uint16 ButtonID);
	void YoutubeTokenRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded);
	void YoutubeTokenRefreshComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded);
	void UploadVideoToYoutube();
	bool bRecordingReplay;
	FString RecordedReplayFilename;
	FString RecordedReplayTitle;
	TSharedPtr<SUTDialogBase> YoutubeDialog;
	TSharedPtr<class SUTYoutubeConsentDialog> YoutubeConsentDialog;

	TSharedPtr<class SUTMatchmakingRegionDialog> MatchmakingRegionDialog;
	void RegionSelectResult(TSharedPtr<SCompoundWidget> Widget, uint16 ButtonID, int32 InPlaylistId);

	TSharedPtr<class SUTMatchmakingDialog> MatchmakingDialog;
	void MatchmakingResult(TSharedPtr<SCompoundWidget> Widget, uint16 ButtonID);
#endif

	void ShowRegionSelectDialog(int32 InPlaylistId);
	void ShowMatchmakingDialog();
	void HideMatchmakingDialog();
	bool IsPartyLeader();

#if !UE_SERVER
	TSharedPtr<SUTDialogBase> LeagueMatchResultsDialog;

	TSharedPtr<SUTDialogBase> GameAbandonedDialog;
	void ShowGameAbandonedDialog();
#endif

	virtual void VerifyGameSession(const FString& ServerSessionId);

	/** Closes any slate UI elements that are open. **/
	virtual void CloseAllUI(bool bExceptDialogs = false);

protected:
	void OnFindSessionByIdComplete(int32 LocalUserNum, bool bWasSucessful, const FOnlineSessionSearchResult& SearchResult);
	
	// Will be true if we are attempting to force the player in to an existing session.
	bool bAttemptingForceJoin;

	/** Used to avoid reading too often */
	double LastItemReadTime;


public:
	virtual void HandleProfileNotification(const FOnlineNotification& Notification);

	/** returns whether the user owns an item that grants the asset (cosmetic, character, whatever) with the given path */
	bool OwnsItemFor(const FString& Path, int32 VariantId = 0) const;

	float QuickMatchLimitTime;

	inline int32 GetOnlineXP() const
	{
#if WITH_PROFILE
		if (GetMcpProfileManager() && GetMcpProfileManager()->GetMcpProfileAs<UUtMcpProfile>(EUtMcpProfile::Profile))
		{
			return GetMcpProfileManager()->GetMcpProfileAs<UUtMcpProfile>(EUtMcpProfile::Profile)->GetXP();
		}

		return 0;
#else
		return 0;
#endif
	}
	inline void AddProfileItem(const UUTProfileItem* NewItem)
	{
		/*LastItemReadTime = 0.0;
		for (FProfileItemEntry& Entry : ProfileItems)
		{
			if (Entry.Item == NewItem)
			{
				Entry.Count++;
				return;
			}
		}
		new(ProfileItems) FProfileItemEntry(NewItem, 1);*/
	}

	bool IsOnTrustedServer() const
	{
		return GetWorld()->GetNetMode() == NM_Client && IsLoggedIn() && CurrentSessionTrustLevel <= 1;
	}

	bool IsEarningXP() const;

	virtual void AttemptJoinInstance(TSharedPtr<FServerData> ServerData, FString InstanceId, bool bSpectate);
	virtual void CloseJoinInstanceDialog();

protected:
#if !UE_SERVER
	TSharedPtr<SUTJoinInstanceWindow> JoinInstanceDialog;
#endif

public:
	/** Set at end of match if earned roster upgrade. */
	UPROPERTY()
		FText RosterUpgradeText;

	/** Set at end of match if earned new stars. */
	UPROPERTY()
		int32 EarnedStars;
	
	/**
	 * Returns the killcam manager object for this player.
	 */
	UUTKillcamPlayback* GetKillcamPlaybackManager() const { return KillcamPlayback; }

	// Returns the Total # of stars collected by this player.
	int32 GetTotalChallengeStars();

	// Returns the # of stars for a given challenge tag.  Returns 0 if this challenge hasn't been started
	int32 GetChallengeStars(FName ChallengeTag);

	// Returns the total # of stars in a given reward group
	int32 GetRewardStars(FName RewardTag);

	// Returns the date a challenge was last updated as a string
	FString GetChallengeDate(FName ChallengeTag);

	// Marks a challenge as completed.
	void ChallengeCompleted(FName ChallengeTag, int32 Stars);
	
	void AwardAchievement(FName AchievementName);
	bool QuickMatchCheckFull();
	void RestartQuickMatch();

	static const FString& GetMCPStorageFilename()
	{
		const static FString MCPStorageFilename = "UnrealTournmentMCPStorage.json";
		return MCPStorageFilename;
	}


	static const FString& GetRankedPlayFilename()
	{
		const static FString RankedPlayFilename = "UnrealTournamentRankedPlay.json";
		return RankedPlayFilename;
	}
	bool IsRankedMatchmakingEnabled(int32 PlaylistId);
	TArray<int32> ActiveRankedPlaylists;
	FOnRankedPlaylistsChangedDelegate OnRankedPlaylistsChanged;

	/** Get the MCP account ID for this player */
	FUniqueNetIdRepl GetGameAccountId() const;

	// Holds data pulled from the MCP upon login.
	FMCPPulledData MCPPulledData;

	// Holds the current challenge update count.  It should only be set when you 
	// enter the challenge menu.
	UPROPERTY(config)
	int32 ChallengeRevisionNumber;

	void ShowAdminDialog(AUTRconAdminInfo* AdminInfo);
	void AdminDialogClosed();

#if !UE_SERVER

	virtual int32 NumDialogsOpened();
#endif

public:
	
	void PostInitProperties() override;

public:
	virtual void OpenSpectatorWindow();
	virtual void CloseSpectatorWindow();

	bool IsFragCenterNew();
	void UpdateFragCenter();

#if WITH_PROFILE
	/** Get manager for the McpProfiles for this user */
	UUtMcpProfileManager* GetMcpProfileManager() const { return Cast<UUtMcpProfileManager>(McpProfileManager); }

	/** Get profile manager for a specific account (can be this user).  "" will return users.  Use non-param version if it is known to have to be users (non-shared) */
	UUtMcpProfileManager* GetMcpProfileManager(const FString& AccountId);
	UUtMcpProfileManager* GetActiveMcpProfileManager() const { return Cast<UUtMcpProfileManager>(ActiveMcpProfileManager); }

#endif

	/** Matchmaking related items */
	void StartMatchmaking(int32 PlaylistId);

	FDelegateHandle MatchmakingReconnectResultHandle;
	void AttemptMatchmakingReconnect(const FString& OldSessionId);
	void AttemptMatchmakingReconnectResult(EMatchmakingCompleteResult Result);

	void InvalidateLastSession();
	void Reconnect(bool bAsSpectator);

	void CachePassword(FString HostAddress, FString Password, bool bSpectator);		
	FString RetrievePassword(FString HostAddress, bool bSpectator);

protected:
	TMap<FString /*HostIP:Port*/, FString /*Password*/> CachedPasswords;
	TMap<FString /*HostIP:Port*/, FString /*Password*/> CachedSpecPasswords;

protected:
	UPROPERTY(Config)
	int32 FragCenterCounter;

	/** Party related items */
	void CreatePersistentParty();
	void DelayedCreatePersistentParty();
	FTimerHandle PersistentPartyCreationHandle;
	
	bool bCancelJoinSession;

	void OnProfileManagerInitComplete(bool bSuccess, const FText& ErrorText);
	void UpdateSharedProfiles();
	void UpdateSharedProfiles(const FUTProfilesLoaded& Callback);
	FUTProfilesLoaded UpdateSharedProfilesComplete;

	/** Our main account with associated profiles. */
	UPROPERTY()
	UObject* McpProfileManager;

	/** Current active one - it may be shared profile or main profile. */
	UPROPERTY()
	UObject* ActiveMcpProfileManager;

	/** Any shared accounts with associated profiles. */
	UPROPERTY()
	TArray<UObject*> SharedMcpProfileManager;

	TSharedPtr<SUTMatchSummaryPanel> GetSummaryPanel();

	// Check to see if this user should be using the Epic branded flag
	void EpicFlagCheck();

	FString PendingGameMode;

	FPlayerLoggedInDelegate PlayerLoggedIn;
	FPlayerLoggedOutDelegate PlayerLoggedOut;

public:

#if !UE_SERVER
	TSharedPtr<SUTChatEditBox> ChatWidget;

	virtual void VerifyChatWidget();

	TSharedPtr<SUTChatEditBox> GetChatWidget();
	virtual void FocusWidget(TSharedPtr<SWidget> WidgetToFocus);
#endif

	FText UIChatTextBuffer;
	FText GetUIChatTextBackBuffer(int Direction);
	void UpdateUIChatTextBackBuffer(const FText& NewText);

	void ShowQuickChat(FName ChatDestination);
	void CloseQuickChat();

	UPROPERTY()
	bool bAutoRankLockWarningShown;

protected:

	UPROPERTY()
	UUTKillcamPlayback* KillcamPlayback;

#if !UE_SERVER
	TSharedPtr<SUTQuickChatWindow> QuickChatWindow;
#endif

	int32 UIChatTextBackBufferPosition;
	TArray<FText> UIChatTextBackBuffer;

	bool bJoinSessionInProgress;	
	FDelegateHandle SpeakerDelegate;
	void OnPlayerTalkingStateChanged(TSharedRef<const FUniqueNetId> TalkerId, bool bIsTalking);

public:

	// Returns true if there is chat text available.
	virtual bool HasChatText();

protected:

	// Loads the local profile from disk.  This happens immediately upon creation and again when a logout occurs.  
	virtual void LoadLocalProfileSettings();

	// Saving of any profile saves locally, as does logging in.  NOTE: There is no toast this with save, it is silent
	virtual void SaveLocalProfileSettings();

#if !UE_SERVER
	void OnUpgradeResults(TSharedPtr<SCompoundWidget> Widget, uint16 ButtonID);
#endif

public:
	UPROPERTY(BlueprintReadOnly, Category = Login)
	bool bLoginAttemptInProgress;

	// Will be true if the user has chosen to play offline.  It get's cleared when a player logs in.
	UPROPERTY(BlueprintReadOnly, Category = Login)
	bool bPlayingOffline;

	UPROPERTY()
	bool bCloseUICalledDuringMoviePlayback;

	UPROPERTY()
	bool bDelayedCloseUIExcludesDialogs;


	UPROPERTY()
	bool bHideMenuCalledDuringMoviePlayback;

	void AttemptLogin();

	void ClearPendingLoginUserName();

	// Defines where we are in the login process.  We stall the game during the whole login process.
	UPROPERTY()
	TEnumAsByte<ELoginPhase::Type> LoginPhase;

	// Called when the entire login process is completed.  This includes loading the profile and progression
	virtual void LoginProcessComplete();

	virtual void SetTutorialFinished(int32 TutorialMask);

	UFUNCTION(BlueprintCallable, Category = UI)
	bool IsSystemMenuOpen();

	virtual void InitializeSocial();

	bool SkipTutorialCheck();

	// Returns true if this local player is currently in a party
	bool IsInAnActiveParty();

	virtual bool IsMenuOptionLocked(FName MenuCommand) const;
	virtual	EVisibility IsMenuOptionLockVisible(FName MenuCommand) const;
	virtual	bool IsMenuOptionEnabled(FName MenuCommand) const;

	virtual FText GetMenuCommandTooltipText(FName MenuCommand) const;

	UPROPERTY(config)
	TArray<FTutorialData> TutorialData;

	UFUNCTION(BlueprintCallable, Category="Tutorial")
	virtual void LaunchTutorial(FName TutorialName, const FString& DesiredQuickmatchType = TEXT(""));

	UFUNCTION(BlueprintCallable, Category="Tutorial")
	virtual bool LaunchPendingQuickmatch();

	UFUNCTION(BlueprintCallable, Category="Tutorial")
	virtual void RestartTutorial();

	UFUNCTION(BlueprintCallable, Category="Tutorial")
	virtual FText GetTutorialSectionText(TEnumAsByte<ETutorialSections::Type> Section) const;

protected:
	UPROPERTY()
	bool bQuickmatchOnLevelChange;
	
	UPROPERTY()
	FString PendingQuickmatchType;

	UPROPERTY()
	FName LastTutorial;

public:
	/** Hides/Shows the friends and chat panel */
	virtual FReply ToggleFriendsAndChat();


};

