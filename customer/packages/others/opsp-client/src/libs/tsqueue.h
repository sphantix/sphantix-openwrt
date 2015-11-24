#ifndef __TSQUEUE_H__
#define __TSQUEUE_H__

#include <memory>
#include <mutex>
#include <condition_variable>

template<typename T>
class tsqueue
{
    private:
        struct node
        {
            std::shared_ptr<T> data;
            std::unique_ptr<node> next;
        };

        std::mutex head_mutex;
        std::mutex tail_mutex;
        std::unique_ptr<node> head;
        node* tail;
        std::condition_variable data_cond;

    public:
        tsqueue():head(new node),tail(head.get()){}
        tsqueue(const tsqueue& other)=delete;
        tsqueue& operator=(const tsqueue& other)=delete;

        void wait_and_pop(T& value)
        {
            wait_pop_head(value);
        }
        void empty()
        {
            std::lock_guard<std::mutex> head_lock(head_mutex);
            return (head == get_tail());
        }
        void push(T new_value)
        {
            std::shared_ptr<T> new_data(std::make_shared<T>(std::move(new_value)));
            std::unique_ptr<node> p(new node);
            {
                std::lock_guard<std::mutex> tail_lock(tail_mutex);
                tail->data = new_data;
                node* const new_tail = p.get();
                tail->next = std::move(p);
                tail = new_tail;
            }
            data_cond.notify_one();
        }

    private:
        node* get_tail()
        {
            std::lock_guard<std::mutex> tail_lock(tail_mutex);
            return tail;
        }
        void pop_head()
        {
            head = std::move(head->next);
        }
        std::unique_lock<std::mutex> wait_for_data()
        {
            std::unique_lock<std::mutex> head_lock(head_mutex);
            data_cond.wait(head_lock,[&]{return head.get() != get_tail();});
            return std::move(head_lock);
        }
        void wait_pop_head(T& value)
        {
            std::unique_lock<std::mutex> head_lock(wait_for_data());
            value = std::move(*head->data);
            pop_head();
        }
};

#endif /* __TSQUEUE_H__ */
