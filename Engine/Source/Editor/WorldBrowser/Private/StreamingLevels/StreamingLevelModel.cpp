// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "WorldBrowserPrivatePCH.h"

#include "StreamingLevelCollectionModel.h"
#include "StreamingLevelModel.h"

#define LOCTEXT_NAMESPACE "WorldBrowser"

FStreamingLevelModel::FStreamingLevelModel(const TWeakObjectPtr<UEditorEngine>& InEditor, 
											FStreamingLevelCollectionModel& InWorldData, 
											class ULevelStreaming* InLevelStreaming)

	: FLevelModel(InWorldData, InEditor)
	, LevelStreaming(InLevelStreaming)
	, bHasValidPackageName(false)
{
	Editor->RegisterForUndo( this );
}

FStreamingLevelModel::~FStreamingLevelModel()
{
	Editor->UnregisterForUndo( this );
}

bool FStreamingLevelModel::HasValidPackage() const
{
	return bHasValidPackageName;
}

UObject* FStreamingLevelModel::GetNodeObject()
{
	return LevelStreaming.Get();
}

ULevel* FStreamingLevelModel::GetLevelObject() const
{
	ULevelStreaming* StreamingObj = LevelStreaming.Get();
	
	if (StreamingObj)
	{
		return StreamingObj->GetLoadedLevel();
	}
	else // persistent level does not have associated level streaming object
	{
		return LevelCollectionModel.GetWorld()->PersistentLevel;
	}
}

FName FStreamingLevelModel::GetAssetName() const
{
	return NAME_None;
}

FName FStreamingLevelModel::GetLongPackageName() const
{
	if (LevelStreaming.IsValid())
	{
		return LevelStreaming->PackageName;
	}
	else
	{
		return LevelCollectionModel.GetWorld()->PersistentLevel->GetOutermost()->GetFName();
	}
}

void FStreamingLevelModel::Update()
{
	UpdatePackageFileAvailability();
	FLevelModel::Update();
}

void FStreamingLevelModel::OnDrop(const TSharedPtr<FLevelDragDropOp>& Op)
{
	TArray<ULevelStreaming*> DropStreamingLevels;
	
	for (auto It = Op->StreamingLevelsToDrop.CreateConstIterator(); It; ++It)
	{
		if ((*It).IsValid())
		{
			DropStreamingLevels.AddUnique((*It).Get());
		}
	}	
	
	// Prevent dropping items on itself
	if (DropStreamingLevels.Num() && DropStreamingLevels.Find(LevelStreaming.Get()) == INDEX_NONE)
	{
		UWorld* CurrentWorld = LevelCollectionModel.GetWorld();
		auto& WorldStreamingLevels = CurrentWorld->StreamingLevels;
		// Remove streaming level objects from a world streaming levels list
		for (auto It : DropStreamingLevels)
		{
			WorldStreamingLevels.Remove(It);
		}
		
		// Find a new place where to insert the in a world streaming levels list
		// Right after the current level, or at start of the list in case if this is persistent level
		int32 InsertIndex = WorldStreamingLevels.Find(LevelStreaming.Get());
		if (InsertIndex == INDEX_NONE)
		{
			InsertIndex = 0;
		}
		else
		{
			InsertIndex++;
		}

		WorldStreamingLevels.Insert(DropStreamingLevels, InsertIndex);
		CurrentWorld->MarkPackageDirty();
			
		// Force levels list refresh
		LevelCollectionModel.PopulateLevelsList();
	}
}

bool FStreamingLevelModel::IsGoodToDrop(const TSharedPtr<FLevelDragDropOp>& Op) const
{
	return true;
}

//bool FStreamingLevelModel::IsReadOnly() const
//{
//	ULevel* Level = GetLevelObject();
//
//	const UPackage* pPackage = Cast<UPackage>( Level->GetOutermost() );
//	if ( pPackage )
//	{
//		FString PackageFileName;
//		if ( FPackageName::DoesPackageExist( pPackage->GetName(), NULL, &PackageFileName ) )
//		{
//			return IFileManager::Get().IsReadOnly( *PackageFileName );
//		}
//	}
//
//	return false;
//}

UClass* FStreamingLevelModel::GetStreamingClass() const
{
	if (!IsPersistent() && LevelStreaming.IsValid())
	{
		return LevelStreaming.Get()->GetClass();
	}
	
	return FLevelModel::GetStreamingClass();
}

void FStreamingLevelModel::SetStreamingClass( UClass *LevelStreamingClass )
{
	if (IsPersistent())
	{
		// Invalid operations for the persistent level
		return;
	}

	ULevelStreaming* StreamingLevel = LevelStreaming.Get();
	if (StreamingLevel)
	{
		StreamingLevel = EditorLevelUtils::SetStreamingClassForLevel( StreamingLevel, LevelStreamingClass );
	}

	Update();
}

const TWeakObjectPtr< ULevelStreaming > FStreamingLevelModel::GetLevelStreaming()
{
	return LevelStreaming;
}

void FStreamingLevelModel::UpdatePackageFileAvailability()
{
	 // Check if streaming level has a valid package name
	if (!GetLevelObject())
	{
		if (LevelStreaming.IsValid())
		{
			FString PackageName = LevelStreaming->PackageNameToLoad == NAME_None ? 
										LevelStreaming->PackageName.ToString() : 
										LevelStreaming->PackageNameToLoad.ToString();
			
			bHasValidPackageName = FPackageName::DoesPackageExist(PackageName);
		}
		else
		{
			bHasValidPackageName = false;
		}
	}
	else
	{
		bHasValidPackageName = true;
	}
}

#undef LOCTEXT_NAMESPACE
