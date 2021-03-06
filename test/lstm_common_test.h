#include <vector>

#define MAX_ERROR 1E-5

static const int H = 3, W = 4, N = 2, K = 3, D = 2;
static const std::vector<int> S{2, 4, 3, 3};

// LSTM input image.
template <typename T>
static const std::vector<T>& I() {
  static const std::vector<T> I_{
    // I(y = 0, x = 0)
    0.44,  0.47,  0.64,
    0.67,  0.67,  0.09,
    // I(y = 0, x = 1)
    0.83,  0.21,  0.36,
    0.87,  0.70,  0.88,
    // I(y = 0, x = 2)
    0.88,  0.12,  0.58,
    0.65,  0.39,  0.87,
    // I(y = 0, x = 3)
    0.46,  0.88,  0.81,
    0.00,  0.00,  0.00,
    // I(y = 1, x = 0)
    0.72,  0.09,  0.20,
    0.80,  0.69,  0.79,
    // I(y = 1, x = 1)
    0.47,  0.64,  0.82,
    0.99,  0.88,  0.49,
    // I(y = 1, x = 2)
    0.29,  0.19,  0.19,
    0.14,  0.39,  0.32,
    // I(y = 1, x = 3)
    0.65,  0.09,  0.57,
    0.00,  0.00,  0.00,
    // I(y = 2, x = 0)
    0.00,  0.00,  0.00,
    0.55,  0.28,  0.34,
    // I(y = 2, x = 1)
    0.00,  0.00,  0.00,
    0.53,  0.05,  0.38,
    // I(y = 2, x = 2)
    0.00,  0.00,  0.00,
    0.42,  0.58,  0.31,
    // I(y = 2, x = 3)
    0.00,  0.00,  0.00,
    0.00,  0.00,  0.00
  };
  return I_;
}


// Gradients of the loss function w.r.t. the LSTM output image.
template <typename T>
static const std::vector<T>& dO() {
  static const std::vector<T> dO_{
    // dO(y = 0, x = 0)
     0.30, -0.73,  0.83,  0.07,  0.21, -0.92,  0.08,  0.43,
     0.46,  0.00,  0.51,  0.38,  0.89,  0.74,  0.98,  0.87,
    // dO(y = 0, x = 1)
     0.96,  0.83,  0.26,  0.78,  0.32, -0.70, -0.97,  0.44,
     0.59,  0.57,  0.50,  0.45,  0.04,  0.57, -0.93,  0.91,
    // dO(y = 0, x = 2)
     0.17, -0.35,  0.66, -0.20,  0.45,  0.57,  0.79,  0.36,
     0.62, -0.04,  0.77,  0.30,  0.87,  0.98,  0.35, -0.73,
    // dO(y = 0, x = 3)
     0.90, -0.55,  0.26, -0.22, -0.76,  0.79, -0.98,  0.90,
     0.00,  0.00,  0.00,  0.00,  0.00,  0.00,  0.00,  0.00,
    // dO(y = 1, x = 0)
     0.75, -0.13, -0.71, -0.94,  0.06,  0.45,  0.30, -0.15,
    -0.41, -0.38, -0.53,  0.43,  0.82,  0.07,  0.92,  0.65,
    // dO(y = 1, x = 1)
    -0.41,  0.66,  0.92,  0.78,  0.91,  0.31,  0.93,  0.50,
     0.46,  0.93,  0.33,  0.12,  0.96,  0.80,  0.66,  0.72,
    // dO(y = 1, x = 2)
     0.10,  0.90,  0.27, -0.56,  0.65, -0.64, -0.69,  0.88,
    -0.34,  0.79, -0.03,  0.60,  0.37,  0.43,  0.83, -0.88,
    // dO(y = 1, x = 3)
    -0.70, -0.20, -0.06, -0.28,  0.06,  0.66,  0.92, -0.92,
     0.00,  0.00,  0.00,  0.00,  0.00,  0.00,  0.00,  0.00,
    // dO(y = 2, x = 0)
     0.00,  0.00,  0.00,  0.00,  0.00,  0.00,  0.00,  0.00,
     0.01,  0.05,  0.70,  0.12,  0.90, -0.74, -0.54,  0.78,
    // dO(y = 2, x = 1)
     0.00,  0.00,  0.00,  0.00,  0.00,  0.00,  0.00,  0.00,
     0.67,  0.10,  0.57,  0.91,  0.89, -0.03, -0.78,  0.82,
    // dO(y = 2, x = 2)
     0.00,  0.00,  0.00,  0.00,  0.00,  0.00,  0.00,  0.00,
     0.03,  0.69,  0.40,  0.36,  0.58,  0.49, -0.55,  0.24,
    // dO(y = 2, x = 3)
     0.00,  0.00,  0.00,  0.00,  0.00,  0.00,  0.00,  0.00,
     0.00,  0.00,  0.00,  0.00,  0.00,  0.00,  0.00,  0.00
  };
  return dO_;
}

