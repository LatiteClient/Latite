#pragma once
namespace SDK {
	enum class OptionID {
		Vsync = 54,
	};

	class Option {
	public:

		class Impl {
		private:
			char pad[0x124];
		public:
			OptionID id;
		};


		std::unique_ptr<Impl> impl;

		virtual ~Option() = 0;
	};

	class BoolOption : public Option {
	public:
		bool value;
		bool defaultValue;
		std::function<bool(bool)> coerceValueCallback;
	};
}