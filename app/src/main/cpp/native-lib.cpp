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
#include <algorithm>

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

const char *matrixMul = R"===(
__kernel void matrixMul(__global int *A, __global int *B, __global int *dst, int N, int M){
    uint gidX = get_global_id(0);
    uint gidY = get_global_id(1);
    int tmp = 0;
    for(int k =0;k<N;k++){
        tmp += A[(gidY * N) + k] * B[(k * M) +gidX];
    }
    dst[gidY*M+gidX] = tmp;
}
)===";

extern "C"
JNIEXPORT void JNICALL
Java_com_example_basicbench_MainActivity_closeOCL(JNIEnv *env, jobject thiz) {
    if (runtime != nullptr) {
        delete runtime;
    }
    if (flopsMesurement != nullptr) {
        delete flopsMesurement;
    }
}extern "C"
JNIEXPORT void JNICALL
Java_com_example_basicbench_MainActivity_initOCL(JNIEnv *env, jobject thiz) {
    if (runtime == nullptr) {
        int status;
        runtime = new OCL::Runtime(status);
        //LOGI("Runtime create: %d\n", status);
    }
    flopsMesurement = new FlopsMesurement();
    flopsMesurement->initMap();
}extern "C"
JNIEXPORT jstring JNICALL
Java_com_example_basicbench_MainActivity_helloWorldFromOCL(JNIEnv *env, jobject thiz) {
    auto kernel = std::make_unique<OCL::Kernel>(runtime->context.get(), program, "hello_opencl",
                                                "");
    auto queue = std::make_unique<OCL::Queue>(runtime);
    auto buffer = std::make_unique<OCL::Buffer>(runtime->context.get(), 100, nullptr);

    kernel->gws[0] = 32;
    kernel->lws[0] = 32;
    kernel->dims = 1;
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
    DataTypes type = (DataTypes) (data_type);
    double flops = flopsMesurement->runTest(type, TestType::Flops);
    return flops;
}

