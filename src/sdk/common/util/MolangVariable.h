#pragma once
#include "sdk/deps/Core/StringUtils.h"

namespace SDK {
	using MolangScriptArgType = int32_t;

	struct MolangLoopBreak {};
	struct MolangLoopContinue {};

	union MolangScriptArgPOD {
		float mFloat;
		uint64_t mHashType64;
		MolangLoopBreak mLoopBreak;
		MolangLoopContinue mLoopContinue;
		class Actor* mActorPtr;
		int64_t mActorId;
		class ItemStackBase* mItemStackBasePtr;
		uint64_t _mData;
	};

	struct MolangScriptArg {
		MolangScriptArgType mType;
		MolangScriptArgPOD mPOD;
		//std::variant<MolangMatrix, MaterialVariants, MolangActorArrayPtr, MolangActorIdArrayPtr, MolangArrayVariable, MolangClientTexture, MolangContextVariable, MolangDataDrivenGeometry, MolangEntityVariable, MolangGeometryVariable, MolangMaterialVariable, MolangMemberAccessor, MolangMemberArray, MolangQueryFunctionPtr, MolangTempVariable, MolangTextureVariable> mData;
	};

	struct MolangVariableSettings {
		std::byte index[2];
		int mAccessSpecifier;
		int mShouldSync;
		int mShouldSave;
	};

	struct MolangVariable {
		HashedString mName;
		MolangScriptArg mValue;
		MolangScriptArg mPublicValue;
		MolangVariableSettings mSettings;
	};

}