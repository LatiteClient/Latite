#include "pch.h"
#include "Filesystem.h"
#include <filesystem>
#include <fstream>
#include "../JsScript.h"
#include "util/ChakraUtil.h"

namespace fs = std::filesystem;

JsValueRef Filesystem::initialize(JsValueRef parent) {
	JsValueRef ret;
	JS::JsCreateObject(&ret);

	Chakra::DefineFunc(ret, write, L"writeAsync", this);
	Chakra::DefineFunc(ret, writeSync, L"write", this);
	Chakra::DefineFunc(ret, read, L"readAsync", this);
	Chakra::DefineFunc(ret, readSync, L"read", this);
	Chakra::DefineFunc(ret, existsSync, L"exists", this);
	Chakra::DefineFunc(ret, createDirectorySync, L"createDirectory", this);
	Chakra::DefineFunc(ret, appendSync, L"append", this);
	Chakra::DefineFunc(ret, deleteFile, L"delete", this);
	Chakra::DefineFunc(ret, readdirSync, L"readDirectory", this);
	Chakra::DefineFunc(ret, moveSync, L"move", this);
	return ret;
}

std::wstring Filesystem::getPath(std::wstring relPath) {
	try {
		if (fs::exists(relPath)) {
			return relPath;
		}
	}
	catch (...) {}
	return owner->getFolderPath() / relPath;
}

namespace {
	void throwFsError() {
		char st[100]; // idfk
		strerror_s(st, sizeof(st), errno);
		std::wstring wstr = util::StrToWStr(st);
		Chakra::ThrowError(L"Filesystem error code " + std::to_wstring(errno) + L": " + wstr);
	}
}

JsValueRef Filesystem::write(JsValueRef callee, bool isConstructor, JsValueRef* arguments, unsigned short argCount, void* callbackState) {
	auto undef = Chakra::GetUndefined();
	if (!Chakra::VerifyArgCount(argCount, 4)) return undef;
	if (!Chakra::VerifyParameters({ {arguments[1], JsString}, {arguments[2], JsTypedArray}, {arguments[3], JsFunction}})) return undef;

	auto thi = reinterpret_cast<Filesystem*>(callbackState);
	auto op = std::make_shared<FSAsyncOperation>(arguments[3], [](JsScript::AsyncOperation* op_) {
		auto op = reinterpret_cast<FSAsyncOperation*>(op_);
		auto& arguments = op->params;
		auto thi = reinterpret_cast<Filesystem*>(op->param);
		std::wofstream ofs;
		ofs.open(op->path, std::ios::out | std::ios::binary);
		int errn = 0;
		if (ofs.fail()) {
			errn = errno;
		}
		else {
			for (size_t i = 0; i < op->data->size(); i++) {
				ofs << (char)op->data->data()[i];
			}
		}
		ofs.close();
		ofs.flush();
		op->err = errn;
		op->flagDone = true;
		}, thi);

	op->path = thi->getPath(Chakra::GetString(arguments[1]));
	op->data = std::vector<BYTE>();

	BYTE* buf;
	unsigned int bufSize;
	JS::JsGetTypedArrayStorage(arguments[2], &buf, &bufSize, nullptr, nullptr);

	for (size_t i = 0; i < bufSize; i++) {
		op->data->push_back(buf[i]);
	}

	op->outData = false;

	op->run();

	thi->owner->pendingOperations.push_back(op);
	return undef;
}

JsValueRef Filesystem::read(JsValueRef callee, bool isConstructor, JsValueRef* arguments, unsigned short argCount, void* callbackState) {
	auto undef = Chakra::GetUndefined();
	if (!Chakra::VerifyArgCount(argCount, 3)) return undef;
	if (!Chakra::VerifyParameters({ {arguments[1], JsString}, {arguments[2], JsFunction} })) return undef;

	auto thi = reinterpret_cast<Filesystem*>(callbackState);
	auto path = thi->getPath(Chakra::GetString(arguments[1]));
	if (fs::is_directory(path)) {
		Chakra::ThrowError(L"Filesystem read: File is a directory");
		return JS_INVALID_REFERENCE;
	}

	auto op = std::make_shared<FSAsyncOperation>(arguments[2], [](JsScript::AsyncOperation* op_) {
		auto op = reinterpret_cast<FSAsyncOperation*>(op_);
		std::ifstream ifs;
		std::wstringstream wss;
		ifs.open(op->path, std::ios::binary | std::ios::ate);
		int errn = 0;
		if (ifs.fail()) {
			errn = errno;
		}
		else {
			auto size = ifs.tellg();
			ifs.seekg(0, std::ios::beg);

			op->data->resize(size);
			ifs.read((char*)op->data->data(), size);
		}
		ifs.close();
		op->err = errn;
		op->flagDone = true;
		}, thi);

	op->path = path;
	op->data = std::vector<uint8_t>();
	op->run();

	thi->owner->pendingOperations.push_back(op);
	return undef;
}


