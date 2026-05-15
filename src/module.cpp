#include <stdio.h>
#include <node_api.h>

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

static inline bool napiCheck(napi_env env, napi_status status) {
  if (status != napi_ok) {
    napi_throw_error(env, nullptr, "N-API call failed");
    return false;
  }
  return true;
}

static const char* nodeTypeName(napi_valuetype type) {
  switch (type) {
    case napi_undefined: return "undefined";
    case napi_null:      return "null";
    case napi_boolean:   return "boolean";
    case napi_number:    return "number";
    case napi_string:    return "string";
    case napi_symbol:    return "symbol";
    case napi_function:  return "function";
    case napi_object:    return "object";
    case napi_external:  return "external";
    case napi_bigint:    return "bigint";
    default:             return "unknown";
  }
}

static napi_value Method(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value world;
  status = napi_create_string_utf8(env, "world", 5, &world);
  if (!napiCheck(env, status)) return nullptr;
  return world;
}

static napi_value LogObject(napi_env env, napi_callback_info info) {
  napi_status status;
  size_t argc = 1;
  napi_value args[1];

  status = napi_get_cb_info(env, info, &argc, args, nullptr, nullptr);
  if (!napiCheck(env, status)) return nullptr;

  if (argc < 1) {
    fprintf(stdout, "Error: expected an object argument\n");
    fflush(stdout);
    return nullptr;
  }

  napi_valuetype arg_type;
  status = napi_typeof(env, args[0], &arg_type);
  if (!napiCheck(env, status)) return nullptr;

  if (arg_type != napi_object) {
    fprintf(stdout, "Error: argument must be an object\n");
    fflush(stdout);
    return nullptr;
  }

  napi_value keys;
  status = napi_get_property_names(env, args[0], &keys);
  if (!napiCheck(env, status)) return nullptr;

  uint32_t length;
  status = napi_get_array_length(env, keys, &length);
  if (!napiCheck(env, status)) return nullptr;

  fprintf(stdout, "Object {\n");

  for (uint32_t i = 0; i < length; i++) {
    napi_value key;
    status = napi_get_element(env, keys, i, &key);
    if (!napiCheck(env, status)) return nullptr;

    char key_str[256];
    size_t key_len;
    status = napi_get_value_string_utf8(env, key, key_str, sizeof(key_str), &key_len);
    if (!napiCheck(env, status)) return nullptr;
    key_str[key_len] = '\0';

    napi_value value; 
    status = napi_get_property(env, args[0], key, &value);
    if (!napiCheck(env, status)) return nullptr;

    napi_valuetype value_type;
    status = napi_typeof(env, value, &value_type);
    if (!napiCheck(env, status)) return nullptr;

    const char *type_str = nodeTypeName(value_type);

    fprintf(stdout, "  %s (%s): ", key_str, type_str);

    if (value_type == napi_string) {
      char val_str[256];
      size_t val_len;
      napi_get_value_string_utf8(env, value, val_str, sizeof(val_str), &val_len);
      val_str[val_len] = '\0';
      fprintf(stdout, "\"%s\"", val_str);
    } else if (value_type == napi_boolean) {
      bool b;
      napi_get_value_bool(env, value, &b);
      fprintf(stdout, "%s", b ? "true" : "false");
    } else if (value_type == napi_number) {
      double d;
      napi_get_value_double(env, value, &d);
      fprintf(stdout, "%g", d);
    } else if (value_type == napi_null) {
      fprintf(stdout, "null");
    } else {
      fprintf(stdout, "[%s]", type_str);
    }

    fprintf(stdout, "\n");
  }

  fprintf(stdout, "}\n");
  fflush(stdout);

  return nullptr;
}

