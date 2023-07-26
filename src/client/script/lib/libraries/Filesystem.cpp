#include "Filesystem.h"
#include <filesystem>
#include <fstream>
#include "../../JsScript.h"
#include "util/ChakraUtil.h"

JsValueRef Filesystem::initialize(JsValueRef parent) {
	JsValueRef ret;
	JS::JsCreateObject(&ret);

	Chakra::DefineFunc(ret, write, L"write", this);
	Chakra::DefineFunc(ret, writeSync, L"writeSync", this);
	Chakra::DefineFunc(ret, read, L"read", this);
	Chakra::DefineFunc(ret, readSync, L"readSync", this);
	Chakra::DefineFunc(ret, existsSync, L"existsSync", this);
	Chakra::DefineFunc(ret, createDirectorySync, L"createDirectorySync", this);
	return ret;
}

std::wstring Filesystem::getPath(std::wstring relPath) {
	try {
		if (std::filesystem::exists(relPath)) {
			return relPath;
		}
	}
	catch (...) {}
	return util::GetLatitePath() / "Scripts" / owner->relFolderPath / relPath;
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
	if (!Chakra::VerifyParameters({ {arguments[1], JsString}, {arguments[2], JsString}, {arguments[3], JsFunction}})) return undef;

	auto thi = reinterpret_cast<Filesystem*>(callbackState);
	auto op = std::make_shared<FSAsyncOperation>(arguments[3], [](JsScript::AsyncOperation* op_) {
		auto op = reinterpret_cast<FSAsyncOperation*>(op_);
		auto& arguments = op->params;
		auto thi = reinterpret_cast<Filesystem*>(op->param);
		std::wofstream ofs{op->path};
		int errn = 0;
		if (ofs.fail()) {
			errn = errno;
		}
		else {
			ofs << op->data.value();
		}
		ofs.close();
		op->err = errn;
		op->flagDone = true;
		}, thi);

	op->path = thi->getPath(Chakra::GetString(arguments[1]));
	op->run();

	thi->owner->pendingOperations.push_back(op);
	return undef;
}

JsValueRef Filesystem::read(JsValueRef callee, bool isConstructor, JsValueRef* arguments, unsigned short argCount, void* callbackState) {
	auto undef = Chakra::GetUndefined();
	if (!Chakra::VerifyArgCount(argCount, 3)) return undef;
	if (!Chakra::VerifyParameters({ {arguments[1], JsString}, {arguments[2], JsFunction} })) return undef;

	auto thi = reinterpret_cast<Filesystem*>(callbackState);
	auto op = std::make_shared<FSAsyncOperation>(arguments[2], [](JsScript::AsyncOperation* op_) {
		auto op = reinterpret_cast<FSAsyncOperation*>(op_);
		std::wifstream ifs;
		std::wstringstream wss;
		ifs.open(op->path);
		int errn = 0;
		if (ifs.fail()) {
			errn = errno;
		}
		else {
			wss << ifs.rdbuf();
		}
		ifs.close();
		op->err = errn;
		op->data = wss.str();
		op->flagDone = true;
		}, thi);

	op->path = thi->getPath(Chakra::GetString(arguments[1]));

	op->run();

	thi->owner->pendingOperations.push_back(op);
	return undef;
}


JsValueRef Filesystem::writeSync(JsValueRef callee, bool isConstructor, JsValueRef* arguments, unsigned short argCount, void* callbackState) {
	auto undef = Chakra::GetUndefined();
	if (!Chakra::VerifyArgCount(argCount, 3)) return undef;
	if (!Chakra::VerifyParameters({ {arguments[1], JsString}, {arguments[2], JsString} })) return undef;

	auto thi = reinterpret_cast<Filesystem*>(callbackState);

	std::wofstream ofs;
	ofs.open(thi->getPath(Chakra::GetString(arguments[1])));
	if (ofs.fail()) {
		throwFsError();
		return undef;
	}
	else {
		ofs << Chakra::GetString(arguments[2]);
	}
	ofs.close();
	return undef;
}

JsValueRef Filesystem::readSync(JsValueRef callee, bool isConstructor, JsValueRef* arguments, unsigned short argCount, void* callbackState) {
	auto ret = Chakra::GetUndefined();
	if (!Chakra::VerifyArgCount(argCount, 2)) return ret;
	if (!Chakra::VerifyParameters({ {arguments[1], JsString} })) return ret;
	std::wifstream ifs;
	std::wstringstream wss;
	auto thi = reinterpret_cast<Filesystem*>(callbackState);

	ifs.open(thi->getPath(Chakra::GetString(arguments[1])));
	if (ifs.fail()) {
		throwFsError();
		return ret;
	}
	else {
		wss << ifs.rdbuf();
	}
	ifs.close();
	JS::JsPointerToString(wss.str().c_str(), wss.str().size(), &ret);
	return ret;
}

JsValueRef Filesystem::existsSync(JsValueRef callee, bool isConstructor, JsValueRef* arguments, unsigned short argCount, void* callbackState) {
	auto ret = Chakra::GetUndefined();
	if (!Chakra::VerifyArgCount(argCount, 2)) return ret;
	if (!Chakra::VerifyParameters({ {arguments[1], JsString} })) return ret;
	std::wifstream ifs;
	std::wstringstream wss;
	auto thi = reinterpret_cast<Filesystem*>(callbackState);

	return std::filesystem::exists(thi->getPath(Chakra::GetString(arguments[1]))) ? Chakra::GetTrue() : Chakra::GetFalse();
}

JsValueRef Filesystem::createDirectorySync(JsValueRef callee, bool isConstructor, JsValueRef* arguments, unsigned short argCount, void* callbackState) {
	auto ret = Chakra::GetUndefined();
	if (!Chakra::VerifyArgCount(argCount, 2)) return ret;
	if (!Chakra::VerifyParameters({ {arguments[1], JsString} })) return ret;

	auto thi = reinterpret_cast<Filesystem*>(callbackState);
	std::error_code errCode;
	if (!std::filesystem::create_directory(thi->getPath(Chakra::GetString(arguments[1])), errCode)) {
		if (errCode.value() != ERROR_ALREADY_EXISTS) {
			Chakra::ThrowError(L"Filesystem error: " + util::StrToWStr(errCode.message()));
		}
		return ret;
	}
	return ret;
}
