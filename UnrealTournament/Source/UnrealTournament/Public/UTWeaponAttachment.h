// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.
#pragma once

#include "UTWeaponAttachment.generated.h"

/** the third person representation of a weapon
 * not spawned on dedicated servers
 */
UCLASS(Blueprintable, NotPlaceable, Abstract)
class AUTWeaponAttachment : public AActor
{
	GENERATED_UCLASS_BODY()

protected:
	/** weapon class that resulted in this attachment; set at spawn time so no need to be reflexive here, just set AttachmentType in UTWeapon */
	UPROPERTY(BlueprintReadOnly, Category = "Weapon")
	TSubclassOf<AUTWeapon> WeaponType;
	/** precast of Instigator to UTCharacter */
	UPROPERTY(BlueprintReadOnly, Category = "Weapon")
	AUTCharacter* UTOwner;
	/** overlay mesh for overlay effects */
	UPROPERTY()
	USkeletalMeshComponent* OverlayMesh;
public:

	/** Tells the aniamtion system what movement blends to use */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	int32 WeaponStance;

	/** particle component for muzzle flash */
	UPROPERTY(EditAnywhere, Category = "Weapon")
	TArray<UParticleSystemComponent*> MuzzleFlash;

	/** third person mesh */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
	TSubobjectPtr<USkeletalMeshComponent> Mesh;
	/** third person mesh attach point */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	FName AttachSocket;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	FVector AttachOffset;
	/** the attachment Mesh is also used for the pickup; set this override to non-zero to override the scale factor when used that way */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Pickup)
	FVector PickupScaleOverride;

	/** particle system for firing effects (instant hit beam and such)
	* particles will be sourced at FireOffset and a parameter HitLocation will be set for the target, if applicable
	*/
	UPROPERTY(EditAnywhere, Category = "Weapon")
	TArray<UParticleSystem*> FireEffect;
	/** optional effect for instant hit endpoint */
	UPROPERTY(EditAnywhere, Category = "Weapon")
	TArray< TSubclassOf<class AUTImpactEffect> > ImpactEffect;

	virtual void BeginPlay() override;
	virtual void RegisterAllComponents() override;
	virtual void Destroyed() override;

	UFUNCTION(BlueprintNativeEvent)
	void AttachToOwner();
	UFUNCTION(BlueprintNativeEvent)
	void DetachFromOwner();

	/** play firing effects (both muzzle flash and any tracers/beams/impact effects)
	 * use UTOwner's FlashLocation and FireMode to determine firing data
	 * don't play sounds as those are played/replicated from UTWeapon by the server as the Pawn/WeaponAttachment may not be relevant
	 */
	UFUNCTION(BlueprintCallable, Category = "Weapon")
	virtual void PlayFiringEffects();
	/** stops any looping fire effects */
	UFUNCTION(BlueprintCallable, Category = "Weapon")
	virtual void StopFiringEffects(bool bIgnoreCurrentMode = false);

	/** read WeaponOverlayFlags from owner and apply the appropriate overlay material (if any) */
	virtual void UpdateOverlays();

	/** set main skin override for the weapon, NULL to restore to default */
	virtual void SetSkin(UMaterialInterface* NewSkin);

#if WITH_EDITORONLY_DATA
	virtual void PostDuplicate(bool bDuplicateForPIE) override;
	void AttachmentBPChanged(UBlueprint* BP);
#endif
};