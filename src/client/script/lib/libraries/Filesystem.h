/* Copyright (C) Imrglop, All rights reserved
 * Copyright (C) Latite Client contributors, All rights reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited without the permission of the author.
 * Proprietary and confidential
 * Written by Imrglop <contact: latiteclientgithub@gmail.com>, 2023
 */

#pragma once
#include "../JsLibrary.h"
#include <functional>
#include <thread>
#include "../../JsScript.h"
#include <winrt/windows.storage.streams.h>

class Filesystem : public JsLibrary {
public:
	JsValueRef initialize(JsValueRef parent) override;
	Filesystem(JsScript* owner) : JsLibrary(owner, L"filesystem") {}
private:


	//template <typename T>
	//void runAsyncOperation(std::function<void(T)> func)

	// can someone tell past me about std::vector....
	/*
	struct BufferRef {
		BYTE* data;
		size_t size;

		BufferRef(size_t sz) : size(sz), data(nullptr) {}

		void alloc(size_t sz) {
			if (data) delete[] data;
			data = new BYTE[sz];
			size = sz;
		}

		~BufferRef() {
			if (data) {
				delete[] data;
			}
		}
	};*/

	class FSAsyncOperation : public JsScript::AsyncOperation {
	public:
		

		std::wstring path = {};

		int err = 0;
		std::optional<std::vector<BYTE>> data = std::nullopt;
		bool outData = true;

		virtual void getArgs() override;

		FSAsyncOperation(JsValueRef callback, decltype(initFunc) initFunc, void* param)
			: JsScript::AsyncOperation(true, callback, initFunc, param), err(0), data(std::nullopt)
		{
		}
	};

	std::wstring getPath(std::wstring relPath);

	static JsValueRef CALLBACK write(JsValueRef callee, bool isConstructor,
		JsValueRef* arguments, unsigned short argCount,
		void* callbackState);
	static JsValueRef CALLBACK read(JsValueRef callee, bool isConstructor,
		JsValueRef* arguments, unsigned short argCount,
		void* callbackState);
	static JsValueRef CALLBACK writeSync(JsValueRef callee, bool isConstructor,
		JsValueRef* arguments, unsigned short argCount,
		void* callbackState);
	static JsValueRef CALLBACK readSync(JsValueRef callee, bool isConstructor,
		JsValueRef* arguments, unsigned short argCount,
		void* callbackState);
	static JsValueRef CALLBACK existsSync(JsValueRef callee, bool isConstructor,
		JsValueRef* arguments, unsigned short argCount,
		void* callbackState);
	static JsValueRef CALLBACK createDirectorySync(JsValueRef callee, bool isConstructor,
		JsValueRef* arguments, unsigned short argCount,
		void* callbackState);
};