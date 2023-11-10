#include "pch.h"
#include "SignCommand.h"

SignCommand::SignCommand() : Command("sign", "Signs a script", "<script>", {"cert"}) {
}

bool SignCommand::execute(std::string const label, std::vector<std::string> args) {
	// in case this somehow doesn't get optimized away
#if LATITE_DEBUG
	if (args.empty()) return false;
	auto path = util::GetLatitePath() / "Plugins" / args[0];
	if (!std::filesystem::exists(path)) {
		message("Could not find script " + path.string(), true);
		return true;
	}
	auto cert = JsPlugin::getHash(path);
	if (cert) {
#if LATITE_DEBUG
		message(std::format("Generated certificate {}", util::WStrToStr(cert.value())));
#endif
		std::wofstream ofs(path / XOR_STRING("certificate"));
		ofs << cert.value();
		ofs.flush();
		return true;
	}
	message(std::format("Could not create certificate for script {}", path.string(), true));
#endif
	return true;
}
