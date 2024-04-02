#include "threadpool.h"

void task1() {
    std::cout << "Task 1 is running..." << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(5)); // 修改任务执行时间为5秒
    std::cout << "Task 1 is completed." << std::endl;
}

void task2() {
    std::cout << "Task 2 is running..." << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(8)); // 修改任务执行时间为8秒
    std::cout << "Task 2 is completed." << std::endl;
}

int main() {
    ThreadPool pool(2, 4);

    // 添加任务
    pool.addTask(task1);
    pool.addTask(task2);

    // 打印线程池状态
    std::cout << "Busy threads: " << pool.getBusyNumber() << std::endl;
    std::cout << "Alive threads: " << pool.getAliveNumber() << std::endl;

    // 等待一段时间，让任务执行完成
    std::this_thread::sleep_for(std::chrono::seconds(10)); // 延长等待时间

    // 打印线程池状态
    std::cout << "Busy threads: " << pool.getBusyNumber() << std::endl;
    std::cout << "Alive threads: " << pool.getAliveNumber() << std::endl;

    // 关闭线程池
    pool.shutdown();

    // 添加任务，此时线程池已关闭，任务不会被执行
    pool.addTask(task1);
    pool.addTask(task2);


    return 0;
}
