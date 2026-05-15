#include <node_api.h>
#include <string>
#include <vector>

static napi_value makeKey(napi_env env, const char* str) {
  napi_value key;
  napi_create_string_utf8(env, str, NAPI_AUTO_LENGTH, &key);
  return key;
}

struct ElkLayoutKeys {
  napi_value id;
  napi_value children;
  napi_value edges;
  napi_value layoutOptions;
  napi_value height;
  napi_value width;
  napi_value ports;
  napi_value sources;
  napi_value targets;

  napi_value nodeLabelsPlacement;
  napi_value nodeSizeMinimum;
  napi_value padding;
  napi_value portConstraints;
  napi_value spacingLabelLabel;
  napi_value spacingLabelNode;

  napi_value portBorderOffset;
  napi_value portSide;

  napi_value filePath;
  napi_value renderMode;
  napi_value showTensor;
  napi_value direction;
  napi_value algorithm;
  napi_value nodePlacementStrategy;
  napi_value nodeNodeBetweenLayers;

  bool init(napi_env env) {
    id = makeKey(env, "id");
    children = makeKey(env, "children");
    edges = makeKey(env, "edges");
    layoutOptions = makeKey(env, "layoutOptions");
    height = makeKey(env, "height");
    width = makeKey(env, "width");
    ports = makeKey(env, "ports");
    sources = makeKey(env, "sources");
    targets = makeKey(env, "targets");

    nodeLabelsPlacement = makeKey(env, "org.eclipse.elk.nodeLabels.placement");
    nodeSizeMinimum = makeKey(env, "org.eclipse.elk.nodeSize.minimum");
    padding = makeKey(env, "org.eclipse.elk.padding");
    portConstraints = makeKey(env, "org.eclipse.elk.portConstraints");
    spacingLabelLabel = makeKey(env, "org.eclipse.elk.spacing.labelLabel");
    spacingLabelNode = makeKey(env, "org.eclipse.elk.spacing.labelNode");

    portBorderOffset = makeKey(env, "org.eclipse.elk.port.borderOffset");
    portSide = makeKey(env, "org.eclipse.elk.port.side");

    filePath = makeKey(env, "PyPTO.filePath");
    renderMode = makeKey(env, "PyPTO.renderMode");
    showTensor = makeKey(env, "PyPTO.showTensor");
    direction = makeKey(env, "elk.direction");
    algorithm = makeKey(env, "org.eclipse.elk.algorithm");
    nodePlacementStrategy = makeKey(env, "org.eclipse.elk.layered.nodePlacement.strategy");
    nodeNodeBetweenLayers = makeKey(env, "org.eclipse.elk.layered.spacing.nodeNodeBetweenLayers");

    return id && children && edges && layoutOptions &&
           height && width && ports && sources && targets &&
           nodeLabelsPlacement && nodeSizeMinimum && padding && portConstraints &&
           spacingLabelLabel && spacingLabelNode &&
           portBorderOffset && portSide &&
           filePath && renderMode && showTensor &&
           direction && algorithm && nodePlacementStrategy && nodeNodeBetweenLayers;
  }
};

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

static bool getNumberProp(napi_env env, napi_value obj, napi_value key, double& out) {
  napi_value value;
  if (napi_get_property(env, obj, key, &value) != napi_ok) return false;

  napi_valuetype type;
  if (napi_typeof(env, value, &type) != napi_ok || type != napi_number) return false;

  if (napi_get_value_double(env, value, &out) != napi_ok) return false;
  return true;
}

static bool getStringArrayProp(napi_env env, napi_value obj, napi_value key, std::vector<std::string>& out) {
  napi_value value;
  if (napi_get_property(env, obj, key, &value) != napi_ok) return false;

  bool is_array;
  if (napi_is_array(env, value, &is_array) != napi_ok) return false;
  if (!is_array) return false;

  uint32_t len;
  if (napi_get_array_length(env, value, &len) != napi_ok) return false;

  out.reserve(len);
  for (uint32_t i = 0; i < len; i++) {
    napi_value element;
    if (napi_get_element(env, value, i, &element) != napi_ok) return false;

    napi_valuetype elem_type;
    if (napi_typeof(env, element, &elem_type) != napi_ok) return false;
    if (elem_type != napi_string) return false;

    size_t str_len;
    if (napi_get_value_string_utf8(env, element, nullptr, 0, &str_len) != napi_ok) return false;

    std::string s(str_len, '\0');
    if (napi_get_value_string_utf8(env, element, &s[0], str_len + 1, &str_len) != napi_ok) return false;
    s.resize(str_len);
    out.push_back(std::move(s));
  }
  return true;
}

