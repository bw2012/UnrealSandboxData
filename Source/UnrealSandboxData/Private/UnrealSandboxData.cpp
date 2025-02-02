// Copyright Epic Games, Inc. All Rights Reserved.

#include "UnrealSandboxData.h"
#include "kvdb.hpp"

#define LOCTEXT_NAMESPACE "FUnrealSandboxDataModule"

void FUnrealSandboxDataModule::StartupModule() {

}

void FUnrealSandboxDataModule::ShutdownModule() {

}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FUnrealSandboxDataModule, UnrealSandboxData)



struct TKvdbInternal {

	std::shared_ptr<kvdb::KvRawFile> raw_file_ptr;

};

int32 NextFileId = 1;

TMap<int32, TKvdbInternal> KvdbGlobal;



int32 KvdbOpen(std::string file) {

	std::shared_ptr<kvdb::KvRawFile> raw_file_ptr(new kvdb::KvRawFile(512));

	if (raw_file_ptr->open(file) != KVDB_OK) {
		return -1;
	}

	auto Res = NextFileId;

	KvdbGlobal.Add(NextFileId, { raw_file_ptr });

	NextFileId++;

	return Res;
}

void KvdbCreate(std::string file, uint32 max_key_size) {
	kvdb::KvRawFile::create_empty(file, max_key_size);
}

void KvdbClose(int32 file_id) {
	if (file_id > 0 && KvdbGlobal.Contains(file_id)) {
		KvdbGlobal[file_id].raw_file_ptr->close();
	}
}

bool KvdbHasKey(int32 file_id, const std::vector<unsigned char>& key) {
	if (file_id > 0 && KvdbGlobal.Contains(file_id)) {
		return KvdbGlobal[file_id].raw_file_ptr->isExist(key);
	}

	return false;
}

std::shared_ptr<std::vector<uint8>> KvdbLoadData(int32 file_id, const std::vector<unsigned char>& key) {
	if (file_id > 0 && KvdbGlobal.Contains(file_id)) {
		return KvdbGlobal[file_id].raw_file_ptr->loadData(key);
	}

	return nullptr;
}

void KvdbSaveData(int32 file_id, const std::vector<unsigned char>& key, const std::vector<unsigned char>& data, uint64 flags) {
	if (file_id > 0 && KvdbGlobal.Contains(file_id)) {
		KvdbGlobal[file_id].raw_file_ptr->save(key, data, flags);
	}
}

uint64 KvdbGetKeyFlags(int32 file_id, const std::vector<unsigned char>& key) {
	if (file_id > 0 && KvdbGlobal.Contains(file_id)) {
		return KvdbGlobal[file_id].raw_file_ptr->k_flags(key);
	}

	return 0;
}

char seq[] = "0123456789abcdefghijklmnopqrstuvwxyz";

std::string base36_encode(uint64_t in) {
	std::string result;

	while (in != 0) {
		result.push_back(seq[in % 36]);
		in /= 36;
	}

	std::reverse(result.begin(), result.end());
	return result;
}

FString TSandboxData::EncodeBase36(uint64 Val) {
	return FString(base36_encode(Val).c_str());
}


uint64 TSandboxData::DecodeBase36(FString Str) {
	const char* r = TCHAR_TO_ANSI(*Str);
	char* end = nullptr;
	return std::strtoull(r, &end, 36);
}