#include <iostream>
#include <unistd.h>
#include <dlfcn.h>
#include "my_class.hpp"

static MyClass g_var(22);

void g_fun() {
  std::cout << __FILE__ << ":" << __LINE__ << std::endl;
}

int main() {
  dlopen("./libmylib1.so", RTLD_NOW | RTLD_LOCAL);
  dlopen("./libmylib2.so", RTLD_NOW | RTLD_LOCAL);
  dlopen("./libmylib3.so", RTLD_NOW | RTLD_LOCAL);
  std::cout << "----------" << std::endl;
  g_fun();
  std::cout << "----------" << std::endl;
  return 0;
}