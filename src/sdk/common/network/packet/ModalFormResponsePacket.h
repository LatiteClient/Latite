#pragma once
#include "../Packet.h"
#include "sdk/String.h"

namespace SDK {

    class ModalFormResponsePacket : public Packet {
    public:
        
        class Value {
        public:
            class CZString {
            public:
                char const* cstr_;
            };

            enum class Type : int {
                Null = 0x0,
                Int = 0x1,
                Uint = 0x2,
                Real = 0x3,
                String = 0x4,
                Boolean = 0x5,
                Array = 0x6,
                Object = 0x7,
            };

            union {
                int64_t                    int_;
                uint64_t                   uint_;
                double                     real_;
                bool                       bool_;
                char*                      str_;
                std::map<CZString, Value>* map_;
            } value_;

            Type type_ : 8;

            // Function to serialize the Value to JSON
            friend void to_json(nlohmann::json& result, const Value& v) {
                result = nullptr;
                switch (v.type_) {
                    case Type::Int:
                        result = v.value_.int_;
                        break;
                    case Type::Uint:
                        result = v.value_.uint_;
                        break;
                    case Type::Real:
                        result = v.value_.real_;
                        break;
                    case Type::String:
                        result = v.value_.str_;
                        break;
                    case Type::Boolean:
                        result = v.value_.bool_;
                        break;
                    case Type::Array:
                        // result = nlohmann::json::array();
                        // TODO: Implement array parsing
                        break;
                    case Type::Object:
                        // result = nlohmann::json::object();
                        // TODO: Implement object parsing
                        break;
                }
            };
        };

        enum class Reason : signed char {
            UserClosed,
            UserBusy
        };

        unsigned int          Id;
        std::optional<Value>  Json;
        std::optional<Reason> CancelReason;

    };

}