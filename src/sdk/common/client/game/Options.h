#pragma once
namespace SDK {
	class Options {
	public:
		static Options& get();

		Options(const Options&) = delete;
		Options& operator=(const Options&) = delete;

		void setPlayerViewPerspective(int) {
			// TODO: lol
		}

		int getPlayerViewPerspective() {
			// TODO: lol
			return 0;
		}

		bool IsGfxVSyncEnabled();
	private:
		Options() = default;
		~Options() = default;

		static Options* instance;
	};
}