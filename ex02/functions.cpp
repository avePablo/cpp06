#include "functions.hpp"
#include "A.hpp"
#include "B.hpp"
#include "C.hpp"
#include <cstdlib>
#include <iostream>

/*
** Randomly instantiates A, B or C and returns it as a Base pointer.
*/
Base* generate(void) {
    int choice = std::rand() % 3;

    switch (choice) {
        case 0:
            return new A();
        case 1:
            return new B();
        default:
            return new C();
    }
}

/*
** Pointer version: a failed dynamic_cast to a pointer type simply
** yields NULL, so no exception handling is required here.
*/
void identify(Base* p) {
    if (dynamic_cast<A*>(p))
        std::cout << "A" << std::endl;
    else if (dynamic_cast<B*>(p))
        std::cout << "B" << std::endl;
    else if (dynamic_cast<C*>(p))
        std::cout << "C" << std::endl;
    else
        std::cout << "Unknown" << std::endl;
}

/*
** Reference version: using a pointer here is forbidden, and so is the
** <typeinfo> header (which rules out naming std::bad_cast or using
** typeid). A dynamic_cast to a reference type throws when it fails,
** so a generic catch(...) block -- which needs no type name at all --
** is enough to detect a mismatch without ever including <typeinfo>.
*/
void identify(Base& p) {
    try {
        (void)dynamic_cast<A&>(p);
        std::cout << "A" << std::endl;
        return;
    } catch (...) {}

    try {
        (void)dynamic_cast<B&>(p);
        std::cout << "B" << std::endl;
        return;
    } catch (...) {}

    try {
        (void)dynamic_cast<C&>(p);
        std::cout << "C" << std::endl;
        return;
    } catch (...) {}

    std::cout << "Unknown" << std::endl;
}