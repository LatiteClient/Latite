#include "pch.h"
#include "util/Util.h"
#include "util/Logger.h"
#include "HiveTranslate.h"
#include "client/Latite.h"
#include "client/misc/ServerDetection.h"
#include "util/HttpClient.h"

#include <regex>
#include <nlohmann/json.hpp>

auto urlEncode = [](const std::string& str) {
    std::string encoded = str;
    for (size_t pos = 0; (pos = encoded.find(' ', pos)) != std::string::npos; pos += 3)
        encoded.replace(pos, 1, "%20");
    return encoded;
};

HiveTranslate::HiveTranslate()
    : Module("HiveTranslate", LocalizeString::get("client.module.hiveTranslate.name"),
             LocalizeString::get("client.module.hiveTranslate.desc"), GAME, nokeybind) {
    addSetting("targetLanguage", LocalizeString::get("client.module.hiveTranslate.targetLanguage.name"),
               LocalizeString::get("client.module.hiveTranslate.targetLanguage.desc"), this->targetLanguage);
    listen<ChatMessageEvent>(static_cast<EventListenerFunc>(&HiveTranslate::onText));
}

void HiveTranslate::onText(Event& evG) {
    if (!ServerDetection::matches(SDK::RemoteConnectorComposite::getConnectionInfo(),
                                  ServerDetection::ServerId::Hive)) {
        return;
    }

    std::string targetLang = util::WStrToStr(std::get<TextValue>(this->targetLanguage).str);
    if (targetLang == "") targetLang = "en";

    std::string message = reinterpret_cast<ChatMessageEvent&>(evG).getMessage();

    static const std::regex pattern("\u00c2\u00a77\u00c2\u00a7l\u00c2\u00bb \u00c2\u00a7r");

    std::smatch match;
    if (!std::regex_search(message, match, pattern) && match.position() == 0) return;

    std::string query = urlEncode(message.substr(match.position() + match.length()));
    std::string url =
        "https://clients5.google.com/translate_a/t?client=dict-chrome-ex&sl=auto&tl=" + targetLang + "&q=" + query;

    std::thread([url = util::StrToWStr(url), targetLang] {
        auto response = http::request(L"GET", url);
        if (!response) {
            Logger::Warn("HiveTranslate: HTTP request failed: {}", response.error());
            return;
        }
        if (response->status != 200) return;

        try {
            auto translation = nlohmann::json::parse(response->body);
            std::string originLang = translation[0][1].get<std::string>();
            if (originLang == targetLang) return;

            std::string formattedMsg =
                "\u00c2\u00a77" + util::WStrToStr(LocalizeString::get("client.module.hiveTranslate.translatedFrom")) +
                " " + originLang + ": " + translation[0][0].get<std::string>();

            Latite::getClientMessageQueue().push(formattedMsg);
        } catch (nlohmann::json::exception const& error) {
            Logger::Warn("HiveTranslate: Invalid response: {}", error.what());
        }
    }).detach();
}
