#pragma once
namespace SDK {
	// An implementation of Minecraft's PrintStream.

	class BasicPrintStream {
	public:
		BasicPrintStream() : mStr("") {};
		BasicPrintStream(std::string str) : mStr(std::move(str)) {};

		std::string mStr;

		virtual ~BasicPrintStream() {}
		virtual void print(const std::string& str) {
			mStr.append(str);
		}
	};
}