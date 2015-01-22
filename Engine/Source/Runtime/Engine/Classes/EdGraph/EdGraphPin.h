// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "EdGraph/EdGraphNode.h"
#include "EdGraphPin.generated.h"

USTRUCT()
struct FSimpleMemberReference
{
	GENERATED_USTRUCT_BODY()

	/** Class that this member is defined in. */
	UPROPERTY()
	TSubclassOf<class UObject> MemberParentClass;

	/** Name of the member */
	UPROPERTY()
	FName MemberName;

	/** The Guid of the member */
	UPROPERTY()
	FGuid MemberGuid;

	void Reset()
	{
		operator=(FSimpleMemberReference());
	}

	bool operator==(const FSimpleMemberReference& Other) const
	{
		return (MemberParentClass == Other.MemberParentClass)
			&& (MemberName == Other.MemberName)
			&& (MemberGuid == Other.MemberGuid);
	}
};

FORCEINLINE FArchive& operator<<(FArchive& Ar, FSimpleMemberReference& Data)
{
	Ar << Data.MemberParentClass;
	Ar << Data.MemberName;
	Ar << Data.MemberGuid;
	return Ar;
}


/** Struct used to define the type of information carried on this pin */
USTRUCT()
struct FEdGraphPinType
{
	GENERATED_USTRUCT_BODY()

	/** Category of pin type */
	UPROPERTY()
	FString PinCategory;

	/** Sub-category of pin type */
	UPROPERTY()
	FString PinSubCategory;

	/** Sub-category object */
	UPROPERTY()
	TWeakObjectPtr<UObject> PinSubCategoryObject;

	/** Sub-category member reference */
	UPROPERTY()
	FSimpleMemberReference PinSubCategoryMemberReference;

	/** Whether or not this pin represents an array of values */
	UPROPERTY()
	bool bIsArray;

	/** Whether or not this pin is a value passed by reference or not */
	UPROPERTY()
	bool bIsReference;

	/** Whether or not this pin is a immutable const value */
	UPROPERTY()
	bool bIsConst;

	/** Whether or not this is a weak reference */
	UPROPERTY()
	bool bIsWeakPointer;

public:
	FEdGraphPinType() 
	{
		PinSubCategoryObject = NULL;
		bIsArray = false;
		bIsReference = false;
		bIsWeakPointer = false;
	}
	FEdGraphPinType(const FString& InPinCategory, const FString& InPinSubCategory, UObject* InPinSubCategoryObject, bool bInIsArray, bool bInIsReference)
	{
		PinCategory = InPinCategory;
		PinSubCategory = InPinSubCategory;
		PinSubCategoryObject = InPinSubCategoryObject;
		bIsArray = bInIsArray;
		bIsReference = bInIsReference;
		bIsWeakPointer = false;
	}
	bool operator == ( const FEdGraphPinType& Other ) const
	{
		return (PinCategory == Other.PinCategory) 
			&& (PinSubCategory == Other.PinSubCategory) 
			&& (PinSubCategoryObject == Other.PinSubCategoryObject) 
			&& (bIsArray == Other.bIsArray) 
			&& (bIsReference == Other.bIsReference)
			&& (bIsWeakPointer == Other.bIsWeakPointer)
			&& (PinSubCategoryMemberReference == Other.PinSubCategoryMemberReference);
	}
	bool operator != ( const FEdGraphPinType& Other ) const
	{
		return (PinCategory != Other.PinCategory) 
			|| (PinSubCategory != Other.PinSubCategory) 
			|| (PinSubCategoryObject != Other.PinSubCategoryObject) 
			|| (bIsArray != Other.bIsArray) 
			|| (bIsReference != Other.bIsReference)
			|| (bIsWeakPointer != Other.bIsWeakPointer);
	}

	void ResetToDefaults()
	{
		PinCategory.Empty();
		PinSubCategory.Empty();
		PinSubCategoryObject = NULL;
		PinSubCategoryMemberReference.Reset();
		bIsArray = false;
		bIsReference = false;
		bIsWeakPointer = false;
	}

	bool Serialize(FArchive& Ar);
};

template<>
struct TStructOpsTypeTraits< FEdGraphPinType > : public TStructOpsTypeTraitsBase
{
	enum 
	{
		WithSerializer = true,
	};
};

UENUM()
enum EBlueprintPinStyleType
{
	BPST_Original UMETA(DisplayName="Circles, Grid, Diamond"),
	BPST_VariantA UMETA(DisplayName="Directional Circles")
};

UCLASS(MinimalAPI)
class UEdGraphPin : public UObject
{
	GENERATED_UCLASS_BODY()

	/** Name of this pin */
	UPROPERTY()
	FString PinName;

	/** Used as the display name if set */
	UPROPERTY()
	FText PinFriendlyName;

	/** The tool-tip describing this pin's purpose */
	UPROPERTY()
	FString PinToolTip;