static napi_value TransformRecursive(napi_env env, napi_value input) {
  napi_status status;

  napi_value output;
  status = napi_create_object(env, &output);
  if (!napiCheck(env, status)) return nullptr;

  napi_value keys;
  status = napi_get_property_names(env, input, &keys);
  if (!napiCheck(env, status)) return nullptr;

  uint32_t length;
  status = napi_get_array_length(env, keys, &length);
  if (!napiCheck(env, status)) return nullptr;

  for (uint32_t i = 0; i < length; i++) {
    napi_value key;
    status = napi_get_element(env, keys, i, &key);
    if (!napiCheck(env, status)) return nullptr;

    char key_str[256];
    size_t key_len;
    status = napi_get_value_string_utf8(env, key, key_str, sizeof(key_str), &key_len);
    if (!napiCheck(env, status)) return nullptr;
    key_str[key_len] = '\0';

    napi_value value;
    status = napi_get_property(env, input, key, &value);
    if (!napiCheck(env, status)) return nullptr;

    napi_valuetype value_type;
    status = napi_typeof(env, value, &value_type);
    if (!napiCheck(env, status)) return nullptr;

    if (value_type == napi_string || value_type == napi_number) {
      napi_value new_value;
      status = napi_create_string_utf8(env, key_str, key_len, &new_value);
      if (!napiCheck(env, status)) return nullptr;
      status = napi_set_property(env, output, value, new_value);
      if (!napiCheck(env, status)) return nullptr;
    } else if (value_type == napi_object) {
      bool is_array;
      status = napi_is_array(env, value, &is_array);
      if (!napiCheck(env, status)) return nullptr;

      if (is_array) {
        uint32_t arr_length;
        status = napi_get_array_length(env, value, &arr_length);
        if (!napiCheck(env, status)) return nullptr;

        napi_value new_array;
        status = napi_create_array(env, &new_array);
        if (!napiCheck(env, status)) return nullptr;

        uint32_t new_idx = 0;
        for (uint32_t j = 0; j < arr_length; j++) {
          napi_value element;
          status = napi_get_element(env, value, j, &element);
          if (!napiCheck(env, status)) return nullptr;

          napi_valuetype elem_type;
          status = napi_typeof(env, element, &elem_type);
          if (!napiCheck(env, status)) return nullptr;

          if (elem_type == napi_object) {
            bool elem_is_array;
            status = napi_is_array(env, element, &elem_is_array);
            if (!napiCheck(env, status)) return nullptr;
            if (!elem_is_array) {
              napi_value processed = TransformRecursive(env, element);
              if (processed == nullptr) return nullptr;
              status = napi_set_element(env, new_array, new_idx++, processed);
              if (!napiCheck(env, status)) return nullptr;
            }
          }
        }

        status = napi_set_property(env, output, key, new_array);
        if (!napiCheck(env, status)) return nullptr;
      } else {
        napi_value processed = TransformRecursive(env, value);
        if (processed == nullptr) return nullptr;
        status = napi_set_property(env, output, key, processed);
        if (!napiCheck(env, status)) return nullptr;
      }
    } else {
      status = napi_set_property(env, output, key, value);
      if (!napiCheck(env, status)) return nullptr;
    }
  }

  return output;
}

static napi_value TransformObject(napi_env env, napi_callback_info info) {
  napi_status status;
  size_t argc = 1;
  napi_value args[1];

  status = napi_get_cb_info(env, info, &argc, args, nullptr, nullptr);
  if (!napiCheck(env, status)) return nullptr;

  if (argc < 1) {
    napi_throw_type_error(env, nullptr, "Expected an object argument");
    return nullptr;
  }

  napi_valuetype arg_type;
  status = napi_typeof(env, args[0], &arg_type);
  if (!napiCheck(env, status)) return nullptr;

  if (arg_type != napi_object) {
    napi_throw_type_error(env, nullptr, "Argument must be an object");
    return nullptr;
  }

  bool is_array;
  status = napi_is_array(env, args[0], &is_array);
  if (!napiCheck(env, status)) return nullptr;
  if (is_array) {
    napi_throw_type_error(env, nullptr, "Argument must be a plain object, not an array");
    return nullptr;
  }

  return TransformRecursive(env, args[0]);
}

static napi_value Init(napi_env env, napi_value exports) {
  napi_status status;
  napi_property_descriptor desc[] = {
    {"hello", 0, Method, 0,0,0, napi_default, 0},
    {"logObject", 0, LogObject, 0,0,0, napi_default, 0},
    {"transformObject", 0, TransformObject, 0,0,0, napi_default, 0}
  };
  status = napi_define_properties(env, exports, 3, desc);
  if (!napiCheck(env, status)) return nullptr;
  return exports;
}

NAPI_MODULE(NODE_GYP_MODULE_NAME, Init)
