//#include <gtest/gtest.h>
//
//#include "Registry.h"
//
//using namespace std;
//
//template<typename T>
//class Base {
//public:
//  virtual ~Base() = default;
//
//  Base(T a, T b, T c, optional<any> parameters = nullopt) : a_(a), b_(b), c_(c), parameters_(parameters) {}
//
//  template <typename T, typename... TArgs> void setParameters(TArgs &&... args) {
//    set(T{std::forward<TArgs>(args)...});
//  }
//
//  T a_;
//  T b_;
//  T c_;
//  optional<any> parameters_;
//
//private:
//  virtual void set(std::any) {};
//};
//
//struct Der1 : Base<double> {
//  struct ParametersPack {
//    int d11;
//    int d12;
//    int d13;
//  };
//
//  Der1(double b, double c, ParametersPack parameters)
//      : Base(1, b, c, parameters), der1Parameters_(any_cast<ParametersPack>(parameters_)) {
//  }
//
//  ParametersPack& der1Parameters_;
//
//public:
//  void set(any parameters) override {
//    parameters_ = any_cast<ParametersPack>(parameters);
//  }
//};
//
//struct Der2 : Base<double> {
//  Der2(double b, double c) : Base(2, b, c) {}
//
//  struct ParametersPack {
//    int d21;
//    int d22;
//  };
//
//  ParametersPack parameters_;
//
//private:
//
//  void set(any parameters) override {
//    parameters_ = any_cast<ParametersPack>(parameters);
//  }
//};
//
//TEST(registry_tests, RegistryCtorDoesntThrow) {
//  Registry<Base<double>, string, int, int, int, optional<any>> registry;
//
//  registry.registerFactory("Der1",
//                           [](double a, double b, double c, optional<any> parameters) { 
//    auto der1 = make_unique<Der1>(b, c, parameters);
//
//    return der1;
//  });
//
//  auto d1 = registry.construct("Der1", 1, 2, 3, );
//  d1->setParameters<Der1::ParametersPack>(32, 64, 128);
//}