	/** Direction of flow of this pin (input or output) */
	UPROPERTY()
	TEnumAsByte<enum EEdGraphPinDirection> Direction;

	/** The type of information carried on this pin */
	UPROPERTY()
	struct FEdGraphPinType PinType;

	/** Default value for this pin (used if the pin has no connections), stored as a string */
	UPROPERTY()
	FString DefaultValue;

	/** Initial default value (the autogenerated value, to identify if the user has modified the value), stored as a string */
	UPROPERTY()
	FString AutogeneratedDefaultValue;

	/** If the default value for this pin should be an object, we store a pointer to it */
	UPROPERTY()
	class UObject* DefaultObject;

	/** If the default value for this pin should be an FText, it is stored here. */
	UPROPERTY()
	FText DefaultTextValue;

	/** Set of pins that we are linked to */
	UPROPERTY()
	TArray<class UEdGraphPin*> LinkedTo;

	/** The pins created when a pin is split and hidden */ 
	UPROPERTY()
	TArray<class UEdGraphPin*> SubPins;

	/** The pin that was split and generated this pin */
	UPROPERTY()
	UEdGraphPin* ParentPin;

#if WITH_EDITORONLY_DATA
	/** If true, this connector is currently hidden. */
	UPROPERTY()
	uint32 bHidden:1;

	/** If true, this connector is unconnectable, and present only to allow the editing of the default text. */
	UPROPERTY()
	uint32 bNotConnectable:1;

	/** If true, the default value of this connector is fixed and cannot be modified by the user (it's visible for reference only) */
	UPROPERTY()
	uint32 bDefaultValueIsReadOnly:1;

	/** If true, the default value on this pin is ignored and should not be set */
	UPROPERTY()
	uint32 bDefaultValueIsIgnored:1;

	/** If true, this pin is the focus of a diff */
	UPROPERTY(transient)
	uint32 bIsDiffing:1;

	/** If true, the pin may be hidden by user */
	UPROPERTY()
	uint32 bAdvancedView:1;

	/** Pin name could be changed, so whenever possible it's good to have a persistent GUID identifying Pin to reconstruct Node seamlessly */
	UPROPERTY()
	FGuid PersistentGuid;
#endif // WITH_EDITORONLY_DATA

public:
	/** Create a link. Note, this does not check that schema allows it, and will not break any existing connections */
	ENGINE_API void MakeLinkTo(UEdGraphPin* ToPin);

	/** Break a link to the specified pin (if present) */
	ENGINE_API void BreakLinkTo(UEdGraphPin* ToPin);

	/**
	 * Generate a string detailing the link this pin has to another pin.
	 * 
	 * @Param	InFunctionName	String with function name requesting the info
	 * @Param	InInfoData		String detailing the info (EG. Is Not linked to)
	 * @Param	InToPin			The relevant pin
	 */
	const FString GetLinkInfoString( const FString& InFunctionName, const FString& InInfoData, const UEdGraphPin* InToPin ) const;

	/** Break all links from this pin */
	ENGINE_API void BreakAllPinLinks();

	/**
	 * Copies the persistent data (across a node refresh) from the SourcePin.
	 *
	 * @param	SourcePin	Source pin.
	 */
	ENGINE_API void CopyPersistentDataFromOldPin(const UEdGraphPin& SourcePin);

	/** Returns the node that owns this pin */
	class UEdGraphNode* GetOwningNode() const
	{
		return CastChecked<UEdGraphNode>(GetOuter());
	}

	class UEdGraphNode* GetOwningNodeUnchecked() const
	{
		return Cast<UEdGraphNode>(GetOuter());
	}

	/** Shorthand way to access the schema of the graph that owns the node that owns this pin */
	ENGINE_API const class UEdGraphSchema* GetSchema() const;

	/** Direction flipping utility; returns the complementary direction */
	static EEdGraphPinDirection GetComplementaryDirection(EEdGraphPinDirection InDirection)
	{
		return (InDirection == EGPD_Input) ? EGPD_Output : EGPD_Input;
	}

	/** Get the current DefaultObject path name, or DefaultValue if its null */
	ENGINE_API FString GetDefaultAsString() const;

	/** Returns how the name of the pin should be displayed in the UI */
	ENGINE_API FText GetDisplayName() const;

	void ResetDefaultValue()
	{
		DefaultValue.Empty();
		DefaultObject = nullptr;
		DefaultTextValue = FText::GetEmpty();
	}

	void ResetToDefaults()
	{
		check(LinkedTo.Num() == 0);

		PinType.ResetToDefaults();

		PinName.Empty();
		PinFriendlyName = FText::GetEmpty();
		AutogeneratedDefaultValue.Empty();
		ResetDefaultValue();

#if WITH_EDITORONLY_DATA
		bHidden = false;
		bNotConnectable = false;
		bDefaultValueIsReadOnly = false;
		bDefaultValueIsIgnored = false;
#endif // WITH_EDITORONLY_DATA
	}
};


