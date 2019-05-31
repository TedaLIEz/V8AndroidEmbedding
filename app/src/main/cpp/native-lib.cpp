#include <jni.h>
#include <string>
#include "libplatform/libplatform.h"
#include "v8.h"
#include "MyClass.h"
#include <android/log.h>

std::unique_ptr<v8::Platform> platform;
v8::Isolate *isolate;
v8::Persistent<v8::Context> persistentContext;

void ReportException(v8::Isolate *pIsolate, v8::TryCatch *pCatch);

extern "C" {

void JNICALL
Java_com_hustunique_v8demoapplication_MainActivity_initV8(JNIEnv *env, jobject /* this */) {
  // Initialize V8.
  v8::V8::InitializeICU();
  platform = v8::platform::NewDefaultPlatform();
  v8::V8::InitializePlatform(&(*platform.get()));
  v8::V8::Initialize();

}


void runScript() {
  const char *csource = R"(
    let mycls = new MyClass('yahaha');
    let clz = mycls.clz;
  )";

  // Create a stack-allocated handle scope.
  v8::HandleScope handle_scope(isolate);
  auto ctx = persistentContext.Get(isolate);
  v8::Context::Scope context_scope(ctx);
  v8::TryCatch try_catch(isolate);
  // Create a string containing the JavaScript source code.
  v8::Local<v8::String> source = v8::String::NewFromUtf8(isolate, csource,
                                                         v8::NewStringType::kNormal).ToLocalChecked();

  // Compile the source code.
  v8::Local<v8::Script> script =
      v8::Script::Compile(ctx, source).ToLocalChecked();
  v8::Local<v8::Value> result;
  if (!script->Run(ctx).ToLocal(&result)) {
    ReportException(isolate, &try_catch);
    return;
  }
  // Convert the result to an UTF8 string and print it.
  v8::String::Utf8Value utf8(isolate, result);
  __android_log_print(ANDROID_LOG_INFO, "V8Native", "%s\n", *utf8);

}
jstring JNICALL
Java_com_hustunique_v8demoapplication_MainActivity_stringFromJNI(JNIEnv *env, jobject /* this */) {
  // Create a new Isolate and make it the current one.
  std::string hello = "Hello";

  v8::Isolate::CreateParams create_params;
  create_params.array_buffer_allocator = v8::ArrayBuffer::Allocator::NewDefaultAllocator();
  isolate = v8::Isolate::New(create_params);
//  isolate->Enter();
  v8::Isolate::Scope isolate_scope(isolate);
  v8::HandleScope scope(isolate);


  auto ft = v8::FunctionTemplate::New(isolate);
  ft->SetClassName(v8::String::NewFromUtf8(isolate, "MyClass"));
  ft->ReadOnlyPrototype();
  ft->SetCallHandler(MyClass::constructorCallback);
  ft->SetLength(1);
  v8::Local<v8::ObjectTemplate> instance_template = ft->InstanceTemplate();
  v8::Local<v8::ObjectTemplate> prototype_template = ft->PrototypeTemplate();
  instance_template->SetInternalFieldCount(1);

  prototype_template->Set(v8::Symbol::GetToStringTag(isolate), v8::String::NewFromUtf8(isolate, "MyClass"),
                          static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontEnum));

  v8::Local<v8::Signature> signature = v8::Signature::New(isolate, ft);

  v8::Local<v8::FunctionTemplate> getter = v8::FunctionTemplate::New(
      isolate, MyClass::ClassGetter, v8::Local<v8::Value>(), signature, 0 );

  if (!getter.IsEmpty()) {
    getter->RemovePrototype();
    getter->SetAcceptAnyReceiver(true);
  }

  prototype_template->SetAccessorProperty(v8::String::NewFromUtf8(isolate, "clz", v8::NewStringType::kInternalized, 3).ToLocalChecked(),
      getter, v8::Local<v8::FunctionTemplate>(), static_cast<v8::PropertyAttribute>( v8::DontDelete));

  v8::Eternal<v8::FunctionTemplate> v8EventConstructor(isolate, ft);
  auto global_template = v8::ObjectTemplate::New(isolate);
  global_template->Set(v8::String::NewFromUtf8(isolate, "MyClass"), v8EventConstructor.Get(isolate));

  // Enter the context for compiling and running the hello world script.
  v8::Local<v8::Context> context = v8::Context::New(isolate, nullptr, global_template);
  persistentContext.Reset(isolate, context);

  // Run the script to get the result.
  runScript();
  return env->NewStringUTF(hello.c_str());
}

}

const char *ToCString(const v8::String::Utf8Value &value) {
  return *value ? *value : "<string conversion failed>";
}

void ReportException(v8::Isolate *isolate, v8::TryCatch *try_catch) {
  v8::HandleScope handle_scope(isolate);
  v8::String::Utf8Value exception(isolate, try_catch->Exception());
  const char *exception_string = ToCString(exception);
  v8::Local<v8::Message> message = try_catch->Message();
  if (message.IsEmpty()) {
    // V8 didn't provide any extra information about this error; just
    // print the exception.
    __android_log_print(ANDROID_LOG_ERROR, "V8Native", "%s\n", exception_string);
//    fprintf(stderr, "%s\n", exception_string);
  } else {
    // Print (filename):(line number): (message).
    v8::String::Utf8Value filename(isolate,
                                   message->GetScriptOrigin().ResourceName());
    v8::Local<v8::Context> context(isolate->GetCurrentContext());
    const char *filename_string = ToCString(filename);
    int linenum = message->GetLineNumber(context).FromJust();
    __android_log_print(ANDROID_LOG_ERROR, "V8Native", "%s:%i: %s\n", filename_string, linenum,
                        exception_string);
//    fprintf(stderr, "%s:%i: %s\n", filename_string, linenum, exception_string);
    // Print line of source code.
    v8::String::Utf8Value sourceline(
        isolate, message->GetSourceLine(context).ToLocalChecked());
    const char *sourceline_string = ToCString(sourceline);
    __android_log_print(ANDROID_LOG_ERROR, "V8Native", "%s\n", sourceline_string);
//    fprintf(stderr, "%s\n", sourceline_string);
    // Print wavy underline (GetUnderline is deprecated).
    int start = message->GetStartColumn(context).FromJust();
    for (int i = 0; i < start; i++) {
      __android_log_print(ANDROID_LOG_ERROR, "V8Native", " ");
//      fprintf(stderr, " ");
    }
    int end = message->GetEndColumn(context).FromJust();
    for (int i = start; i < end; i++) {
      __android_log_print(ANDROID_LOG_ERROR, "V8Native", "^");
//      fprintf(stderr, "^");
    }
    __android_log_print(ANDROID_LOG_ERROR, "V8Native", "\n");
//    fprintf(stderr, "\n");
    v8::Local<v8::Value> stack_trace_string;
    if (try_catch->StackTrace(context).ToLocal(&stack_trace_string) &&
        stack_trace_string->IsString() &&
        v8::Local<v8::String>::Cast(stack_trace_string)->Length() > 0) {
      v8::String::Utf8Value stack_trace(isolate, stack_trace_string);
      const char *stack_trace_string = ToCString(stack_trace);
      __android_log_print(ANDROID_LOG_ERROR, "V8Native", "%s\n", stack_trace_string);
//      fprintf(stderr, "%s\n", stack_trace_string);
    }
  }
}
