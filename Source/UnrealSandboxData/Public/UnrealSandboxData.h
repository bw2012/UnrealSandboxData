// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"
#include <vector>
#include <memory>
#include <string>

class FUnrealSandboxDataModule : public IModuleInterface {
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};



UNREALSANDBOXDATA_API int32 KvdbOpen(std::string File);

UNREALSANDBOXDATA_API void KvdbCreate(std::string file, uint32 max_key_size);

UNREALSANDBOXDATA_API void KvdbClose(int32 file_id);

UNREALSANDBOXDATA_API bool KvdbHasKey(int32 file_id, const std::vector<unsigned char>& key);

UNREALSANDBOXDATA_API std::shared_ptr<std::vector<uint8>> KvdbLoadData(int32 file_id, const std::vector<unsigned char>& key);

UNREALSANDBOXDATA_API void KvdbSaveData(int32 file_id, const std::vector<unsigned char>& key, const std::vector<unsigned char>& data, uint64 flags);

UNREALSANDBOXDATA_API uint64 KvdbGetKeyFlags(int32 file_id, const std::vector<unsigned char>& key);


#define TO_BYTES(P) std::vector<uint8> ba(sizeof(P)); std::fill(ba.begin(), ba.end(), 0); std::memcpy(ba.data(), &P, sizeof(P)); 

class UNREALSANDBOXDATA_API FKvdb {

public:

	template<typename K>
	static int32 OpenOrCreateFile(const FString& FullPath) {		
		std::string Str(TCHAR_TO_UTF8(*FullPath));

		if (!FPlatformFileManager::Get().GetPlatformFile().FileExists(*FullPath)) {
			KvdbCreate(Str, (uint32)sizeof(K)); // TODO check bounds
		}

		return KvdbOpen(Str);
	}

	static void Close(int32 Fid) {
		KvdbClose(Fid);
	}

	template<typename K>
	static bool HasKey(int32 Fid, K Key) {
		TO_BYTES(Key)
		return KvdbHasKey(Fid, ba);
	}

	template<typename K>
	static std::shared_ptr<std::vector<uint8>> LoadData(int32 Fid, K Key) {
		TO_BYTES(Key)
		return KvdbLoadData(Fid, ba);
	}

	template<typename K>
	static void SaveData(int32 Fid, K Key, const std::vector<uint8>& Data, uint64 KeyFlags) {
		TO_BYTES(Key)
		KvdbSaveData(Fid, ba, Data, KeyFlags);
	}

	template<typename K>
	static uint64 GetKeyFlags(int32 Fid, K Key) {
		TO_BYTES(Key)
		return KvdbGetKeyFlags(Fid, ba);
	}

};


class UNREALSANDBOXDATA_API TSandboxData {

public:

	static FString EncodeBase36(uint64 Val);

	static uint64 DecodeBase36(FString Str);

};