struct LayoutOptions {
  std::string nodeLabelsPlacement;
  std::string nodeSizeMinimum;
  std::string padding;
  std::string portConstraints;
  std::string spacingLabelLabel;
  std::string spacingLabelNode;

  bool parse(napi_env env, napi_value obj, const ElkLayoutKeys& keys) {
    bool ok = true;
    ok = ok && getStringProp(env, obj, keys.nodeLabelsPlacement, nodeLabelsPlacement);
    ok = ok && getStringProp(env, obj, keys.nodeSizeMinimum, nodeSizeMinimum);
    ok = ok && getStringProp(env, obj, keys.padding, padding);
    ok = ok && getStringProp(env, obj, keys.portConstraints, portConstraints);
    ok = ok && getStringProp(env, obj, keys.spacingLabelLabel, spacingLabelLabel);
    ok = ok && getStringProp(env, obj, keys.spacingLabelNode, spacingLabelNode);
    return ok;
  }
};

struct LayoutOptions3 {
  std::string portBorderOffset;
  std::string portSide;

  bool parse(napi_env env, napi_value obj, const ElkLayoutKeys& keys) {
    return getStringProp(env, obj, keys.portBorderOffset, portBorderOffset) &&
           getStringProp(env, obj, keys.portSide, portSide);
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
    ok = ok && getStringProp(env, obj, keys.filePath, filePath);
    ok = ok && getStringProp(env, obj, keys.renderMode, renderMode);
    ok = ok && getStringProp(env, obj, keys.showTensor, showTensor);
    ok = ok && getStringProp(env, obj, keys.direction, direction);
    ok = ok && getStringProp(env, obj, keys.algorithm, algorithm);
    ok = ok && getStringProp(env, obj, keys.nodePlacementStrategy, nodePlacementStrategy);
    ok = ok && getStringProp(env, obj, keys.nodeNodeBetweenLayers, nodeNodeBetweenLayers);
    return ok;
  }
};

struct PortsItem {
  double height;
  std::string id;
  LayoutOptions3 layoutOptions;
  double width;

  bool parse(napi_env env, napi_value obj, const ElkLayoutKeys& keys) {
    napi_status status;

    if (!getNumberProp(env, obj, keys.height, height)) return false;
    if (!getStringProp(env, obj, keys.id, id)) return false;
    if (!getNumberProp(env, obj, keys.width, width)) return false;

    napi_value loObj;
    status = napi_get_property(env, obj, keys.layoutOptions, &loObj);
    if (status != napi_ok) return false;

    napi_valuetype type;
    status = napi_typeof(env, loObj, &type);
    if (status != napi_ok) return false;
    if (type != napi_object) return false;

    if (!layoutOptions.parse(env, loObj, keys)) return false;
    return true;
  }
};

struct ChildrenItem {
  double height;
  std::string id;
  LayoutOptions layoutOptions;
  std::vector<PortsItem> ports;
  double width;

