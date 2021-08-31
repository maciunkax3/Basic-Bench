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
#include "kernels/double.h"
#include "kernels/half.h"
#include "kernels/int.h"
#include "kernels/float.h"
#include "kernels/bandwith.h"

extern OCL::Runtime *runtime;

double FlopsMesurement::runTest(DataTypes type, TestType testType) {
    std::unique_ptr<OCL::Kernel> kernel;
    if (testType == TestType::Flops) {
        auto program = getProgramSource(type);
        kernel = std::make_unique<OCL::Kernel>(runtime->context.get(), program,
                                               dataMap.find(type)->second.c_str(), "");
    } else {
        std::string options = "-D";
        options.append(dataMap.find(type)->second.c_str());
        options.append("=1");
        kernel = std::make_unique<OCL::Kernel>(runtime->context.get(), bandwithType, "readType",
                                               options.c_str());
    }
    kernel->createEvent();
    auto queue = std::make_unique<OCL::Queue>(runtime);
    size_t bufferSize = 0;
    auto size = runtime->maxWG * runtime->numComputeUnits * 256;
    switch (type) {
        case half1:
        case half2:
        case half4:
        case half8:
        case half16: {
            bufferSize = size * sizeof(uint16_t);
            if (testType == TestType::Flops) {
                float a = 1.0;
                kernel->setArg<float>(1, &a);
            }
            break;
        }
        case int1:
        case int2:
        case int4:
        case int8:
        case int16: {
            bufferSize = size * sizeof(int);
            if (testType == TestType::Flops) {
                int a = 1.0;
                kernel->setArg<int>(1, &a);
            }
            break;
        }
        case float1:
        case float2:
        case float4:
        case float8:
        case float16: {
            bufferSize = size * sizeof(float);
            if (testType == TestType::Flops) {
                float a = 1.0;
                kernel->setArg<float>(1, &a);
            }
            break;
            break;
        }
        case double1:
        case double2:
        case double4:
        case double8:
        case double16:
            if (runtime->supportDP) {
                bufferSize = size * sizeof(double);
                if (testType == TestType::Flops) {
                    double a = 1.0;
                    kernel->setArg<double>(1, &a);
                }
                break;
            } else {
                return 0.0;
            }
    };

    auto vecotrSize = 1 << ((int) (type) % 5);
    if (testType == TestType::Bandwith) {
        bufferSize *= 128;
    }
    auto buffer = std::make_unique<OCL::Buffer>(runtime->context.get(),
                                                bufferSize, nullptr);
    auto bufferSrc = std::make_unique<OCL::Buffer>(runtime->context.get(),
                                                   bufferSize, nullptr);
    if (testType == TestType::Bandwith) {
        kernel->setArg<cl_mem>(1, &bufferSrc->memObj);
        auto initMem = std::make_unique<char[]>(bufferSize);
        memset(initMem.get(), 12, bufferSize);
        bufferSrc->toDevice(queue.get(), initMem.get());
    }

    kernel->setArg<cl_mem>(0, &buffer->memObj);
    kernel->gws[0] = (size * (testType == TestType::Bandwith ? 128 : 1)) / vecotrSize;
    kernel->lws[0] = runtime->maxWG;
    kernel->dims = 1;
    kernel->setArg<cl_mem>(0, &buffer->memObj);
    double time = 0.0;
    size_t iterations = 128;

    queue->runKernel(kernel.get());
    queue->runKernel(kernel.get());
    queue->waitForExecutionFinish();
    for (int i = 0; i < iterations; i++) {
        queue->runKernel(kernel.get());
        queue->waitForExecutionFinish();
        time += (double) (runtime->getKernelExecutionTime(kernel->event)) * 0.000000001;
    }
    double result = 0.0;
    if (testType == TestType::Bandwith) {
        result = (iterations * bufferSize) / time;
    } else {
        uint32_t operationCount = 4096;
        result = (size/vecotrSize * iterations * operationCount) / time;
    }
    return result;
}

double FlopsMesurement::runLatency() {
    const char *program = R"===(
    __kernel void hello_opencl(__global char *dst){
        uint gid = get_global_id(0);
        dst[gid]++;
    }
    )===";
    auto kernel = std::make_unique<OCL::Kernel>(runtime->context.get(), program, "hello_opencl",
                                                "");
    auto queue = std::make_unique<OCL::Queue>(runtime);
    auto buffer = std::make_unique<OCL::Buffer>(runtime->context.get(), 100, nullptr);

    kernel->gws[0] = 32;
    kernel->lws[0] = 32;
    kernel->dims = 1;
    kernel->setArg<cl_mem>(0, &buffer->memObj);
    kernel->createEvent();

    queue->runKernel(kernel.get());
    queue->runKernel(kernel.get());
    queue->waitForExecutionFinish();

    auto iters = 4096;
    double time = 0.0;
    for(int i = 0; i < iters ; i++){
        queue->runKernel(kernel.get());
        queue->waitForExecutionFinish();
        time += (double) (runtime->getLatencyTime(kernel->event));
    }

    return time/(double)(iters);
}

const char *FlopsMesurement::getProgramSource(DataTypes type) {
    switch (type) {
        case half1:
            return half1Program;
        case half2:
            return half2Program;
        case half4:
            return half4Program;
        case half8:
            return half8Program;
        case half16:
            return half16Program;
        case int1:
            return int1Program;
        case int2:
            return int2Program;
        case int4:
            return int4Program;
        case int8:
            return int8Program;
        case int16:
            return int16Program;
        case float1:
            return float1Program;
        case float2:
            return float2Program;
        case float4:
            return float4Program;
        case float8:
            return float8Program;
        case float16:
            return float16Program;
        case double1:
            return double1Program;
        case double2:
            return double2Program;
        case double4:
            return double4Program;
        case double8:
            return double8Program;
        case double16:
            return double16Program;
    };
    return int1Program;
}

void FlopsMesurement::initMap() {
    dataMap.insert(std::make_pair(DataTypes::half1, "Half1"));
    dataMap.insert(std::make_pair(DataTypes::half2, "Half2"));
    dataMap.insert(std::make_pair(DataTypes::half4, "Half4"));
    dataMap.insert(std::make_pair(DataTypes::half8, "Half8"));
    dataMap.insert(std::make_pair(DataTypes::half16, "Half16"));


    dataMap.insert(std::make_pair(DataTypes::int1, "Int1"));
    dataMap.insert(std::make_pair(DataTypes::int2, "Int2"));
    dataMap.insert(std::make_pair(DataTypes::int4, "Int4"));
    dataMap.insert(std::make_pair(DataTypes::int8, "Int8"));
    dataMap.insert(std::make_pair(DataTypes::int16, "Int16"));


    dataMap.insert(std::make_pair(DataTypes::float1, "Float1"));
    dataMap.insert(std::make_pair(DataTypes::float2, "Float2"));
    dataMap.insert(std::make_pair(DataTypes::float4, "Float4"));
    dataMap.insert(std::make_pair(DataTypes::float8, "Float8"));
    dataMap.insert(std::make_pair(DataTypes::float16, "Float16"));


    dataMap.insert(std::make_pair(DataTypes::double1, "Double1"));
    dataMap.insert(std::make_pair(DataTypes::double2, "Double2"));
    dataMap.insert(std::make_pair(DataTypes::double4, "Double4"));
    dataMap.insert(std::make_pair(DataTypes::double8, "Double8"));
    dataMap.insert(std::make_pair(DataTypes::double16, "Double16"));
}

