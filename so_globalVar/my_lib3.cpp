#include <iostream>
#include "my_class.hpp"

MyClass g_var(3);

void g_fun() {
  std::cout << __FILE__ << ":" << __LINE__ << std::endl;
}