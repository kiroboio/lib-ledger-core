// AUTOGENERATED FILE - DO NOT MODIFY!
// This file generated by Djinni from wallet.djinni

#include "TrustLevel.hpp"  // my header
#include "enum_from_string.hpp"

namespace ledger { namespace core { namespace api {

std::string to_string(const TrustLevel& trustLevel) {
    switch (trustLevel) {
        case TrustLevel::TRUSTED: return "TRUSTED";
        case TrustLevel::PENDING: return "PENDING";
        case TrustLevel::UNTRUSTED: return "UNTRUSTED";
        case TrustLevel::DROPPED: return "DROPPED";
    };
};
template <>
TrustLevel from_string(const std::string& trustLevel) {
    if (trustLevel == "TRUSTED") return TrustLevel::TRUSTED;
    else if (trustLevel == "PENDING") return TrustLevel::PENDING;
    else if (trustLevel == "UNTRUSTED") return TrustLevel::UNTRUSTED;
    else return TrustLevel::DROPPED;
};

std::ostream &operator<<(std::ostream &os, const TrustLevel &o)
{
    switch (o) {
        case TrustLevel::TRUSTED:  return os << "TRUSTED";
        case TrustLevel::PENDING:  return os << "PENDING";
        case TrustLevel::UNTRUSTED:  return os << "UNTRUSTED";
        case TrustLevel::DROPPED:  return os << "DROPPED";
    }
}

} } }  // namespace ledger::core::api