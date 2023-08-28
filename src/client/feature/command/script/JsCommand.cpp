#include "pch.h"
#include "JsCommand.h"

bool JsCommand::execute(std::string const label, std::vector<std::string> args) {
    JS::JsSetCurrentContext(ctx);
    JsValueRef array;
    JS::JsCreateArray(static_cast<unsigned>(args.size()), &array);
    for (int i = 0; i < args.size(); i++) {
        JS::JsSetIndexedProperty(array, Chakra::MakeInt(i), Chakra::MakeString(util::StrToWStr(args[i])));
    }

    Event ev{ L"execute", { Chakra::MakeString(util::StrToWStr(label)), array }};
    return dispatchEvent(L"execute", ev);
}
