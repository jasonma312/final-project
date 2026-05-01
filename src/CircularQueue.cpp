#include "CircularQueue.h"

CircularQueue::CircularQueue(int capacity)
    : capacity(capacity), head(0), tail(0), count(0) {

    buffer = new double[capacity];
}

CircularQueue::~CircularQueue() {
    delete[] buffer;
}

void CircularQueue::enqueue(double value) {
    buffer[tail] = value;

    //tail index and mind circular structure
    tail = (tail + 1) % capacity;

    //circular logic
    if (isFull()) {
        head = (head + 1) % capacity;
    } else {
        count++;
    }
}

double CircularQueue::dequeue() {
    if (isEmpty()) return 0.0;

    double value = buffer[head];
    head = (head + 1) % capacity;
    count--;

    return value;
}

double CircularQueue::peek() const {
    if (isEmpty()) return 0.0;
    return buffer[head];
}

bool CircularQueue::isEmpty() const {
    return count == 0;
}

bool CircularQueue::isFull() const {
    return count == capacity;
}

int CircularQueue::getCount() const {
    return count;
}

int CircularQueue::getCapacity() const {
    return capacity;
}

double CircularQueue::getAverage() const {
    if (isEmpty()) return 0.0;

    double sum = 0.0;

    for (int i = 0; i < count; i++) {
        int index = (head + i) % capacity;
        sum += buffer[index];
    }

    return (sum/count);
}