/*
 * Valarray_PhaseA_unittests.cpp
 * EPL - Spring 2015
 */

#include <chrono>
#include <complex>
#include <cstdint>
#include <future>
#include <iostream>
#include <stdexcept>
#define PHASE_B
#define PHASE_A
#include "Valarray.h"
#include "gtest/gtest.h"


#include "InstanceCounter.h"

int InstanceCounter::counter = 0;



using std::cout;
using std::endl;
using std::string;
using std::complex;

using namespace epl;

class Foo{};
template <typename T1, typename T2>
void operator+(T1, T2) {
    cout << "passed\n";
}


template <typename X, typename Y>
bool match(X x, Y y) {
    double d = x - y;
    if (d < 0) { d = -d; }
    return d < 1.0e-9; // not really machine epsilon, but close enough
}



/*********************************************************************/
// Phase B Tests
/*********************************************************************/

#if defined(PHASE_B1_0) | defined(PHASE_B)
TEST(PhaseB1, Sqrt) {
    valarray<int> v1(10);
    valarray<int> v4 = 4 + v1;
    
    valarray<float> v5 = v4.sqrt();
//        valarray<double> v5 = v4.sqrt();
    for (uint64_t i = 0; i<10; i++) {
        EXPECT_EQ(2.0, v5[i]);
    }
}
#endif

#if defined(PHASE_B1_1) | defined(PHASE_B)
TEST(PhaseB1, Apply) {
    valarray<int> v1(10);
    valarray<int> v4 = 4 + v1;
    
    valarray<int> v7 = v4.apply(std::negate<int>());
    
    EXPECT_EQ(10, v7.size());
    
    for (uint64_t i = 0; i<10; i++) {
        EXPECT_EQ(-4, v7[i]);
    }
}
#endif

#if defined(PHASE_B1_2) | defined(PHASE_B)
TEST(PhaseB1, Accumulate) {
    valarray<int> v1{1, 2, 3, 4, 5};
    auto sum = v1.accumulate(std::plus<int>());
    auto product = v1.accumulate(std::multiplies<int>());
    EXPECT_EQ(15, sum);
    EXPECT_EQ(120, product);
}
#endif

#if defined(PHASE_B1_3) | defined(PHASE_B)
TEST(PhaseB1, Lazy) {
    // lazy evaluation test
    valarray <double> v1, v2, v3, v4;
    for (int i = 0; i<10; ++i) {
        v1.push_back(1.0);
        v2.push_back(1.0);
        v3.push_back(1.0);
        v4.push_back(1.0);
    }
    int cnt = InstanceCounter::counter;
    v1 + v2 - (v3 * v4);
    EXPECT_EQ(cnt, InstanceCounter::counter);
    
    valarray<double> ans(10);
    ans = v1 + v2 - (v3*v4);
    EXPECT_TRUE(match(ans[3], (v1[3] + v2[3] - (v3[3] * v4[3]))));
}
#endif


/*********************************************************************/
// Phase A Tests
/*********************************************************************/

#if defined(PHASE_A0_1) | defined(PHASE_A)
TEST(PhaseA, BracketOp) {
    valarray<complex<double>> x(10);
    for (int i = 0; i < 10; ++i)
        x[i] = complex<double>(i, i);
    
    //passes
    auto y = x + x;
    for (int i = 0; i<10; ++i) {
        complex<double> ans(i, i);
        ans += ans;
        EXPECT_TRUE(match(y[i].real(), ans.real()));
    }
    
    //fails
    x = y;
    int i = 0;
    for (auto yi : x) {
        complex<double> ans(i, i);
        ++i;
        ans += ans;
        cout<<yi<<endl;
        EXPECT_TRUE(match(yi.real(), ans.real()));
    }
}
#endif

#if defined(PHASE_A0_0) | defined(PHASE_A)
TEST(PhaseA, Operators) {
    valarray<int> x(10);
    valarray<int> y(20);
    x = -((2 * (1 + x - y + 1)) / 1);
    
    for (int i = 0; i < 10; i++) {
        EXPECT_EQ(-4, x[i]);
    }
}
#endif

#if defined(PHASE_A1_0) | defined(PHASE_A)
TEST(PhaseA1, Complex) {
    valarray<complex<float>> x(10);
    valarray<double> y = { 1.5, 2.5, 3.5 };
    
    for (int i = 0; i < 10; i++)
        x[i] = complex<float>(0.0, 0.0);
    
    auto z = y + x;
    auto r = y + y + z;
    EXPECT_EQ(r[0], complex<double>(4.5, 0));
    EXPECT_EQ(r[1], complex<double>(7.5, 0));
    EXPECT_EQ(r[2], complex<double>(10.5, 0));
}
#endif

