// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InputDataWrapper.h"
#include "RenderDataWrapper.h"
#include "render_data_generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogGP, Log, All);


/**
 * 
 */
class GPWrapper
{
public:
	static void LoadDLL();
	static void UnLoadDLL();
	static void Start();
	static const GPRenderData* Update(const flatbuffers::FlatBufferBuilder& input_builder);
private:
	inline static void* gp_dll_handle = nullptr;
	inline static void (*gp_start)() = nullptr;
	inline static const uint8* (*gp_update)(const uint8*, uint32) = nullptr;
};