#include "irods_plugin_context.hpp"
#include "irods_re_plugin.hpp"
#include "irods_re_serialization.hpp"
#include "irods_re_ruleexistshelper.hpp"
#include "rcMisc.h"
#include "rodsError.h"

#include <string>
#include <array>
#include <algorithm>
#include <iterator>
#include <functional>

using pluggable_rule_engine = irods::pluggable_rule_engine<irods::default_re_ctx>;

namespace {

// This is a "sorted" list of the supported PEPs.
// This will allow us to do binary search on the list for lookups.
constexpr std::array<const char*, 7> peps{{
    "pep_api_coll_create_post",
    "pep_api_data_obj_close_post",
    "pep_api_data_obj_close_pre",
    "pep_api_data_obj_put_post",
    "pep_api_data_obj_rename_post",
    "pep_api_data_obj_unlink_post",
    "pep_api_rm_coll_post"
}};

//
// PEP Handlers
//

namespace handler {

irods::error pep_api_coll_create_post(std::list<boost::any>& _rule_arguments, irods::callback& _effect_handler)
{
    rodsLog(LOG_NOTICE, ">>> fallthrough::pep_api_coll_create_post");
    return ERROR(SYS_NOT_SUPPORTED, "not really an error");
}

irods::error pep_api_data_obj_close_pre(std::list<boost::any>& _rule_arguments, irods::callback& _effect_handler)
{
    rodsLog(LOG_NOTICE, ">>> fallthrough::pep_api_data_obj_close_pre");
    return ERROR(SYS_NOT_SUPPORTED, "not really an error");
}

irods::error pep_api_data_obj_close_post(std::list<boost::any>& _rule_arguments, irods::callback& _effect_handler)
{
    rodsLog(LOG_NOTICE, ">>> fallthrough::pep_api_data_obj_close_post");
    return ERROR(SYS_NOT_SUPPORTED, "not really an error");
}

irods::error pep_api_data_obj_put_post(std::list<boost::any>& _rule_arguments, irods::callback& _effect_handler)
{
    rodsLog(LOG_NOTICE, ">>> fallthrough::pep_api_data_obj_put_post");
    return ERROR(SYS_NOT_SUPPORTED, "not really an error");
}

irods::error pep_api_data_obj_rename_post(std::list<boost::any>& _rule_arguments, irods::callback& _effect_handler)
{
    rodsLog(LOG_NOTICE, ">>> fallthrough::pep_api_data_obj_rename_post");
    return ERROR(SYS_NOT_SUPPORTED, "not really an error");
}

irods::error pep_api_data_obj_unlink_post(std::list<boost::any>& _rule_arguments, irods::callback& _effect_handler)
{
    rodsLog(LOG_NOTICE, ">>> fallthrough::pep_api_data_obj_unlink_post");
    return ERROR(SYS_NOT_SUPPORTED, "not really an error");
}

irods::error pep_api_rm_coll_post(std::list<boost::any>& _rule_arguments, irods::callback& _effect_handler)
{
    rodsLog(LOG_NOTICE, ">>> fallthrough::pep_api_rm_coll_post");
    return ERROR(SYS_NOT_SUPPORTED, "not really an error");
}

} // namespace handler

//
// Rule Engine Plugin
//

template <typename ...Args>
using operation = std::function<irods::error(irods::default_re_ctx&, Args...)>;

irods::error rule_exists(irods::default_re_ctx&, const std::string& _rule_name, bool& _exists)
{
    auto b = std::cbegin(peps);
    auto e = std::cend(peps);

    _exists = std::binary_search(b, e, _rule_name.c_str(), [](const auto* _lhs, const auto* _rhs) {
        return strcmp(_lhs, _rhs) < 0;
    });

    return SUCCESS();
}

irods::error list_rules(irods::default_re_ctx&, std::vector<std::string>& _rules)
{
    _rules.insert(std::end(_rules), std::begin(peps), std::end(peps));
    return SUCCESS();
}

irods::error exec_rule(irods::default_re_ctx&,
                       const std::string& _rule_name,
                       std::list<boost::any>& _rule_arguments,
                       irods::callback _effect_handler)
{
    using handler_t = std::function<irods::error(std::list<boost::any>&, irods::callback&)>;

    static const std::map<std::string, handler_t> handlers{
        {peps[0], handler::pep_api_coll_create_post},
        {peps[1], handler::pep_api_data_obj_close_post},
        {peps[2], handler::pep_api_data_obj_close_pre},
        {peps[3], handler::pep_api_data_obj_put_post},
        {peps[4], handler::pep_api_data_obj_rename_post},
        {peps[5], handler::pep_api_data_obj_unlink_post},
        {peps[6], handler::pep_api_rm_coll_post}
    };

    auto iter = handlers.find(_rule_name);

    if (std::end(handlers) != iter) {
        return (iter->second)(_rule_arguments, _effect_handler);
    }

    const auto* msg = "[irods_rule_engine_plugin-fallthrough][rule => %s] "
                      "rule not supported in rule engine plugin";

    rodsLog(LOG_ERROR, msg, _rule_name.c_str());

    // DO NOT BLOCK RULE ENGINE PLUGINS THAT FOLLOW THIS ONE!
    return ERROR(SYS_NOT_SUPPORTED, msg);
}

} // namespace (anonymous)

//
// Plugin Factory
//

extern "C"
pluggable_rule_engine* plugin_factory(const std::string& _instance_name,
                                      const std::string& _context)
{
    // clang-format off
    const auto no_op         = [] { return SUCCESS(); };
    const auto not_supported = [] { return ERROR(SYS_NOT_SUPPORTED, "Not supported"); };
    // clang-format on

    auto* re = new pluggable_rule_engine{_instance_name, _context};

    re->add_operation("start", {no_op});
    re->add_operation("stop", {no_op});
    re->add_operation("rule_exists", operation<const std::string&, bool&>{rule_exists});
    re->add_operation("list_rules", operation<std::vector<std::string>&>{list_rules});
    re->add_operation("exec_rule", operation<const std::string&, std::list<boost::any>&, irods::callback>{exec_rule});
    re->add_operation("exec_rule_text", {not_supported});
    re->add_operation("exec_rule_expression", {not_supported});

    return re;
}