TEST(PhaseMY, OSTREAM) {
    valarray<complex<float>> x(10);
    valarray<double> y = { 1.5, 2.5, 3.5 };
    
    for (int i = 0; i < 10; i++)
        x[i] = complex<float>(0.0, 0.0);
    
    auto z = y + x;
    auto r = y + y + z+1;
    std::cout <<r<<std::endl;
    std::cout<<x+2.0<<std::endl;
    EXPECT_EQ(3, r.size());
    EXPECT_EQ(r[0], complex<double>(5.5, 0));
    EXPECT_EQ(r[1], complex<double>(8.5, 0));
    EXPECT_EQ(r[2], complex<double>(11.5, 0));
}


TEST(PhaseMY, vectorop){
    valarray<int> x(10);
    valarray<double> y{};
    valarray<complex<double>> z(10);
    auto p=z+z+1-2;
    valarray<complex<double>> v=z+1;
    valarray<complex<double>> vv{p};
    y.push_back(3);
    x.push_back(2);
    x.push_back(2.0);
    x.pop_back();
    x.pop_back();
    x.pop_back();
    EXPECT_EQ(9, x.size());
    EXPECT_EQ(1, y.size());
    cout<<v<<endl;
    cout<<vv<<endl;
}

TEST(PhaseMY, assign){
    valarray<complex<float>> x(20);
    valarray<int> y(10);
    valarray<double> z(5);
    x=y+z-5;
    EXPECT_EQ(x.size(), 20);
    x=0.5;
    EXPECT_EQ(x.size(), 20);
    cout<<x<<endl;
    valarray<complex<double>> kk={1};
    EXPECT_EQ(kk.size(), 1);
}

TEST(PhaseMY, Lazy) {
    // lazy evaluation test
    valarray <double> v1, v2, v3, v4;
    for (int i = 0; i<10; ++i) {
        v1.push_back(1.0);
        v2.push_back(1.0);
        v3.push_back(1.0);
        v4.push_back(1.0);
    }
    int cnt = InstanceCounter::counter;
    ((v1 + v2 - (v3 * v4)).sqrt()).apply(std::negate<int>());
    EXPECT_EQ(cnt, InstanceCounter::counter);
    
    valarray<double> ans(10);
    ans =((v1 + v2 - (v3 * v4)).sqrt()).apply(std::negate<int>());
    EXPECT_TRUE(match(-ans[3], (v1[3] + v2[3] - (v3[3] * v4[3]))));
    ++v1[1];
    ++v2[1];
    --v4[1];
    ans=((v1 + v2 - (v3 * v4)).sqrt()).apply(std::negate<int>());
    EXPECT_EQ(ans[1], -2.0);
}

TEST(PhaseMY, Accumulate) {
    valarray<int> v1{1, 2, 3, 4, 5};
    auto sum = v1.accumulate(std::plus<double>());
    auto product = v1.accumulate(std::multiplies<float>());
    EXPECT_EQ(15, sum);
    sum=v1.sum();
    EXPECT_EQ(15, sum);
    EXPECT_EQ(120, product);
}

TEST(PhaseMY, iterator){
    valarray<int> x(10);
    valarray<double> y(10);
    y=2;
    for (auto const & p : x + y) {
        cout << p; // p should be a double
        EXPECT_EQ(2.0, p);
    }
    x=1;
    auto p = (x + y).begin();
    auto q = (x + y).end();
    while (p != q) {
        cout << *p;
        EXPECT_EQ(3.0, *p);
        ++p; }

}


TEST(PhaseC, op){
    float x;
    double y;
    valarray<double> v;
    std::complex<float> fx;
    std::complex<double> fy;
    auto z=x+v;
    auto zz=y+v;
    auto fz=fx+v;
    auto fzz=fy+v;
    
}


TEST(PhaseC, Foo){
    Foo f1,f2;
    valarray<int> x;
    f1+f2;
    f1+x;
    x+f1;
}
template<typename T>
struct addval{
    T val;
    addval(T t): val(t){
    }
    using result_type=T;
    result_type operator()(T tt){
        return tt+val;
    }
    const result_type operator()(T tt) const{
        return tt+val;
    }
};

template<typename T>
struct mulval{
    T val;
    mulval(T t): val(t){
    }
    using result_type=T;
    result_type operator()(T tt){
        return tt*val;
    }
    const result_type operator()(T tt) const{
        return tt*val;
    }
};

TEST(PhaseC, structop){
    valarray<int> x{1,2,3,4,5};
    valarray<float> y{1,2,3,4,5};
    auto addv=addval<double>(1.5);
    auto mulv=mulval<int>(2);
    auto z=(x+y).apply(addv);
    auto w=(x+y-x).apply(mulv);
    for(int i=1;i<=5;++i){
        EXPECT_EQ(i*2+1.5, z[i-1]);
    }
    for(int i=1;i<=5;++i){
        EXPECT_EQ(i*2, w[i-1]);
    }
}



int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    auto out = RUN_ALL_TESTS();
    
#ifdef _MSC_VER
    system("pause");
#endif
    
    return out;
}
