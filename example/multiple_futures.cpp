#include "BS_thread_pool.hpp"

int square(const int i)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    return i * i;
};

int main()
{
    BS::thread_pool pool;
    BS::multi_future<int> mf1;
    BS::multi_future<int> mf2;
    for (int i = 0; i < 100; ++i)
        mf1.f.push_back(pool.submit(square, i));
    for (int i = 100; i < 200; ++i)
        mf2.f.push_back(pool.submit(square, i));
    /// ...
    /// Do some stuff while the first group of tasks executes...
    /// ...
    const std::vector<int> squares1 = mf1.get();
    std::cout << "Results from the first group:" << '\n';
    for (const int s : squares1)
        std::cout << s << ' ';
    /// ...
    /// Do other stuff while the second group of tasks executes...
    /// ...
    const std::vector<int> squares2 = mf2.get();
    std::cout << '\n' << "Results from the second group:" << '\n';
    for (const int s : squares2)
        std::cout << s << ' ';
}
