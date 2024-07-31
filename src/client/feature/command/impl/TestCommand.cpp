#include "pch.h"
#include "TestCommand.h"
#include "client/Latite.h"
#include "sdk/common/client/gui/controls/UIControl.h"
#include "sdk/common/client/gui/controls/VisualTree.h"

static SDK::UIControl* bossbar = nullptr;

TestCommand::TestCommand() : Command("test", L"A command for testing", "$ [...]", {"tc"}) {
	Eventing::get().listen<RenderLayerEvent>(this, (EventListenerFunc)&TestCommand::onRenderLayer);
}

bool TestCommand::execute(std::string const label, std::vector<std::string> args) {

#if 0
	winrt::hstring Title = L"title";
	winrt::hstring Content = L"djfiadhswfahsd9fasdf";

	winrt::Windows::UI::Popups::UICommand yesCommand;
	yesCommand.Label(L"custombutton1");
	auto yesId = yesCommand.Id();

	// Create No button
	winrt::Windows::UI::Popups::UICommand noCommand;
	noCommand.Label(L"custombutton2");
	auto noId = noCommand.Id();

	winrt::Windows::UI::Popups::MessageDialog dialog(Content, Title);
	dialog.Commands().Append(yesCommand);
	dialog.Commands().Append(noCommand);
#endif
#if 0
	//Latite::get().fetchLatiteUsers();
	if (!bossbar) {
		message("Cannot find bossbar", false);
		return true;
	}

	static auto textcomp = memory::instructionToAddress( util::FindSignature("48 8d 05 ? ? ? ? 48 89 01 48 83 c1 ? 48 8d 15 ? ? ? ? e8 ? ? ? ? 90 48 8d 4b"));

	bossbar->getDescendants([&](std::shared_ptr<SDK::UIControl> contr) {
		message("> " + contr->name);
		for (auto& comp : contr->uiComponents) {
			if (*(uintptr_t*)comp == textcomp) {
				auto tc = (SDK::TextComponent*)comp;
				Logger::Info("{} {} {}", tc->resolvedText, tc->lang, tc->variableToText);
			}
		}
		});
#endif

	auto lp = SDK::ClientInstance::get()->getLocalPlayer();
	if (!lp) return true;
	//Logger::Info("{} {} {}", lp->getHealth(), lp->getHunger(), lp->getSaturation());
	
	if (args.size() < 1) return false;

	//SDK::LevelRendererPlayer::selecOffset = std::stoull(args.at(0), 0, 16);



	return true;
}

void TestCommand::onRenderLayer(Event& evG) {
	//auto& ev = reinterpret_cast<RenderLayerEvent&>(evG);
	//if (!bossbar) bossbar = ev.getScreenView()->visualTree->rootControl->findFirstDescendantWithName("boss_health_grid");
}
