//
// Created by aLIEzTed on 5/31/19.
//

#ifndef V8ANDROIDEMBEDDING_MYCLASS_H
#define V8ANDROIDEMBEDDING_MYCLASS_H


#include <v8.h>
#include <string>
#include "BindingBase.h"
class MyClass : public BindingBase {


 public:
  std::string clz;
// This class must be static only
  MyClass(std::string& utf);

  static void constructorCallback(const v8::FunctionCallbackInfo<v8::Value> &);


  static void ClassGetter(const v8::FunctionCallbackInfo<v8::Value> &);
};


#endif //V8ANDROIDEMBEDDING_MYCLASS_H