// LSTM parameter
template <typename T>
static const std::vector<T>& P() {
  static const std::vector<T> P_{
    // bias, z = 0
     0.74, -0.18, -0.09,  0.28,  0.42, -0.01, -0.47,  0.40,  0.21,  0.70,
    // input weight, z = 0
     0.62, -0.52, -0.07,  0.31, -0.02, -0.58,  0.12,  0.49,  0.27, -1.00,
     0.38,  0.14, -0.57,  0.86,  0.27, -0.77,  0.87,  0.30,  0.21, -0.02,
    -0.38,  0.63,  0.23,  0.95, -0.18,  0.74,  0.48, -0.50,  0.55, -0.86,
    // recurrent-y weight, z = 0
    -0.65,  0.89,  0.97, -0.57, -0.68, -0.89,  0.04,  0.38,  0.82,  0.65,
     0.25,  0.56,  0.11, -0.98, -0.73,  0.51, -0.47, -0.49,  0.74,  0.48,
    // recurrent-x weight, z = 0
    -0.98, -0.31,  0.01,  0.40, -0.56,  0.17, -0.34,  0.11, -0.09, -0.15,
     0.67, -0.61,  0.50,  0.58,  0.45,  0.98,  0.99, -0.82, -0.08, -0.57,
    // bias, z = 1
    -0.16, -0.32, -0.94,  0.96, -0.53,  0.27,  0.31,  0.00,  0.80, -0.22,
    // input weight, z = 1
    -0.59, -0.42,  0.93, -0.64, -0.90, -0.14, -0.57,  0.04, -0.89, -0.98,
    -0.49, -0.20, -0.68,  0.82,  0.28, -0.62, -0.81,  0.74, -0.58,  0.15,
     0.84,  0.88, -0.23, -0.70, -0.76,  0.25, -0.98, -0.97, -0.06,  0.07,
    // recurrent-y weight, z = 1
     0.81, -0.71, -0.33, -0.65, -0.61,  0.37, -0.27, -0.59,  0.51,  0.31,
    -0.54,  0.78,  0.08, -0.97, -0.69, -0.91,  0.38, -0.73,  0.73,  0.99,
    // recurrent-x weight, z = 1
    -0.17,  0.77, -0.59, -0.07,  0.74,  0.49, -0.11, -0.27, -0.72,  0.05,
     0.86,  0.28, -0.37, -0.84,  0.06,  0.64, -0.06, -0.76,  0.16,  0.91,
    // bias, z = 2
     0.43,  0.48,  0.86, -0.77,  0.41,  0.17, -0.15, -0.52, -0.51, -0.31,
    // input weight, z = 2
    -0.87,  0.12, -0.60, -0.28, -0.81, -0.05, -0.28,  0.54,  0.94,  0.80,
    -0.33, -0.39, -0.86, -0.04, -0.96,  0.95,  0.39, -0.14,  0.21,  0.09,
    -0.25,  0.84, -0.84,  0.52,  0.57,  0.49,  0.10, -0.75,  0.88,  0.21,
    // recurrent-y weight, z = 2
     0.67, -0.39, -0.15, -0.03, -0.56, -0.66,  0.62, -0.12, -0.67,  0.33,
    -0.64, -1.00, -0.66,  0.97,  0.26,  0.81, -0.20,  0.90,  0.36,  0.89,
    // recurrent-x weight, z = 2
     0.95, -0.49,  0.36,  0.84, -0.09, -0.07,  0.23, -0.13,  0.60,  0.47,
    -0.28,  0.99, -0.13, -0.87, -0.42, -0.19,  0.20,  0.16,  0.83, -0.36,
    // bias, z = 3
     0.69,  0.63,  0.92, -0.05,  0.97, -0.06, -1.00,  0.13,  0.78, -0.64,
    // input weight, z = 3
     0.18,  0.17,  0.89, -0.17,  0.61,  0.04,  0.60,  0.21, -0.30, -0.69,
    -0.87, -0.29,  0.84,  0.52, -0.21, -0.59, -0.82, -0.60,  0.82, -0.89,
     0.66,  0.11, -0.07,  0.29,  0.18, -0.56,  0.25, -0.76, -0.33, -0.97,
    // recurrent-y weight, z = 3
     0.29,  0.12, -0.65,  0.20, -0.09,  0.68,  0.16, -0.64,  0.76, -0.75,
    -0.33,  0.03, -0.65,  0.14, -0.70, -0.71, -0.67,  0.46, -0.83, -0.16,
    // recurrent-x weight, z = 3
     0.64, -0.75, -0.68,  0.70, -0.86, -0.72, -0.80, -0.32, -0.78,  0.22,
    -0.17,  0.35, -0.39,  0.41, -0.95, -1.00,  0.36,  0.81,  0.39, -0.96
  };
  return P_;
}