JsValueRef Filesystem::writeSync(JsValueRef callee, bool isConstructor, JsValueRef* arguments, unsigned short argCount, void* callbackState) {
	auto undef = Chakra::GetUndefined();
	if (!Chakra::VerifyArgCount(argCount, 3)) return undef;
	if (!Chakra::VerifyParameters({ {arguments[1], JsString}, {arguments[2], JsTypedArray} })) return undef;

	auto thi = reinterpret_cast<Filesystem*>(callbackState);

	std::wofstream ofs;
	ofs.open(thi->getPath(Chakra::GetString(arguments[1])), std::ios::binary | std::ios::out);
	if (ofs.fail()) {
		throwFsError();
		return undef;
	}
	else {
		BYTE* buf;
		unsigned int bufSize;
		JS::JsGetTypedArrayStorage(arguments[2], &buf, &bufSize, nullptr, nullptr);

		for (size_t i = 0; i < bufSize; i++) {
			ofs << (char)buf[i];
		}
	}
	ofs.close();
	ofs.flush();
	return undef;
}

JsValueRef Filesystem::readSync(JsValueRef callee, bool isConstructor, JsValueRef* arguments, unsigned short argCount, void* callbackState) {
	auto ret = Chakra::GetUndefined();
	if (!Chakra::VerifyArgCount(argCount, 2)) return ret;
	if (!Chakra::VerifyParameters({ {arguments[1], JsString} })) return ret;
	auto thi = reinterpret_cast<Filesystem*>(callbackState);
	auto path = thi->getPath(Chakra::GetString(arguments[1]));
	if (fs::is_directory(path)) {
		Chakra::ThrowError(L"Filesystem readSync: File is a directory");
		return JS_INVALID_REFERENCE;
	}

	std::ifstream ifs;

	ifs.open(path, std::ios::binary | std::ios::ate);

	if (ifs.fail()) {
		throwFsError();
		return ret;
	}
	else {
		auto size = ifs.tellg();
		ifs.seekg(0, std::ios::beg);
		auto realSize = static_cast<unsigned>(size);
		JS::JsCreateTypedArray(JsArrayTypeUint8, JS_INVALID_REFERENCE, 0, static_cast<unsigned>(realSize), &ret);
		BYTE* buf;
		unsigned int sz;
		JsValueRef arrayBuffer;

		JS::JsGetTypedArrayInfo(ret, nullptr, &arrayBuffer, nullptr, nullptr);
		JS::JsGetArrayBufferStorage(arrayBuffer, &buf, &sz);
		ifs.read((char*)buf, sz);
	}
	ifs.close();
	return ret;
}


JsValueRef Filesystem::existsSync(JsValueRef callee, bool isConstructor, JsValueRef* arguments, unsigned short argCount, void* callbackState) {
	auto ret = Chakra::GetUndefined();
	if (!Chakra::VerifyArgCount(argCount, 2)) return ret;
	if (!Chakra::VerifyParameters({ {arguments[1], JsString} })) return ret;
	std::wifstream ifs;
	std::wstringstream wss;
	auto thi = reinterpret_cast<Filesystem*>(callbackState);

	return fs::exists(thi->getPath(Chakra::GetString(arguments[1]))) ? Chakra::GetTrue() : Chakra::GetFalse();
}

JsValueRef Filesystem::createDirectorySync(JsValueRef callee, bool isConstructor, JsValueRef* arguments, unsigned short argCount, void* callbackState) {
	auto ret = Chakra::GetUndefined();
	if (!Chakra::VerifyArgCount(argCount, 2)) return ret;
	if (!Chakra::VerifyParameters({ {arguments[1], JsString} })) return ret;

	auto thi = reinterpret_cast<Filesystem*>(callbackState);
	std::error_code errCode;
	if (!fs::create_directory(thi->getPath(Chakra::GetString(arguments[1])), errCode)) {
		if (errCode.value() != ERROR_ALREADY_EXISTS && errCode.value() != 0) {
			Chakra::ThrowError(L"Filesystem error: " + util::StrToWStr(errCode.message()));
		}
		return ret;
	}
	return ret;
}

