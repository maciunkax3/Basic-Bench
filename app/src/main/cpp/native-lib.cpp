#include <jni.h>
#include <string>
#include <CL/cl_ext.h>
#include "OCL_Common/OCL_Init.h"
#include "OCL_Common/Context.h"
#include "OCL_Common/Kernel.h"
#include "OCL_Common/Queue.h"
#include "OCL_Common/Buffer.h"
#include "log.h"
#include "DataTypesEnum.h"
#include "flops_mesrement.h"

OCL::Runtime *runtime = nullptr;
FlopsMesurement *flopsMesurement = nullptr;

const char *program = R"===(
__constant char hello[24] = {'H','e','l','l','o',' ','w','o','r','l','d',' ','f','r','o','m',' ','O','p','e','n','C','l','\0'};
__kernel void hello_opencl(__global char *dst){
    uint gid = get_global_id(0);
    if(gid <= 24){
        dst[gid] = hello[gid];
    }
}
)===";

extern "C"
JNIEXPORT void JNICALL
Java_com_example_basicbench_MainActivity_closeOCL(JNIEnv *env, jobject thiz) {
    if(runtime!= nullptr){
        delete runtime;
    }
    if(flopsMesurement != nullptr){
        delete flopsMesurement;
    }
}extern "C"
JNIEXPORT void JNICALL
Java_com_example_basicbench_MainActivity_initOCL(JNIEnv *env, jobject thiz) {
    if(runtime == nullptr){
        int status;
        runtime = new OCL::Runtime(status);
        LOGI("Runtime create: %d\n", status);
    }
    flopsMesurement = new FlopsMesurement();
    flopsMesurement->initMap();
}extern "C"
JNIEXPORT jstring JNICALL
Java_com_example_basicbench_MainActivity_helloWorldFromOCL(JNIEnv *env, jobject thiz) {
    auto kernel = std::make_unique<OCL::Kernel>(runtime->context.get(), program, "hello_opencl", "");
    auto queue =  std::make_unique<OCL::Queue>(runtime);
    auto buffer = std::make_unique<OCL::Buffer>(runtime->context.get(), 100, nullptr);

    kernel->gws[0]= 32;
    kernel->lws[0]= 32;
    kernel->dims=1;
    kernel->setArg<cl_mem>(0, &buffer->memObj);

    queue->runKernel(kernel.get());

    char hello[100];
    buffer->toHost(queue.get(), &hello);
    queue->waitForExecutionFinish();
    
    return env->NewStringUTF(hello);
}
extern "C"
JNIEXPORT jdouble JNICALL
Java_com_example_basicbench_MainActivity_runFlopsTest(JNIEnv *env, jobject thiz, jint data_type) {
    DataTypes type = (DataTypes)(data_type);
    double flops =  flopsMesurement->runTest(type, TestType::Flops);
    return flops;
}

extern "C"
JNIEXPORT jdouble JNICALL
Java_com_example_basicbench_MainActivity_runBandwith(JNIEnv *env, jobject thiz, jint data_type) {
    DataTypes type = (DataTypes)(data_type);
    double result =  flopsMesurement->runTest(type, TestType::Mesurement);
    return result;
}