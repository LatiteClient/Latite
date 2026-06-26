#pragma once
#include <optional>
#include <string>
#include <vector>
#include "client/feature/Feature.h"
#include "client/localization/LocalizeString.h"

class ICommand : public Listener, public Feature {
public:
    ICommand(std::string const& name, std::wstring const& description, std::string const& usage,
             std::vector<std::string> aliases = {})
        : cmdName(name)
        , description(description)
        , usage(usage) {
        this->aliases = { name };
        for (auto& a : aliases) {
            this->aliases.push_back(a);
        }
    }
    ICommand(std::string const& name, LocalizedString const& description, std::string const& usage,
             std::vector<std::string> aliases = {})
        : ICommand(name, description.value(), usage, aliases) {
        descriptionKey = description.key();
    }
    ~ICommand() = default;

    virtual bool tryRun(std::string const& label, std::vector<std::string> args, std::string const& commandLine) {
        return execute(label, args);
    }

    [[nodiscard]] virtual bool isLocalWorldOnly() { return this->localWorldOnly; }
    [[nodiscard]] virtual bool isScript() { return this->script; }
    [[nodiscard]] std::vector<std::string> getAliases() { return aliases; }
    [[nodiscard]] std::string getUsage() { return usage; }

    virtual void message(std::wstring const& str, bool error = false) = 0;

    [[nodiscard]] std::wstring desc() override { return description; };
    [[nodiscard]] std::string name() override { return cmdName; };

    virtual void refreshLocalization() {
        if (descriptionKey) description = LocalizeString::get(*descriptionKey).value();
    }

protected:
    std::string cmdName, usage;
    std::wstring description;
    std::optional<std::string> descriptionKey;
    std::vector<std::string> aliases;
    bool script = false;

    virtual bool execute(std::string const label, std::vector<std::string> args) = 0;

private:
    bool localWorldOnly;
};

class Command : public ICommand {
public:
    ~Command() = default;
    Command(std::string const& name, std::wstring const& description, std::string const& usage,
            std::vector<std::string> aliases = {})
        : ICommand(name, description, usage, aliases) {}
    Command(std::string const& name, LocalizedString const& description, std::string const& usage,
            std::vector<std::string> aliases = {})
        : ICommand(name, description, usage, aliases) {}

    virtual void message(std::wstring const& str, bool error = false) override;
    void message(std::string const& str, bool error = false);

protected:
};
