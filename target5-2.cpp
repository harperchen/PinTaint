#include <iostream>

class Test {
    private:
        int a;
        void foo(void);

    public:
        Test(int num) {
            this->a = num;
        }
        ~Test() {};

        void wrapper(void);
};

void Test::foo(void) {
    std::cout << this->a << std::endl;
}

void Test::wrapper(void) {
    this->foo();
}

int main()
{
    Test *ptr = new Test(1234);
    Test *old = ptr;
    

    ptr->wrapper();
    delete ptr;

    ptr->wrapper();
    ptr = new Test(1234);
    
    old->wrapper();
    
}
