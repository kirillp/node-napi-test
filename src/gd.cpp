#include <node_api.h>
#include <string>
#include <vector>

struct ElkLayoutKeys {
  napi_value id;
  napi_value children;
  napi_value layoutOptions;
  napi_value filePath;
  napi_value renderMode;
  napi_value showTensor;
  napi_value direction;
  napi_value algorithm;
  napi_value nodePlacementStrategy;
  napi_value nodeNodeBetweenLayers;
};

static napi_value makeKey(napi_env env, const char* str) {
  napi_value key;
  napi_create_string_utf8(env, str, NAPI_AUTO_LENGTH, &key);
  return key;
}

static bool getStringProp(napi_env env, napi_value obj, napi_value key, std::string& out) {
  napi_value value;
  if (napi_get_property(env, obj, key, &value) != napi_ok) return false;

  napi_valuetype type;
  if (napi_typeof(env, value, &type) != napi_ok || type != napi_string) return false;

  size_t len;
  if (napi_get_value_string_utf8(env, value, nullptr, 0, &len) != napi_ok) return false;

  std::string result(len, '\0');
  if (napi_get_value_string_utf8(env, value, &result[0], len + 1, &len) != napi_ok) return false;
  result.resize(len);
  out = std::move(result);
  return true;
}

struct TestInterface {
  std::string algorithm;
  std::string nodePlacementStrategy;
  std::string nodeNodeBetweenLayers;

  bool parse(napi_env env, napi_value obj, const ElkLayoutKeys& keys) {
    bool ok = true;
    ok = getStringProp(env, obj, keys.algorithm, algorithm) && ok;
    ok = getStringProp(env, obj, keys.nodePlacementStrategy, nodePlacementStrategy) && ok;
    ok = getStringProp(env, obj, keys.nodeNodeBetweenLayers, nodeNodeBetweenLayers) && ok;
    return ok;
  }
};

struct LayoutOptions4 {
  std::string filePath;
  std::string renderMode;
  std::string showTensor;
  std::string direction;
  std::string algorithm;
  std::string nodePlacementStrategy;
  std::string nodeNodeBetweenLayers;

  bool parse(napi_env env, napi_value obj, const ElkLayoutKeys& keys) {
    bool ok = true;
    ok = getStringProp(env, obj, keys.filePath, filePath) && ok;
    ok = getStringProp(env, obj, keys.renderMode, renderMode) && ok;
    ok = getStringProp(env, obj, keys.showTensor, showTensor) && ok;
    ok = getStringProp(env, obj, keys.direction, direction) && ok;
    ok = getStringProp(env, obj, keys.algorithm, algorithm) && ok;
    ok = getStringProp(env, obj, keys.nodePlacementStrategy, nodePlacementStrategy) && ok;
    ok = getStringProp(env, obj, keys.nodeNodeBetweenLayers, nodeNodeBetweenLayers) && ok;
    return ok;
  }
};

struct ElkLayout {
  std::vector<TestInterface> children;
  std::string id;
  LayoutOptions4 layoutOptions;

  bool parse(napi_env env, napi_value obj, const ElkLayoutKeys& keys) {
    napi_status status;
    bool ok = true;

    ok = getStringProp(env, obj, keys.id, id) && ok;

    napi_value layoutOptsObj;
    status = napi_get_property(env, obj, keys.layoutOptions, &layoutOptsObj);
    if (status == napi_ok) {
      napi_valuetype type;
      napi_typeof(env, layoutOptsObj, &type);
      if (type == napi_object) {
        ok = layoutOptions.parse(env, layoutOptsObj, keys) && ok;
      }
    }

    napi_value childrenVal;
    status = napi_get_property(env, obj, keys.children, &childrenVal);
    if (status == napi_ok) {
      bool is_array;
      napi_is_array(env, childrenVal, &is_array);
      if (is_array) {
        uint32_t len;
        napi_get_array_length(env, childrenVal, &len);
        children.reserve(len);
        for (uint32_t i = 0; i < len; i++) {
          napi_value element;
          status = napi_get_element(env, childrenVal, i, &element);
          if (status == napi_ok) {
            napi_valuetype elem_type;
            napi_typeof(env, element, &elem_type);
            if (elem_type == napi_object) {
              TestInterface ti;
              ti.parse(env, element, keys);
              children.push_back(std::move(ti));
            }
          }
        }
      }
    }

    return ok;
  }
};

napi_value graphLayout(napi_env env, napi_callback_info info) {
  size_t argc = 1;
  napi_value args[1];

  if (napi_get_cb_info(env, info, &argc, args, nullptr, nullptr) != napi_ok)
    return nullptr;
  if (argc < 1) return nullptr;

  napi_valuetype arg_type;
  if (napi_typeof(env, args[0], &arg_type) != napi_ok) return nullptr;
  if (arg_type != napi_object) return nullptr;

  ElkLayoutKeys keys = {
    makeKey(env, "id"),
    makeKey(env, "children"),
    makeKey(env, "layoutOptions"),
    makeKey(env, "PyPTO.filePath"),
    makeKey(env, "PyPTO.renderMode"),
    makeKey(env, "PyPTO.showTensor"),
    makeKey(env, "elk.direction"),
    makeKey(env, "org.eclipse.elk.algorithm"),
    makeKey(env, "org.eclipse.elk.layered.nodePlacement.strategy"),
    makeKey(env, "org.eclipse.elk.layered.spacing.nodeNodeBetweenLayers")
  };

  ElkLayout layout;
  layout.parse(env, args[0], keys);

  napi_value result;
  if (napi_get_boolean(env, true, &result) != napi_ok) return nullptr;
  return result;
}
