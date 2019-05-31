//
// Created by aLIEzTed on 5/31/19.
//

#include <android/log.h>
#include "MyClass.h"


MyClass::MyClass(const char *utf) {

}

void MyClass::constructorCallback(const v8::FunctionCallbackInfo<v8::Value> &ci) {
  __android_log_print(ANDROID_LOG_DEBUG, "MyClass", "constructor called in js");
  if (!ci.IsConstructCall()) {
    auto isolate = ci.GetIsolate();
    if (!isolate->IsExecutionTerminating()) {
      isolate->ThrowException(
          v8::Exception::Error(
              v8::String::NewFromUtf8(
                  isolate,
                  "Must be constructor")));
    }
    return;
  }
  if (ci.kArgsLength < 1 || !ci[0]->IsString()) {
    auto isolate = ci.GetIsolate();
    if (!isolate->IsExecutionTerminating()) {
      isolate->ThrowException(
          v8::Exception::Error(
              v8::String::NewFromUtf8(
                  isolate,
                  "MyClass needs a type string argument")));
    }
  }

  v8::String::Utf8Value utf(ci.GetIsolate(), ci[0]);
  __android_log_print(ANDROID_LOG_DEBUG, "MyClass", "js called MyClass with param %s", *utf);
  auto event = new MyClass(*utf);
  v8::Local<v8::Object> wrapper = ci.Holder();
  ci.GetReturnValue().Set(wrapper);
}