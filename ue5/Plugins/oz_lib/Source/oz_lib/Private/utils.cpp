

uint8* ReadFileContent(FString filePath, uint32 *ret_size)
{
	// 验证文件存在性
	if (!FPaths::FileExists(filePath))
	{
		UE_LOG(LogTemp, Warning, TEXT("read content file %s not found"), *filePath)
		return NULL;
	}

	TArray<uint8> FileRawContent;
	if (!FFileHelper::LoadFileToArray(FileRawContent, *filePath))
	{
		UE_LOG(LogTemp, Warning, TEXT("read content file %s to array failed"), *filePath)
		return NULL;
	}

	const uint8* DataPtr = FileRawContent.GetData();

	*ret_size = static_cast<uint32_t>(FileRawContent.Num());

	FString HexHeader = BytesToHex(DataPtr, FMath::Min(32, FileRawContent.Num()));
	
	uint8_t* newDataPtr = new uint8_t[*ret_size];
	memcpy(newDataPtr, DataPtr, static_cast<size_t>(*ret_size) * sizeof(uint8_t));
	return newDataPtr;
}