  bool parse(napi_env env, napi_value obj, const ElkLayoutKeys& keys) {
    napi_status status;

    if (!getNumberProp(env, obj, keys.height, height)) return false;
    if (!getStringProp(env, obj, keys.id, id)) return false;
    if (!getNumberProp(env, obj, keys.width, width)) return false;

    napi_value loObj;
    status = napi_get_property(env, obj, keys.layoutOptions, &loObj);
    if (status == napi_ok) {
      napi_valuetype type;
      status = napi_typeof(env, loObj, &type);
      if (status == napi_ok && type == napi_object) {
        layoutOptions.parse(env, loObj, keys);
      }
    }

    napi_value portsVal;
    status = napi_get_property(env, obj, keys.ports, &portsVal);
    if (status != napi_ok) return false;

    bool is_array;
    status = napi_is_array(env, portsVal, &is_array);
    if (status != napi_ok) return false;
    if (!is_array) return false;

    uint32_t len;
    status = napi_get_array_length(env, portsVal, &len);
    if (status != napi_ok) return false;

    ports.reserve(len);
    for (uint32_t i = 0; i < len; i++) {
      napi_value element;
      status = napi_get_element(env, portsVal, i, &element);
      if (status != napi_ok) return false;

      napi_valuetype elem_type;
      status = napi_typeof(env, element, &elem_type);
      if (status != napi_ok) return false;
      if (elem_type != napi_object) return false;

      PortsItem pi;
      if (!pi.parse(env, element, keys)) return false;
      ports.push_back(std::move(pi));
    }

    return true;
  }
};

struct EdgesItem {
  std::string id;
  std::vector<std::string> sources;
  std::vector<std::string> targets;

  bool parse(napi_env env, napi_value obj, const ElkLayoutKeys& keys) {
    return getStringProp(env, obj, keys.id, id) &&
           getStringArrayProp(env, obj, keys.sources, sources) &&
           getStringArrayProp(env, obj, keys.targets, targets);
  }
};

struct ElkLayout {
  std::vector<ChildrenItem> children;
  std::vector<EdgesItem> edges;
  std::string id;
  LayoutOptions4 layoutOptions;

  bool parse(napi_env env, napi_value obj, const ElkLayoutKeys& keys) {
    napi_status status;

    if (!getStringProp(env, obj, keys.id, id)) return false;

    napi_value layoutOptsObj;
    status = napi_get_property(env, obj, keys.layoutOptions, &layoutOptsObj);
    if (status != napi_ok) return false;

    napi_valuetype type;
    status = napi_typeof(env, layoutOptsObj, &type);
    if (status != napi_ok) return false;
    if (type != napi_object) return false;

    if (!layoutOptions.parse(env, layoutOptsObj, keys)) return false;

    napi_value childrenVal;
    status = napi_get_property(env, obj, keys.children, &childrenVal);
    if (status != napi_ok) return false;

    bool is_array;
    status = napi_is_array(env, childrenVal, &is_array);
    if (status != napi_ok) return false;
    if (!is_array) return false;

    {
      uint32_t len;
      status = napi_get_array_length(env, childrenVal, &len);
      if (status != napi_ok) return false;

      children.reserve(len);
      for (uint32_t i = 0; i < len; i++) {
        napi_value element;
        status = napi_get_element(env, childrenVal, i, &element);
        if (status != napi_ok) return false;

        napi_valuetype elem_type;
        status = napi_typeof(env, element, &elem_type);
        if (status != napi_ok) return false;
        if (elem_type != napi_object) return false;

        ChildrenItem ci;
        if (!ci.parse(env, element, keys)) return false;
        children.push_back(std::move(ci));
      }
    }

    napi_value edgesVal;
    status = napi_get_property(env, obj, keys.edges, &edgesVal);
    if (status != napi_ok) return false;

    status = napi_is_array(env, edgesVal, &is_array);
    if (status != napi_ok) return false;
    if (!is_array) return false;

    {
      uint32_t len;
      status = napi_get_array_length(env, edgesVal, &len);
      if (status != napi_ok) return false;

      edges.reserve(len);
      for (uint32_t i = 0; i < len; i++) {
        napi_value element;
        status = napi_get_element(env, edgesVal, i, &element);
        if (status != napi_ok) return false;

        napi_valuetype elem_type;
        status = napi_typeof(env, element, &elem_type);
        if (status != napi_ok) return false;
        if (elem_type != napi_object) return false;

        EdgesItem ei;
        if (!ei.parse(env, element, keys)) return false;
        edges.push_back(std::move(ei));
      }
    }

    return true;
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

  ElkLayoutKeys keys;
  if (!keys.init(env)) return nullptr;

  ElkLayout layout;
  if (!layout.parse(env, args[0], keys)) return nullptr;

  napi_value jsTrue;
  if (napi_get_boolean(env, true, &jsTrue) != napi_ok) return nullptr;
  return jsTrue;
}
