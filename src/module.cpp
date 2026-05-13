#include <assert.h>
#include <stdio.h>
#include <node_api.h>

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

static napi_value Method(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value world;
  status = napi_create_string_utf8(env, "world", 5, &world);
  assert(status == napi_ok);
  return world;
}

static napi_value LogObject(napi_env env, napi_callback_info info) {
  napi_status status;
  size_t argc = 1;
  napi_value args[1];

  status = napi_get_cb_info(env, info, &argc, args, nullptr, nullptr);
  assert(status == napi_ok);

  if (argc < 1) {
    fprintf(stdout, "Error: expected an object argument\n");
    fflush(stdout);
    return nullptr;
  }

  napi_valuetype arg_type;
  status = napi_typeof(env, args[0], &arg_type);
  assert(status == napi_ok);

  if (arg_type != napi_object) {
    fprintf(stdout, "Error: argument must be an object\n");
    fflush(stdout);
    return nullptr;
  }

  napi_value keys;
  status = napi_get_property_names(env, args[0], &keys);
  assert(status == napi_ok);

  uint32_t length;
  status = napi_get_array_length(env, keys, &length);
  assert(status == napi_ok);

  fprintf(stdout, "Object {\n");

  for (uint32_t i = 0; i < length; i++) {
    napi_value key;
    status = napi_get_element(env, keys, i, &key);
    assert(status == napi_ok);

    char key_str[256];
    size_t key_len;
    status = napi_get_value_string_utf8(env, key, key_str, sizeof(key_str), &key_len);
    assert(status == napi_ok);
    key_str[key_len] = '\0';

    napi_value value; 
    status = napi_get_property(env, args[0], key, &value);
    assert(status == napi_ok);

    napi_valuetype value_type;
    status = napi_typeof(env, value, &value_type);
    assert(status == napi_ok);

    const char *type_str;
    switch (value_type) {
      case napi_undefined: type_str = "undefined"; break;
      case napi_null:      type_str = "null";      break;
      case napi_boolean:   type_str = "boolean";   break;
      case napi_number:    type_str = "number";    break;
      case napi_string:    type_str = "string";    break;
      case napi_symbol:    type_str = "symbol";    break;
      case napi_function:  type_str = "function";  break;
      case napi_object:    type_str = "object";    break;
      case napi_external:  type_str = "external";  break;
      case napi_bigint:    type_str = "bigint";    break;
      default:             type_str = "unknown";   break;
    }

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

static napi_value Init(napi_env env, napi_value exports) {
  napi_status status;
  napi_property_descriptor desc[] = {
    {"hello", 0, Method, 0,0,0, napi_default, 0},
    {"logObject", 0, LogObject, 0,0,0, napi_default, 0}
  };
  status = napi_define_properties(env, exports, 2, desc);
  assert(status == napi_ok);
  //DebugBreak();
  return exports;
}

NAPI_MODULE(NODE_GYP_MODULE_NAME, Init)
