#include <jni.h>
#include <string>
#include "libplatform/libplatform.h"
#include "v8.h"
#include <android/log.h>

std::unique_ptr<v8::Platform> platform;
v8::Isolate *isolate;
v8::Persistent<v8::Context> persistentContext;

void ReportException(v8::Isolate *pIsolate, v8::TryCatch *pCatch);

extern "C" {

void JNICALL Java_com_hustunique_v8demoapplication_MainActivity_initV8(JNIEnv *env, jobject /* this */) {
  // Initialize V8.
  v8::V8::InitializeICU();
  platform = v8::platform::NewDefaultPlatform();
  v8::V8::InitializePlatform(&(*platform.get()));
  v8::V8::Initialize();

  // Create a new Isolate and make it the current one.
  v8::Isolate::CreateParams create_params;
  create_params.array_buffer_allocator = v8::ArrayBuffer::Allocator::NewDefaultAllocator();
  isolate = v8::Isolate::New(create_params);

  v8::Isolate::Scope isolate_scope(isolate);
  // Create a stack-allocated handle scope.
  v8::HandleScope handle_scope(isolate);

  // Create a new context.
  v8::Local<v8::Context> context = v8::Context::New(isolate);

  // attach the context to the persistent context, to avoid V8 GC-ing it
  persistentContext.Reset(isolate, context);
}

jstring JNICALL
Java_com_hustunique_v8demoapplication_MainActivity_stringFromJNI(JNIEnv *env, jobject /* this */) {
    std::string hello = "Hello v8 from C++!\n";
    const char *csource = R"(
    let mycls = new MyClass();
  )";
    v8::Isolate::Scope isolate_scope(isolate);
    v8::HandleScope handle_scope(isolate);
    v8::TryCatch try_catch(isolate);
    // Enter the context for compiling and running the hello world script.
    v8::Local<v8::Context> context = v8::Local<v8::Context>::New(isolate, persistentContext);
    v8::Context::Scope context_scope(context);

    // Create a string containing the JavaScript source code.
//  v8::Local<v8::String> source = v8::String::NewFromUtf8(
//          isolate, "'Hello' + ', from Javascript!'", v8::NewStringType::kNormal).ToLocalChecked();
    v8::Local<v8::String> source = v8::String::NewFromUtf8(isolate, csource,
                                                           v8::NewStringType::kNormal).ToLocalChecked();

    // Compile the source code.
    v8::Local<v8::Script> script =
            v8::Script::Compile(context, source).ToLocalChecked();

    // Run the script to get the result.
    v8::Local<v8::Value> result;
    if (!script->Run(context).ToLocal(&result)) {
        ReportException(isolate, &try_catch);
        return env->NewStringUTF(hello.c_str());
    }
//  v8::Local<v8::Value> result = script->Run(context).ToLocalChecked();

    // Convert the result to an UTF8 string and print it.
    v8::String::Utf8Value utf8(isolate, result);
    printf("%s\n", *utf8);
    hello += *utf8;

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
