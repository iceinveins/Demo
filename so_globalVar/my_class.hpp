#include <iostream>

void g_fun();

class MyClass {
 public:
   MyClass(int a) : a_(a) {
     std::cout << "Construct! " << a_ << " @ " << this << std::endl;
     g_fun();
   }

   ~MyClass() {
     std::cout << "Destruct! " << a_ << " @ " << this << std::endl;
   }

 private:
   int a_;
};