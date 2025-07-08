#pragma once
#define visual virtual

namespace SDK {
	// An implementation of Minecraft's IDataOutput.

	class BasicDataOutput {
	public:
		visual ~BasicDataOutput() {};
		std::string mStr;

		virtual void writeString(void* gslStringSpan) {

		}
		virtual void writeLongString(void* gslStringSpan) {

		}
		virtual void writeFloat(float f) {

		}
		virtual void writeDouble(long double d) {

		}
		virtual void writeByte(char c) {

		}
		virtual void writeShort(int16_t s) {

		}
		virtual void writeInt(int i) {

		}
		virtual void writeLongLong(int64_t o) {

		}
		virtual void writeBytes(const void* bytes, uint64_t size) {

		}
		virtual bool isOk() {
			return true;
		}
	};
}