// Useful union to interpret float/double as hex values.
// e.g. HexIEEE<uint32_t,float>((uint32_t)0x41798f61) ~~ 1.5011077
template <typename H, typename F>
union HexIEEE {
  H h; F f;
  explicit HexIEEE(const H& h) : h(h) {}
  explicit HexIEEE(const F& f) : f(f) {}
  inline operator H() const { return h; }
  inline operator F() const { return f; }
};

// Sum of all elements of the output tensor (computed with Theano).
template <typename T>
static inline T expected_sum_O();

// Sum of all elements of the gradient of the loss w.r.t. the input
// (computed with Theano).
template <typename T>
static inline T expected_sum_dI();

// Sum of all elements of the gradient of the loss w.r.t. the parameters
// (computed with Theano).
template <typename T>
static inline T expected_sum_dP();

template <>
inline float expected_sum_O<float>() {
  static const HexIEEE<uint32_t,float> v((uint32_t)0x41798f61);
  return v;
}

template <>
inline double expected_sum_O<double>() {
  static const HexIEEE<uint64_t,double> v((uint64_t)0x402f31ec0be9fdb5L);
  return v;
}

template <>
inline float expected_sum_dI<float>() {
  static const HexIEEE<uint32_t,float> v((uint32_t)0x3fc0244c);
  return v;
}

template <>
inline double expected_sum_dI<double>() {
  static const HexIEEE<uint64_t,double> v((uint64_t)0x3ff80489608c4a3bL);
  return v.f;
}

template <>
inline float expected_sum_dP<float>() {
  static const HexIEEE<uint32_t,float> v((uint32_t)0x42122ff7);
  return v;
}

template <>
inline double expected_sum_dP<double>() {
  static const HexIEEE<uint64_t,double> v((uint64_t)0x404245fef49673f5L);
  return v;
}
