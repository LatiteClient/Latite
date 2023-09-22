#include "pch.h"
#include "JsCommand.h"

bool JsCommand::execute(std::string const label, std::vector<std::string> args) {
    __debugbreak(); // we shouldn't be here
    return false;
}

bool JsCommand::tryRun(std::string const& label, std::vector<std::string> args, std::string const& line) {
    JS::JsSetCurrentContext(ctx);
    JsValueRef array;
    JS::JsCreateArray(static_cast<unsigned>(args.size()), &array);
    for (int i = 0; i < args.size(); i++) {
        JS::JsSetIndexedProperty(array, Chakra::MakeInt(i), Chakra::MakeString(util::StrToWStr(args[i])));
    }

    Event ev{ L"execute", { Chakra::MakeString(util::StrToWStr(label)), array, Chakra::MakeString(util::StrToWStr(line)) } };
    return Chakra::GetBool(dispatchEvent(L"execute", ev));
}
