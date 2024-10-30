#include "hip/hip_runtime.h"
#include <iostream>
#include <vector>

#define checkHIPErrors(err) __checkHIPErrors(err, __FILE__, __LINE__)
void __checkHIPErrors(hipError_t err, const char *file, const int line) {
  if (hipSuccess != err) {
    const char *errorStr = hipGetErrorString(err);

    std::cout << "checkHIPErrors() Driver API error = " << err << "\""
              << errorStr << "\""
              << " from file <" << file << "> line " << line << std::endl;
    throw std::runtime_error("failed to process a hip command");
  }
}

__global__ void add(const float *__restrict__ a, const float *__restrict__ b,
                    float *__restrict__ c) {
  int i = hipBlockDim_x * hipBlockIdx_x + hipThreadIdx_x;
  c[i] = a[i] + b[i];
}

__global__ void scale(const float *__restrict__ c, float *__restrict__ d,
                      float factor) {
  int i = hipBlockDim_x * hipBlockIdx_x + hipThreadIdx_x;
  d[i] = factor * c[i];
}

struct Config {
  size_t width{288};
  size_t height{256};
  size_t size{width * height};
};

int main(int argc, char *argv[]) {
  hipDeviceProp_t devProp;
  checkHIPErrors(hipGetDeviceProperties(&devProp, 0));
  std::cout << "System minor " << devProp.minor << std::endl;
  std::cout << "System major " << devProp.major << std::endl;
  std::cout << "Agent prop name " << devProp.name << std::endl;
  std::cout << "HIP Device prop succeeded " << std::endl;

  hipEvent_t startEvent, stopEvent;
  checkHIPErrors(hipEventCreate(&startEvent));
  checkHIPErrors(hipEventCreate(&stopEvent));

  Config config{};

  std::vector<float> hostA(config.size);
  std::vector<float> hostB(config.size);
  std::vector<float> hostC(config.size);
  std::vector<float> hostD(config.size);

  // initialize the input data
  for (size_t i = 0; i < config.size; i++) {
    hostA[i] = static_cast<float>(i);
    hostB[i] = static_cast<float>(i * 100.0f);
    hostC[i] = static_cast<float>(0.0f);
  }

  float *deviceA{nullptr};
  float *deviceB{nullptr};
  float *deviceC{nullptr};
  float *deviceD{nullptr};

  checkHIPErrors(hipMalloc((void **)&deviceA, config.size * sizeof(float)));
  checkHIPErrors(hipMalloc((void **)&deviceB, config.size * sizeof(float)));
  checkHIPErrors(hipMalloc((void **)&deviceC, config.size * sizeof(float)));
  checkHIPErrors(hipMalloc((void **)&deviceD, config.size * sizeof(float)));

  checkHIPErrors(hipMemcpy(deviceA, hostA.data(), config.size * sizeof(float),
                           hipMemcpyHostToDevice));
  checkHIPErrors(hipMemcpy(deviceB, hostB.data(), config.size * sizeof(float),
                           hipMemcpyHostToDevice));

  auto grid = dim3(config.width);
  auto block = dim3(config.height);

  constexpr float factor = 3.0f;
  add<<<grid, block>>>(deviceA, deviceB, deviceC);
  scale<<<grid, block>>>(deviceC, deviceD, factor);

  checkHIPErrors(hipMemcpy(hostD.data(), deviceD, config.size * sizeof(float),
                           hipMemcpyDeviceToHost));

  size_t errors = 0;
  for (size_t i = 0; i < config.size; i++) {
    float expected_result = factor * (hostA[i] + hostB[i]);
    if (hostD[i] != expected_result) {
      ++errors;
    }
  }

  if (errors != 0) {
    std::cout << "VERIFICATION FAILED: " << errors << " errors" << std::endl;
  } else {
    std::cout << "VERIFICATION PASSED" << std::endl;
  }

  checkHIPErrors(hipFree(deviceA));
  checkHIPErrors(hipFree(deviceB));
  checkHIPErrors(hipFree(deviceC));
  checkHIPErrors(hipFree(deviceD));

  return 0;
}