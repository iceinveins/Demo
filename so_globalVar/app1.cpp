#include "my_class.hpp"

static MyClass g_var(11);

void g_fun() {
  std::cout << __FILE__ << ":" << __LINE__ << std::endl;
}

int main() {
  std::cout << "----------" << std::endl;
  g_fun();
  std::cout << "----------" << std::endl;
  return 0;
}