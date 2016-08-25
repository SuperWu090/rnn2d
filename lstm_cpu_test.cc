#include <cstring>
#include <iostream>
#include <random>

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "lstm_cpu.h"

static std::default_random_engine RNG = std::default_random_engine();

namespace testing {

template <typename T>
bool FloatRelativeEq(const T r, const T a, const T t) {
  const T d = std::fabs(r - a);
  const T R = std::fabs(r);
  const T A = std::fabs(a);
  const T L = std::max(A, R);
  return (d <= L * t);
}

MATCHER_P(FloatRelativeEqPointwise, tol, "") {
  return FloatRelativeEq<float>(std::get<1>(arg), std::get<0>(arg),
                                tol);
}
MATCHER_P(DoubleRelativeEqPointwise, tol, "") {
  return FloatRelativeEq<double>(std::get<1>(arg), std::get<0>(arg),
                                 tol);
}

}  // namespace testing

using ::testing::Pointwise;
using ::testing::FloatRelativeEqPointwise;
using ::testing::DoubleRelativeEqPointwise;

template <typename T>
void init_uniform(T * x, int n, T a, T b) {
  std::uniform_real_distribution<T> u(a, b);
  for (int i = 0; i < n; ++i) {
    x[i] = u(RNG);
  }
}

template <typename T, typename M>
void test_forward(const M& matcher) {
  const int H = 2, W = 3, N = 2, K = 3, D = 2;
  const int S[N * 2] = {2, 3,
                        2, 3};
  const T I[] = {
    -0.4903562148187783, +0.1824233451472277, +0.1648769289158907,
    +0.0211014807881813, -0.5167319877830909, -0.4904234432259502,
    -0.3009907392710474, -0.257435054492243,  +0.6556094613571242,
    -0.7235756366794519, +0.1015826585395099, +0.0448628033201421,
    -0.0110695449719929, -0.9168062853832109, -0.7173157315329348,
    -0.1922856524005041, +0.9616520027095021, -0.4447104542421643,
    +0.2987322766057841, +0.4123685605545526, -0.8042764680265879,
    -0.952506181101044,  +0.3901241865906482, -0.324755975033316,
    +0.4916938220707712, -0.5322112391609797, -0.2965873710273401,
    +0.2344335374271702, -0.3633549153911784, -0.6171373058572165,
    +0.1772318029172082, +0.0387415319339457, -0.7633002874192214,
    +0.5268991823219358, +0.1630026964156088, -0.175247140125449
  };
  // Parameters (note, the 4 directions share the same weights)
  const T P[] = {
    // Bias
    +0.1238004398185371, +0.0597128206388473,
    +0.0577279198995233, +0.0906018192866844,
    +0.8898644614451579, +0.4405335324860431,
    +0.110218813029644,  +0.7272118639599343,
    +0.2552732163008662, -0.5255682299216178,
    // Input weights
    -0.8894968524202722, +0.8047916818659624,
    -0.4731911134058526, -0.498749711638939,
    -0.9677260043045435, +0.2089869867155434,
    +0.5842106522804797, +0.6790591473179441,
    +0.0144472685460779, -0.4531692352693608,
    -0.0036911227827381, -0.2948598353051857,
    -0.0695009409996545, +0.9102211356145096,
    -0.4996023285018472, -0.8439609185717205,
    -0.0598509697592831, +0.3107416064498483,
    +0.6002271722582584, +0.0776335750524577,
    +0.0216525793488624, +0.0020822871177588,
    -0.388239779430863,  +0.632153070278543,
    +0.1284795095002735, +0.3406997810968424,
    -0.8439956739717211, +0.3298467637270757,
    +0.512699070421641,  -0.7010828568231797,
    // Recurrent weights in y-dimension
    +0.5342824872171745, +0.9095258131525115,
    -0.2027778037119181, +0.4348038134502246,
    -0.3756837616821269, +0.6170942348020962,
    -0.1877499676046004, +0.6589453548733233,
    +0.2043691645707375, +0.9566995222055903,
    +0.2660246341562276, +0.9869254699274681,
    +0.1737244428648741, -0.01800945642773,
    +0.5129167970212474, +0.4680877830497863,
    -0.3851238268069799, +0.9385502852434926,
    -0.2053524728393175, +0.2586026968695132,
    // Recurrent weights in x-dimension
    -0.8296531535248959, -0.6323805326861465,
    -0.8270203720865337, -0.9004494374134326,
    +0.4912577112861503, +0.1293784564112055,
    -0.9423184210402249, -0.6455210265691766,
    +0.8723856227435929, -0.55500712430405,
    -0.6833540573349393, -0.0368474400460981,
    -0.3807644248172442, +0.1237544706481524,
    +0.9337586116748213, +0.2978454426480892,
    -0.6698477155959321, +0.2872717495888266,
    -0.346393410266852,  -0.2715765379246087
  };
  const T* Pd[] = {P, P, P, P};
  // Allocate space used for the internal states
  std::vector<T> Q(4 * H * W * N * 6 * D);
  T* Qd[4] = {
    Q.data(),
    Q.data() + 1 * H * W * N * 6 * D,
    Q.data() + 2 * H * W * N * 6 * D,
    Q.data() + 3 * H * W * N * 6 * D
  };
  // Output
  std::vector<T> O(H * W * N * 4 * D);
  lstm_2d_fwd_cpu< T, Linear<T>, Linear<T>, Linear<T> >(
      H, W, N, K, D, I, S, Pd, O.data(), Qd);
  const T expected_O[] = {
    // O(y=0,x=0,n=0)
    +0.1552421034,    -0.05649568186,
    +0.08922744475,   -0.1157763068,
    +0.1703495897,    -0.05987027915,
    +0.1211779002,    -0.07849787613,
    // O(y=0,x=0,n=1)
    +0.02810216138,   -0.1140128839,
    -0.0007228780851, +0.05824349101,
    +0.2109926397,    -1.370216511,
    +0.01750854822,   -2.125602853,
    // O(y=0,x=1,n=0)
    +0.03556559059,   +0.136169424,
    +0.06698174937,   +0.1893004634,
    +0.04631744976,   +0.01979038109,
    +0.01516661301,   +0.04484186813,
    // O(y=0,x=1,n=1)
    +0.4879741199,    -0.05067487128,
    +0.3373537726,    -0.06568050016,
    +0.4143301737,    +0.1982976303,
    +0.2951851036,    -0.06560264006,
    // O(y=0,x=2,n=0)
    -0.02452716927,   -0.3815267225,
    +0.06234558474,   -0.3686046391,
    +0.09829063469,   -0.3894028067,
    +0.04446251935,   -0.353278607,
    // O(y=0,x=2,n=1)
    +0.2649477513,    +0.07558265926,
    +0.03859640083,   +0.1667382504,
    +1.493953946,     +8.779175142,
    +0.04466547855,   +0.1967516258,
    // O(y=1,x=0,n=0)
    +0.01979253485,   +0.0114496853,
    -0.01876394682,   +0.06987545818,
    -0.009373682579,  +0.004032387207,
    -0.003567841123,  +0.01588096963,
    // O(y=1,x=0,n=1)
    +0.8238826829,    +0.1558329632,
    +1.188578091,     +0.1763634541,
    +0.9208777966,    +0.116774824,
    +1.03869074,      +0.1545284057,
    // O(y=1,x=1,n=0)
    +0.001213213334,  -0.3522602576,
    +0.02360241993,   -0.4008692726,
    +0.007836435713,  -0.4326151695,
    +0.003678691821,  -0.40631158,
    // O(y=1,x=1,n=1)
    +0.6919190613,    -0.1955717704,
    +0.08448594846,   -0.417938714,
    +1.119509789,     +0.6039107646,
    -0.02378770939,   -0.1457313092,
    // O(y=1,x=2,n=0)
    +0.0003332241053, -0.009296707826,
    +0.01030864164,   -0.003399693126,
    +0.02051543038,   -0.007027195103,
    -0.008120984055,  -0.01557363792,
    // O(y=1,x=2,n=1)
    +0.3801859616,    -0.1181500349,
    +0.02328252853,   -0.2048477451,
    +4.250911981,     -0.5692016098,
    +0.0130038029,    -0.02070659421
  };
  EXPECT_THAT(O, Pointwise(matcher, expected_O));
  print_O(H, W, N, D, O.data());
}

template <typename T, typename M>
void test_backward(const M& matcher) {
  const int H = 2, W = 3, N = 4, K = 3, D = 2;
  const int S[N * 2] = {2, 3,
                        1, 3,
                        2, 1,
                        2, 2};
  std::vector<T> I(H * W * N * 4 * K);
  init_uniform(I.data(), H * W * N * 4 * K, -0.1, 0.1);
}

TEST(lstm_cpu_test, forward) {
  test_forward<float>(FloatRelativeEqPointwise(1E-4));
  test_forward<double>(DoubleRelativeEqPointwise(1E-4));
}

TEST(lstm_cpu_test, backward) {
}