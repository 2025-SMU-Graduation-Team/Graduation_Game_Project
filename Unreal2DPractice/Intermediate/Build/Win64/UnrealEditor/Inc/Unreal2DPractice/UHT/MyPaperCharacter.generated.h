// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

// IWYU pragma: private, include "MyPaperCharacter.h"

#ifdef UNREAL2DPRACTICE_MyPaperCharacter_generated_h
#error "MyPaperCharacter.generated.h already included, missing '#pragma once' in MyPaperCharacter.h"
#endif
#define UNREAL2DPRACTICE_MyPaperCharacter_generated_h

#include "UObject/ObjectMacros.h"
#include "UObject/ScriptMacros.h"

PRAGMA_DISABLE_DEPRECATION_WARNINGS

struct FInputActionValue;

// ********** Begin Class AMyPaperCharacter ********************************************************
#define FID_Users_user_Documents_Unreal_Projects_Unreal2DPractice_Source_Unreal2DPractice_MyPaperCharacter_h_17_RPC_WRAPPERS_NO_PURE_DECLS \
	DECLARE_FUNCTION(execStopJump); \
	DECLARE_FUNCTION(execStartJump); \
	DECLARE_FUNCTION(execMove);


UNREAL2DPRACTICE_API UClass* Z_Construct_UClass_AMyPaperCharacter_NoRegister();

#define FID_Users_user_Documents_Unreal_Projects_Unreal2DPractice_Source_Unreal2DPractice_MyPaperCharacter_h_17_INCLASS_NO_PURE_DECLS \
private: \
	static void StaticRegisterNativesAMyPaperCharacter(); \
	friend struct Z_Construct_UClass_AMyPaperCharacter_Statics; \
	static UClass* GetPrivateStaticClass(); \
	friend UNREAL2DPRACTICE_API UClass* Z_Construct_UClass_AMyPaperCharacter_NoRegister(); \
public: \
	DECLARE_CLASS2(AMyPaperCharacter, APaperCharacter, COMPILED_IN_FLAGS(0 | CLASS_Config), CASTCLASS_None, TEXT("/Script/Unreal2DPractice"), Z_Construct_UClass_AMyPaperCharacter_NoRegister) \
	DECLARE_SERIALIZER(AMyPaperCharacter)


#define FID_Users_user_Documents_Unreal_Projects_Unreal2DPractice_Source_Unreal2DPractice_MyPaperCharacter_h_17_ENHANCED_CONSTRUCTORS \
	/** Deleted move- and copy-constructors, should never be used */ \
	AMyPaperCharacter(AMyPaperCharacter&&) = delete; \
	AMyPaperCharacter(const AMyPaperCharacter&) = delete; \
	DECLARE_VTABLE_PTR_HELPER_CTOR(NO_API, AMyPaperCharacter); \
	DEFINE_VTABLE_PTR_HELPER_CTOR_CALLER(AMyPaperCharacter); \
	DEFINE_DEFAULT_CONSTRUCTOR_CALL(AMyPaperCharacter) \
	NO_API virtual ~AMyPaperCharacter();


#define FID_Users_user_Documents_Unreal_Projects_Unreal2DPractice_Source_Unreal2DPractice_MyPaperCharacter_h_14_PROLOG
#define FID_Users_user_Documents_Unreal_Projects_Unreal2DPractice_Source_Unreal2DPractice_MyPaperCharacter_h_17_GENERATED_BODY \
PRAGMA_DISABLE_DEPRECATION_WARNINGS \
public: \
	FID_Users_user_Documents_Unreal_Projects_Unreal2DPractice_Source_Unreal2DPractice_MyPaperCharacter_h_17_RPC_WRAPPERS_NO_PURE_DECLS \
	FID_Users_user_Documents_Unreal_Projects_Unreal2DPractice_Source_Unreal2DPractice_MyPaperCharacter_h_17_INCLASS_NO_PURE_DECLS \
	FID_Users_user_Documents_Unreal_Projects_Unreal2DPractice_Source_Unreal2DPractice_MyPaperCharacter_h_17_ENHANCED_CONSTRUCTORS \
private: \
PRAGMA_ENABLE_DEPRECATION_WARNINGS


class AMyPaperCharacter;

// ********** End Class AMyPaperCharacter **********************************************************

#undef CURRENT_FILE_ID
#define CURRENT_FILE_ID FID_Users_user_Documents_Unreal_Projects_Unreal2DPractice_Source_Unreal2DPractice_MyPaperCharacter_h

PRAGMA_ENABLE_DEPRECATION_WARNINGS
