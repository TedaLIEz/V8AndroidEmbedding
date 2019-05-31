//
// Created by aLIEzTed on 5/31/19.
//

#ifndef V8ANDROIDEMBEDDING_BINDINGBASE_H
#define V8ANDROIDEMBEDDING_BINDINGBASE_H


#include <v8.h>
class BindingBase {
 private:
  v8::Persistent<v8::Object> wrapper_;

 protected:
  virtual v8::Local<v8::Object> AssociateWithWrapper(
      v8::Isolate *,
      v8::Local<v8::Object> wrapper,
      uint16_t class_id);

  bool SetWrapper(v8::Isolate *isolate,
                  v8::Local<v8::Object>& wrapper,
                  uint16_t class_id);

  void SetNativeInfo(v8::Local<v8::Object> wrapper );

  bool ContainWrapper() const {
    return !wrapper_.IsEmpty();
  }
 public:
  virtual ~BindingBase();

};


#endif //V8ANDROIDEMBEDDING_BINDINGBASE_H
