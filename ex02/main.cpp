#include "functions.hpp"
#include <iostream>
#include <cstdlib>
#include <ctime>

int main() {
    std::srand(static_cast<unsigned int>(std::time(NULL)));

    for (int i = 0; i < 6; i++) {
        Base* p = generate();

        std::cout << "--- instance " << i << " ---" << std::endl;
        std::cout << "identify(Base*): ";
        identify(p);
        std::cout << "identify(Base&): ";
        identify(*p);

        delete p;
    }
    return 0;
}