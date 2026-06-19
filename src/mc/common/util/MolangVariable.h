#pragma once
#include "mc/deps/core/StringUtils.h"
#include <cstddef>

namespace SDK {
    enum class MolangScriptArgType : int {
        Unset                  = -1,
        Float                  = 0,
        HashType64             = 1,
        MolangLoopBreak        = 2,
        MolangLoopContinue     = 3,
        MolangActorPtr         = 4,
        MolangActorIdPtr       = 5,
        MolangItemStackBasePtr = 6,
        Variant                = 7,
    };

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
		std::byte mData[0x50];
	};

	struct MolangVariableSettings {
		std::byte index[2];
		int mAccessSpecifier;
		int mShouldSync;
		int mShouldSave;
	};

	struct MolangVariable {
		MolangScriptArg mValue;
		std::unique_ptr<MolangScriptArg> mPublicValue;
		MolangVariableSettings mSettings;

	    static uint16_t _findOrAddVariableIndex(uint64_t hash, const char* name, bool allowSpecialCharacters);
	};

}
