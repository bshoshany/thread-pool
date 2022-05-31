#include "BS_thread_pool.hpp"

void sleep()
{
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
}

int main()
{
    BS::thread_pool pool;
    std::future<void> my_future = pool.submit(sleep);
    std::cout << "Waiting... ";
    my_future.wait();
    std::cout << "Done.";
}