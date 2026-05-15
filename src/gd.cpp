#include <node_api.h>

napi_value GraphLayout(napi_env env, napi_callback_info info) {
  size_t argc = 1;
  napi_value args[1];

  if (napi_get_cb_info(env, info, &argc, args, nullptr, nullptr) != napi_ok)
    return nullptr;

  if (argc < 1) return nullptr;

  napi_valuetype arg_type;
  if (napi_typeof(env, args[0], &arg_type) != napi_ok) return nullptr;

  if (arg_type != napi_object) return nullptr;

  napi_value result;
  if (napi_get_boolean(env, true, &result) != napi_ok) return nullptr;

  return result;
}