extern "C"
JNIEXPORT jdouble JNICALL
Java_com_example_basicbench_MainActivity_runBandwith(JNIEnv *env, jobject thiz, jint data_type) {
    DataTypes type = (DataTypes) (data_type);
    double result = flopsMesurement->runTest(type, TestType::Bandwith);
    return result;
}
extern "C"
JNIEXPORT jdouble JNICALL
Java_com_example_basicbench_MainActivity_runOCLLatency(JNIEnv *env, jobject thiz) {
    double result = flopsMesurement->runLatency();
    return result;
}extern "C"
JNIEXPORT jdouble JNICALL
Java_com_example_basicbench_MainActivity_runReadBuffer(JNIEnv *env, jobject thiz) {
    auto queue = std::make_unique<OCL::Queue>(runtime);
    auto event = clCreateUserEvent(runtime->context.get()->context, NULL);
    auto size = 1024 * 1024 * 10;
    auto buffer = std::make_unique<OCL::Buffer>(runtime->context.get(), size, nullptr);
    int pattern = 0x1234;
    clEnqueueFillBuffer(queue->queue, buffer->memObj, &pattern, sizeof(int), 0, size, 0, nullptr,
                        nullptr);
    queue->waitForExecutionFinish();
    auto iters = 256;
    auto mem = std::make_unique<uint8_t[]>(size);

    double time = 0.0;
    for (int i = 0; i < iters; i++) {
        clEnqueueReadBuffer(queue->queue, buffer->memObj, false, 0, size, mem.get(), 0, nullptr,
                            &event);
        queue->waitForExecutionFinish();
        double timeNs = runtime->getKernelExecutionTime(event);
        time += timeNs / (1000 * 1000 * 1000);
    }
    return (10.0 * iters) / time;
}extern "C"
JNIEXPORT jdouble JNICALL
Java_com_example_basicbench_MainActivity_runWriteBuffer(JNIEnv *env, jobject thiz) {
    auto queue = std::make_unique<OCL::Queue>(runtime);
    auto event = clCreateUserEvent(runtime->context.get()->context, NULL);
    auto size = 1024 * 1024 * 10;
    auto buffer = std::make_unique<OCL::Buffer>(runtime->context.get(), size, nullptr);
    auto memSrc = std::make_unique<uint8_t[]>(size);
    auto memDst = std::make_unique<uint8_t[]>(size);
    memset(memSrc.get(), 45, size);
    auto iters = 256;

    clEnqueueWriteBuffer(queue->queue, buffer->memObj, false, 0, size, memSrc.get(), 0, nullptr,
                         &event);
    clEnqueueWriteBuffer(queue->queue, buffer->memObj, false, 0, size, memSrc.get(), 0, nullptr,
                         &event);
    queue->waitForExecutionFinish();
    double time = 0.0;
    for (int i = 0; i < iters; i++) {
        clEnqueueWriteBuffer(queue->queue, buffer->memObj, false, 0, size, memSrc.get(), 0, nullptr,
                             &event);
        queue->waitForExecutionFinish();
        double timeNs = runtime->getKernelExecutionTime(event);
        time += timeNs / (1000 * 1000 * 1000);
        //buffer->toHost(queue.get(), memDst.get());
        //queue->waitForExecutionFinish();
        //LOGI("0: %d, 1024: %d, 1024 * 1024 - 1: %d\n", memDst[0], memDst[sizeof(int) * 1024],
        //     memDst[(sizeof(int) * (1024 * 1024 - 1))]);
        //memset(memSrc.get(), 45+ i, size);
    }
    return (10.0 * iters) / time;
}extern "C"
JNIEXPORT jdouble JNICALL
Java_com_example_basicbench_MainActivity_runMapBuffer(JNIEnv *env, jobject thiz) {
    auto queue = std::make_unique<OCL::Queue>(runtime);
    auto event = clCreateUserEvent(runtime->context.get()->context, NULL);
    auto size = 1024 * 1024 * 10;
    auto buffer = std::make_unique<OCL::Buffer>(runtime->context.get(), size, nullptr);
    int pattern = 0x1234;
    clEnqueueFillBuffer(queue->queue, buffer->memObj, &pattern, sizeof(int), 0, size, 0, nullptr,
                        nullptr);
    queue->waitForExecutionFinish();
    auto iters = 256;
    auto mem = std::make_unique<uint8_t[]>(size);

    double time = 0.0;
    for (int i = 0; i < iters; i++) {
        int err = -1;
        auto ptr = clEnqueueMapBuffer(queue->queue, buffer->memObj, false,
                                      CL_MAP_READ | CL_MAP_WRITE, 0, size, 0, nullptr, &event,
                                      &err);
        queue->waitForExecutionFinish();
        double timeNs = runtime->getKernelExecutionTime(event);
        time += timeNs / (1000 * 1000 * 1000);
    }
    return (10.0 * iters) / time;
}extern "C"
JNIEXPORT jdouble JNICALL
Java_com_example_basicbench_MainActivity_runUnMapBuffer(JNIEnv *env, jobject thiz) {
    auto queue = std::make_unique<OCL::Queue>(runtime);
    auto event = clCreateUserEvent(runtime->context.get()->context, NULL);
    auto size = 1024 * 1024 * 10;
    auto buffer = std::make_unique<OCL::Buffer>(runtime->context.get(), size, nullptr);
    int pattern = 0x1234;
    clEnqueueFillBuffer(queue->queue, buffer->memObj, &pattern, sizeof(int), 0, size, 0, nullptr,
                        nullptr);
    queue->waitForExecutionFinish();
    auto iters = 256;
    auto mem = std::make_unique<uint8_t[]>(size);

    double time = 0.0;
    for (int i = 0; i < iters; i++) {
        int err = -1;
        auto ptr = clEnqueueMapBuffer(queue->queue, buffer->memObj, false,
                                      CL_MAP_READ | CL_MAP_WRITE, 0, size, 0, nullptr, nullptr,
                                      &err);
        queue->waitForExecutionFinish();
        clEnqueueUnmapMemObject(queue->queue, buffer->memObj, ptr, 0, nullptr, &event);
        queue->waitForExecutionFinish();
        double timeNs = runtime->getKernelExecutionTime(event);
        time += timeNs / (1000 * 1000 * 1000);
    }
    return (10.0 * iters) / time;
}extern "C"
JNIEXPORT jstring JNICALL
Java_com_example_basicbench_MainActivity_runMatrixMul(JNIEnv *env, jobject thiz) {
    int sizeN = std::max(runtime->maxWG, (size_t)1024);
    int sizeM = std::max(runtime->maxWG, (size_t)1024);
    auto bufferA = std::make_unique<OCL::Buffer>(runtime->context.get(),
                                                 sizeN * sizeM * sizeof(int), nullptr);
    auto bufferB = std::make_unique<OCL::Buffer>(runtime->context.get(),
                                                 sizeN * sizeM * sizeof(int), nullptr);
    auto bufferDst = std::make_unique<OCL::Buffer>(runtime->context.get(),
                                                   sizeN * sizeN * sizeof(int), nullptr);

    auto queue = std::make_unique<OCL::Queue>(runtime);

    auto kernel = std::make_unique<OCL::Kernel>(runtime->context.get(), matrixMul, "matrixMul", "");
    kernel->gws[0] = sizeM;
    kernel->gws[1] = sizeN;
    kernel->lws[0] = runtime->maxWG;
    kernel->dims = 2;
    kernel->setArg<cl_mem>(0, &bufferA->memObj);
    kernel->setArg<cl_mem>(1, &bufferB->memObj);
    kernel->setArg<cl_mem>(2, &bufferDst->memObj);
    kernel->setArg<int>(3, &sizeM);
    kernel->setArg<int>(4, &sizeN);
    kernel->createEvent();

    int pattern = 2;
    clEnqueueFillBuffer(queue->queue, bufferA->memObj, &pattern, sizeof(int), 0,
                        sizeN * sizeM * sizeof(int), 0, nullptr,
                        nullptr);
    pattern = 3;
    clEnqueueFillBuffer(queue->queue, bufferB->memObj, &pattern, sizeof(int), 0,
                        sizeN * sizeM * sizeof(int), 0, nullptr,
                        nullptr);
    pattern = 0;
    clEnqueueFillBuffer(queue->queue, bufferDst->memObj, &pattern, sizeof(int), 0,
                        sizeN * sizeM * sizeof(int), 0, nullptr,
                        nullptr);
    queue->waitForExecutionFinish();

    auto iters = 3;
    std::string output = "";
    queue->runKernel(kernel.get());
    queue->runKernel(kernel.get());
    queue->waitForExecutionFinish();
    while (kernel->lws[0] > 0) {
        auto time = 0.0;
        for (int i = 0; i < iters; i++) {
            queue->runKernel(kernel.get());
            queue->waitForExecutionFinish();
            time += (double) (runtime->getKernelExecutionTime(kernel->event)) * 0.000000001;
        }
        time /= iters;
        output.append("Matrix mul lws: ");
        output.append(std::to_string(kernel->lws[0]));
        output.append("x");
        output.append(std::to_string(kernel->lws[1]));
        output.append(" time: ");
        output.append(std::to_string(time));
        output.append("\n");
        kernel->lws[0] = kernel->lws[0] >> 1;
        kernel->lws[1] = kernel->lws[1] << 1;
        auto memory = std::make_unique<uint8_t[]>(sizeN * sizeN * sizeof(int));
        bufferDst->toHost(queue.get(), memory.get());
        queue->waitForExecutionFinish();
        //LOGI("0: %d, 1024: %d, 1024 * 1024 - 1: %d\n", memory[0], memory[sizeof(int) * 1024],
             //memory[(sizeof(int) * (1024 * 1024 - 1))]);

    }
    queue->nullLws = true;auto time = 0.0;
    for (int i = 0; i < iters; i++) {
        queue->runKernel(kernel.get());
        queue->waitForExecutionFinish();
        time += (double) (runtime->getKernelExecutionTime(kernel->event)) * 0.000000001;
    }
    time /= iters;
    output.append("Matrix mul lws: ");
    output.append("nullptr");
    output.append(" time: ");
    output.append(std::to_string(time));
    output.append("\n");
    queue->nullLws=false;
    return env->NewStringUTF(output.c_str());
}