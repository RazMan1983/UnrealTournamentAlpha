// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "ModuleInterface.h"
#include "Toolkits/IToolkit.h"	// For EAssetEditorMode

DECLARE_LOG_CATEGORY_EXTERN(LogBehaviorTreeEditor, Log, All);

class IBehaviorTreeEditor;

/** DataTable Editor module */
class FBehaviorTreeEditorModule : public IModuleInterface,
	public IHasMenuExtensibility, public IHasToolBarExtensibility
{

public:
	// IModuleInterface
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	/** Creates an instance of Behavior Tree editor.  Only virtual so that it can be called across the DLL boundary. */
	virtual TSharedRef<IBehaviorTreeEditor> CreateBehaviorTreeEditor( const EToolkitMode::Type Mode, const TSharedPtr< class IToolkitHost >& InitToolkitHost, class UObject* Object );

	/** Gets the extensibility managers for outside entities to extend static mesh editor's menus and toolbars */
	virtual TSharedPtr<FExtensibilityManager> GetMenuExtensibilityManager() {return MenuExtensibilityManager;}
	virtual TSharedPtr<FExtensibilityManager> GetToolBarExtensibilityManager() {return ToolBarExtensibilityManager;}

	TSharedPtr<struct FClassBrowseHelper> GetClassCache() { return ClassCache; }

	/** Behavior Tree app identifier string */
	static const FName BehaviorTreeEditorAppIdentifier;

private:
	TSharedPtr<FExtensibilityManager> MenuExtensibilityManager;
	TSharedPtr<FExtensibilityManager> ToolBarExtensibilityManager;

	/** Asset type actions */
	TArray<TSharedPtr<class FAssetTypeActions_Base>> ItemDataAssetTypeActions;

	TSharedPtr<struct FClassBrowseHelper> ClassCache;
};


