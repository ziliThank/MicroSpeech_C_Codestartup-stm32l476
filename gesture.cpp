// #define STM32L476xx

// #include "stm33l476/device/stm32l4xx.h"
// #include "stm32l476/device/system_stm32l4xx.h"
// #include "stm32l476/device/system_stm32l4xx.c"
// #include "stm32l476/device/stm32l4xx_ll_utils.h"
// #include "stm32l476/device/stm32l4xx_ll_utils.c"

#include <array>
#include <cmath>
// #include <cstdio>
// #include <cstdlib>

// input 1xN, weights MxN, bias 1xM, output 1xM
template <int N, int M>
void fc_layer(const float input[N], const float weights[M * N],
              const float bias[M], float output[M]) {
  for (int j = 0; j < M; j++) {
    output[j] = bias[j];
    for (int k = 0; k < N; k++) {
      output[j] += input[k] * weights[j * N + k];
    }
  }
}

template <int N> void softmax_layer(float input[N]) {
  float softmax_sum = 0;
  for (int i = 0; i < N; i++) {
    float t = expf(input[i]);
    softmax_sum += t;
    input[i] = t;
  }

  for (int i = 0; i < N; i++) {
    input[i] /= softmax_sum;
  }
}

template <int N> void relu_layer(float input[N]) {
  for (int i = 0; i < N; i++) {
    input[i] = std::max(input[i], 0.0f);
  }
}

template <int N> void print_array(const float input[N]) {
  // for (int i = 0; i < N; i++) {
  //   printf("%f, ", input[i]);
  // }
  // printf("\n");
}

#include "model.hpp"
float input[R1] = {};
float output1[C1] = {};
float output2[C2] = {};
float output3[C3] = {};
extern "C" int Main(void) {
  // LL_SetSystemCoreClock(80000000);
  // read input data
  // FILE *fptr;
  // if ((fptr = fopen("params/serving_default_dense_input:0", "r")) == NULL) {
  //   // printf("Error! opening file");
  //   exit(1);
  // }
  // for (int i = 0; i < C1; i++) {
  //   fscanf(fptr, "%f ", &input[i]);
  // }
  // fclose(fptr);

  // printf("input: ");
  print_array<10>(input);

  // printf("weight1: ");
  print_array<10>(weight1);

  fc_layer<R1, C1>(input, weight1, bias1, output1);
  relu_layer<C1>(output1);
  // printf("output1: ");
  print_array<C1>(output1);

  // printf("weight2: ");
  print_array<10>(weight2);

  fc_layer<R2, C2>(output1, weight2, bias2, output2);
  relu_layer<C2>(output2);
  // printf("output2: ");
  print_array<C2>(output2);

  fc_layer<R3, C3>(output2, weight3, bias3, output3);
  softmax_layer<C3>(output3);
  // printf("output3: ");
  print_array<C3>(output3);

  return 0;
}
