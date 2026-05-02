#include "CircularQueue.h"
#include <stdexcept>

CircularQueue::CircularQueue(int capacity) 
: capacity(capacity), head(0), tail(0), count(0), sum(0.0) { 
    buffer = new double[capacity];
}

CircularQueue::~CircularQueue() { 
    delete[] buffer;
}

void CircularQueue::enqueue(double value) {
    if(isFull()) { 
        sum -= buffer[head];            // overwrites oldest element 
        head = (head + 1) % capacity;   // shifts oldest element to next
    } else { 
        count ++; 
    }
    sum += value; 
    buffer[tail] = value; 
    tail = (tail + 1) % capacity; 
}

double CircularQueue::dequeue() { 
    if(isEmpty()) { 
        throw std::runtime_error("Queue is empty");
    }
    double value = buffer[head];    // return 
    sum -= value;                   // update running sum 

    head = (head + 1) % capacity; 
    count--; 
    return value; 
}

double CircularQueue::peek() const {
    if(isEmpty()) throw std::runtime_error("Queue is empty");
    return buffer[head]; 
}

double CircularQueue::getAverage() const {
    if(isEmpty()) return 0.0; 
    return sum / count; 
}

bool CircularQueue::isFull() const { 
    return count == capacity; 
}

bool CircularQueue::isEmpty() const { 
    return count == 0;
}

int CircularQueue::getCount() const { 
    return count; 
}

int CircularQueue::getCapacity() const { 
    return capacity; 
}