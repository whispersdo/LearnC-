#include <atomic>
#include <thread>
using namespace std;

// µ¥
// https://zh.cppreference.com/w/cpp/atomic/memory_order

template<typename T, size_t Size>
class ringbuffer 
{
public:
    ringbuffer() : head_(0), tail_(0) {}

    bool push(const T& value)
    {
        size_t head = head_.load(std::memory_order_relaxed);
        size_t next_head = next(head);
        while (next_head == tail_.load(std::memory_order_acquire))
            this_thread::sleep_for(chrono::milliseconds(1));

        ring_[head] = value;
        head_.store(next_head, std::memory_order_release);

        return true;
    }
    bool pop(T& value)
    {
        size_t tail = tail_.load(std::memory_order_relaxed);
        while (tail == head_.load(std::memory_order_acquire))
        {
            this_thread::sleep_for(chrono::milliseconds(1));
        }

        value = ring_[tail];
        tail_.store(next(tail), std::memory_order_release);

        return true;
    }
private:
    size_t next(size_t current)
    {
        return (current + 1) % Size;
    }
    T ring_[Size];
    atomic<size_t> head_, tail_;
};

ringbuffer<int, 200> queue;

void producer1()
{
    for (int i = 0; i < 100; i++) {
        queue.push(i);
        this_thread::sleep_for(chrono::seconds(1));
    }
}

void producer2()
{
    for (int i = 100; i < 200; i++) {
        queue.push(i);
        this_thread::sleep_for(chrono::seconds(1));
    }
}

void consumer()
{
    for (int i = 0; i < 200; i++) {
        int x;
        queue.pop(x);
        printf("%d ", x);
        fflush(stdout);
    }
}

int main()
{
    std::thread th1(producer1);
    std::thread th2(producer2);
    std::thread th3(consumer);

    th1.join();
    //th2.join();
    th3.join();

    system("pause");
}