JsValueRef Filesystem::appendSync(JsValueRef callee, bool isConstructor, JsValueRef* arguments, unsigned short argCount, void* callbackState) {
	auto undef = Chakra::GetUndefined();
	if (!Chakra::VerifyArgCount(argCount, 3)) return undef;
	if (!Chakra::VerifyParameters({ {arguments[1], JsString}, {arguments[2], JsTypedArray} })) return undef;

	auto thi = reinterpret_cast<Filesystem*>(callbackState);

	std::wofstream ofs;
	ofs.open(thi->getPath(Chakra::GetString(arguments[1])), std::ios::app | std::ios::binary);
	if (ofs.fail()) {
		throwFsError();
		return undef;
	}
	else {
		BYTE* buf;
		unsigned int bufSize;
		JS::JsGetTypedArrayStorage(arguments[2], &buf, &bufSize, nullptr, nullptr);

		for (size_t i = 0; i < bufSize; i++) {
			ofs << (char)buf[i];
		}
	}
	ofs.close();
	return undef;
}

JsValueRef Filesystem::deleteFile(JsValueRef callee, bool isConstructor, JsValueRef* arguments, unsigned short argCount, void* callbackState) {
	auto ret = Chakra::GetUndefined();
	if (!Chakra::VerifyArgCount(argCount, 2)) return ret;
	if (!Chakra::VerifyParameters({ {arguments[1], JsString} })) return ret;
	std::wifstream ifs;
	std::wstringstream wss;
	auto thi = reinterpret_cast<Filesystem*>(callbackState);

	auto path = thi->getPath(Chakra::GetString(arguments[1]));
	if (fs::exists(path)) {
		fs::remove(path);
	}
	else {
		errno = ENOENT;
		throwFsError();
		return JS_INVALID_REFERENCE;
	}
	return Chakra::GetUndefined();
}

JsValueRef Filesystem::readdirSync(JsValueRef callee, bool isConstructor, JsValueRef* arguments, unsigned short argCount, void* callbackState) {
	auto ret = Chakra::GetUndefined();
	if (!Chakra::VerifyArgCount(argCount, 2)) return ret;
	if (!Chakra::VerifyParameters({ {arguments[1], JsString} })) return ret;
	auto thi = reinterpret_cast<Filesystem*>(callbackState);

	auto path = thi->getPath(Chakra::GetString(arguments[1]));
	if (!fs::is_directory(path)) {
		Chakra::ThrowError(L"Filesystem error: " + path + L": Not a directory");
		return JS_INVALID_REFERENCE;
	}

	std::vector<std::wstring> entries;

	for (auto& entry : fs::directory_iterator(path)) {
		entries.push_back(entry.path().filename());
	}

	JsValueRef array;
	JS::JsCreateArray(entries.size(), &array);

	for (size_t i = 0; auto & entry : entries) {
		JS::JsSetIndexedProperty(array, Chakra::MakeInt(i), Chakra::MakeString(entry));
		i++;
	}

	return array;
}

JsValueRef Filesystem::moveSync(JsValueRef callee, bool isConstructor, JsValueRef* arguments, unsigned short argCount, void* callbackState) {
	auto ret = Chakra::GetUndefined();
	if (!Chakra::VerifyArgCount(argCount, 3)) return ret;
	if (!Chakra::VerifyParameters({ {arguments[1], JsString}, {arguments[1], JsString} })) return ret;
	std::wifstream ifs;
	std::wstringstream wss;
	auto thi = reinterpret_cast<Filesystem*>(callbackState);

	auto path1 = thi->getPath(Chakra::GetString(arguments[1]));
	auto path2 = thi->getPath(Chakra::GetString(arguments[1]));

	std::error_code errCode;
	fs::rename(path1, path2, errCode);

	if (errCode.value() != ERROR_ALREADY_EXISTS && errCode.value() != 0) {
		Chakra::ThrowError(L"Filesystem error: " + util::StrToWStr(errCode.message()));
		return JS_INVALID_REFERENCE;
	}

	return Chakra::GetUndefined();
}

void Filesystem::FSAsyncOperation::getArgs() {
	JsValueRef err;
	JS::JsIntToNumber(this->err, &err);
	this->args.push_back(err);
	if (data.has_value() && outData) {
		JsValueRef jData;

		JS::JsCreateTypedArray(JsArrayTypeUint8, JS_INVALID_REFERENCE, 0, static_cast<unsigned int>(data->size()), &jData);
		BYTE* storage;
		unsigned int bufLen;
		JsTypedArrayType at;
		int elemSize;
		JS::JsGetTypedArrayStorage(jData, &storage, &bufLen, &at, &elemSize);

		for (size_t i = 0; i < data->size(); ++i) {
			storage[i] = data->at(i);
		}
		
		this->args.push_back(jData);
	}
}
