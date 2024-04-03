#include "threadpool.h"
#include <random>

class Philosopher {
public:
    Philosopher(int id, std::mutex& leftFork, std::mutex& rightFork) :
            id(id), leftFork(leftFork), rightFork(rightFork) {}

    void dine() {
        while (true) {
            think(); // 思考
            eat();   // 进餐
        }
    }

    void think() {
        std::cout << "Philosopher " << id << " is thinking..." << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(getRandomTime())); // 模拟思考一段时间
    }

    void eat() {
        std::lock(leftFork, rightFork); // 同时锁住左右两根筷子，避免死锁
        std::lock_guard<std::mutex> leftLock(leftFork, std::adopt_lock);
        std::lock_guard<std::mutex> rightLock(rightFork, std::adopt_lock);
        std::cout << "Philosopher " << id << " is eating..." << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(getRandomTime())); // 模拟进餐一段时间
    }

private:
    int id;
    std::mutex& leftFork;
    std::mutex& rightFork;

    int getRandomTime() {
        static std::mt19937 gen(std::random_device{}());
        std::uniform_int_distribution<int> dist(100, 1000); // 随机生成100到1000毫秒的时间
        return dist(gen);
    }
};

int main() {
    const int numPhilosophers = 5;
    std::vector<std::mutex> forks(numPhilosophers); // 五根筷子
    ThreadPool pool(5, 5); // 创建一个具有五个线程的线程池

    // 创建五个哲学家并添加到线程池中
    for (int i = 0; i < numPhilosophers; ++i) {
        pool.addTask([i, &forks]() {
            Philosopher philosopher(i, forks[i], forks[(i + 1) % numPhilosophers]);
            philosopher.dine();
        });
    }

    // 运行一段时间后关闭线程池
    std::this_thread::sleep_for(std::chrono::seconds(30)); // 运行30秒
    pool.shutdown();

    return 0;
}
