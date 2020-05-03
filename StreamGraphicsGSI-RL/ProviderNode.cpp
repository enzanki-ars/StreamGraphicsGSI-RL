#include "ProviderNode.h"

nlohmann::json ProviderNode::GetJson()
{
	nlohmann::json json_provider;
	json_provider["name"] = Name;
	json_provider["appid"] = AppId;
	return json_provider;
};
