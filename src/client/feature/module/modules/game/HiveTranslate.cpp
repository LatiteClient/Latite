#include "pch.h"
#include "util/Util.h"
#include "util/Logger.h"
#include "HiveTranslate.h"
#include "client/Latite.h"

#include <regex>
#include <winrt/base.h>
#include <nlohmann/json.hpp>
#include <winrt/Windows.Web.Http.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/windows.storage.streams.h>

using namespace winrt;
using namespace winrt::Windows::Web::Http;
using namespace winrt::Windows::Foundation;

auto urlEncode = [](const std::string& str) {
    std::string encoded = str;
    for (size_t pos = 0; (pos = encoded.find(' ', pos)) != std::string::npos; pos += 3)
        encoded.replace(pos, 1, "%20");
    return encoded;
};

HiveTranslate::HiveTranslate() : Module("HiveTranslate", LocalizeString::get("client.module.hiveTranslate.name"),
                          LocalizeString::get("client.module.hiveTranslate.desc"), GAME, nokeybind) {
	 addSetting("targetLanguage", LocalizeString::get("client.module.hiveTranslate.targetLanguage.name"),
               LocalizeString::get("client.module.hiveTranslate.targetLanguage.desc"), this->targetLanguage);
    listen<ChatMessageEvent>(static_cast<EventListenerFunc>(&HiveTranslate::onText));
}

void HiveTranslate::onText(Event& evG) {
	std::string targetLang = util::WStrToStr(std::get<TextValue>(this->targetLanguage).str);
	if(targetLang == "") targetLang = "en";

    std::string message = reinterpret_cast<ChatMessageEvent&>(evG).getMessage();

    static const std::regex pattern("\u00c2\u00a77\u00c2\u00a7l\u00c2\u00bb \u00c2\u00a7r");
    
    std::smatch match;
	if (!std::regex_search(message, match, pattern) && match.position() == 0) return;

	std::string query = urlEncode(message.substr(match.position() + match.length()));
	std::string url = "https://clients5.google.com/translate_a/t?client=dict-chrome-ex&sl=auto&tl=" + targetLang + "&q=" + query;
	
	try {
		winrt::Windows::Foundation::Uri requestUri(std::wstring(url.begin(), url.end()));
		HttpRequestMessage request(HttpMethod::Get(), requestUri);
		
		auto http = HttpClient();
		
		http.SendRequestAsync(request).Completed([this, targetLang](const auto& op, auto status) {
			try {
				if (status != AsyncStatus::Completed) return;
				
				auto response = op.GetResults();
				
				if ((int)response.StatusCode() != 200) return;
				
				auto buffer = response.Content().ReadAsBufferAsync().get();
				std::string responseBody(reinterpret_cast<char*>(buffer.data()), buffer.Length());
				auto json = nlohmann::json::parse(responseBody);
				
				std::string originLang = json[0][1].get<std::string>();
				if (originLang == targetLang) return;
				
				std::string formattedMsg = "\u00c2\u00a77" + 
					util::WStrToStr(LocalizeString::get("client.module.hiveTranslate.translatedFrom")) +
					" " + originLang + ": " + json[0][0].get<std::string>();
				
				Latite::get().getClientMessageQueue().push(formattedMsg);
				
			}
			catch (nlohmann::json::exception const& err) {
				std::string msg = "HiveTranslate: HTTP request failed: " + std::string(err.what());
				Logger::Warn(msg);
				Latite::getClientMessageQueue().push(msg);
			}
			catch (winrt::hresult_error const& err) {
				std::string msg = "HiveTranslate: HTTP request failed: " + winrt::to_string(err.message());
				Logger::Warn(msg);
				Latite::getClientMessageQueue().push(msg);
			}
		});
		
	} catch (winrt::hresult_error const& err) {
		std::string msg = "HiveTranslate: HTTP request failed: " + winrt::to_string(err.message());
		Logger::Warn(msg);
		Latite::getClientMessageQueue().push(msg);
	}
}