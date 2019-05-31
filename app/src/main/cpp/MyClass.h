//
// Created by aLIEzTed on 5/31/19.
//

#ifndef V8ANDROIDEMBEDDING_MYCLASS_H
#define V8ANDROIDEMBEDDING_MYCLASS_H


#include <v8.h>
class MyClass {
 public:
// This class must be static only
  MyClass(const char* utf);

  static void constructorCallback(const v8::FunctionCallbackInfo<v8::Value> &);

};


#endif //V8ANDROIDEMBEDDING_MYCLASS_H
