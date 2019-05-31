//
// Created by aLIEzTed on 5/31/19.
//

#include "BindingBase.h"


v8::Local<v8::Object> BindingBase::AssociateWithWrapper(v8::Isolate *isolate,
                                                        v8::Local<v8::Object> wrapper,
                                                        uint16_t class_id) {

  if (SetWrapper(isolate, wrapper, class_id)) {
    SetNativeInfo(wrapper);
  }

  return wrapper;
}

static void weakCallbackForDOMObjectHolder(const v8::WeakCallbackInfo<BindingBase> &data) {
  delete data.GetParameter();
}

bool BindingBase::SetWrapper(v8::Isolate *isolate,
                             v8::Local<v8::Object> &wrapper,
                             uint16_t class_id) {
  if (ContainWrapper()) {
    wrapper = v8::Local<v8::Object>::New(isolate, wrapper_);
    return false;
  }
  wrapper_.Reset(isolate, wrapper);
  wrapper_.SetWrapperClassId(class_id);
// basic GC management.
  wrapper_.SetWeak(
      this,
      weakCallbackForDOMObjectHolder,
      v8::WeakCallbackType::kParameter);

  return true;
}
void BindingBase::SetNativeInfo(v8::Local<v8::Object> wrapper) {
  // set native info
  int indices[] = {0};
  void *values[] = {this};

  wrapper->SetAlignedPointerInInternalFields(
      1, indices, values);
}

BindingBase::~BindingBase() {
  if (ContainWrapper()) {
    wrapper_.Reset();
  }
}
