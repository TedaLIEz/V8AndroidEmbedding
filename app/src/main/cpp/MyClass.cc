//
// Created by aLIEzTed on 5/31/19.
//

#include <android/log.h>
#include "MyClass.h"


MyClass::MyClass(std::string& utf) : clz(utf) {

}


void MyClass::ClassGetter(const v8::FunctionCallbackInfo<v8::Value> &info) {
  __android_log_print(ANDROID_LOG_DEBUG, "MyClass", "getter called in js");
  MyClass* clz = reinterpret_cast<MyClass*>(info.Holder()->GetAlignedPointerFromInternalField(0));
  if (clz != nullptr) {
    info.GetReturnValue().Set(v8::String::NewFromUtf8(info.GetIsolate(), clz->clz.c_str()));
  } else {
    info.GetReturnValue().Set(v8::Null(info.GetIsolate()));
  }
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
  std::string str(*utf);
  auto event = new MyClass(str);
  v8::Local<v8::Object> wrapper = ci.Holder();
  event->AssociateWithWrapper(ci.GetIsolate(), wrapper, 16);
  ci.GetReturnValue().Set(wrapper);
}