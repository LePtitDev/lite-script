#define DOCTEST_CONFIG_IMPLEMENT
#include "helpers.hpp"

#include <atomic>
#include <cstdlib>
#include <new>

#if defined(_MSC_VER) && defined(_DEBUG) && !defined(__SANITIZE_ADDRESS__)
#define LITESCRIPT_TESTS_CRT_LEAKS
#include <crtdbg.h>
#endif

////// ALLOCATIONS COUNTER //////

namespace {

    std::atomic<std::int64_t> live_allocations{ 0 };

    void * Allocate(std::size_t size) {
        void * p = std::malloc(size == 0 ? 1 : size);
        if (p == nullptr)
            throw std::bad_alloc();
        ++live_allocations;
        return p;
    }

    void Deallocate(void * p) noexcept {
        if (p == nullptr)
            return;
        --live_allocations;
        std::free(p);
    }

}

std::int64_t Tests::LiveAllocations() {
    return live_allocations.load();
}

void * operator new(std::size_t size) { return Allocate(size); }
void * operator new[](std::size_t size) { return Allocate(size); }
void * operator new(std::size_t size, const std::nothrow_t&) noexcept {
    try { return Allocate(size); } catch (...) { return nullptr; }
}
void * operator new[](std::size_t size, const std::nothrow_t&) noexcept {
    try { return Allocate(size); } catch (...) { return nullptr; }
}
void operator delete(void * p) noexcept { Deallocate(p); }
void operator delete[](void * p) noexcept { Deallocate(p); }
void operator delete(void * p, std::size_t) noexcept { Deallocate(p); }
void operator delete[](void * p, std::size_t) noexcept { Deallocate(p); }
void operator delete(void * p, const std::nothrow_t&) noexcept { Deallocate(p); }
void operator delete[](void * p, const std::nothrow_t&) noexcept { Deallocate(p); }

int main(int argc, char ** argv) {
#ifdef LITESCRIPT_TESTS_CRT_LEAKS
    // The debug heap of the CRT reports the leaks at the end of the execution
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
    _CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_FILE);
    _CrtSetReportFile(_CRT_WARN, _CRTDBG_FILE_STDERR);
#endif
    doctest::Context context(argc, argv);
    return context.run();
}
