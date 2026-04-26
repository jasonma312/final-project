#ifndef CIRCULARQUEUE_H
#define CIRCULARQUEUE_H

//fixed-size circular queue
//use to compute N-day moving averages
class CircularQueue {
private:
    double* buffer;
    int capacity;
    int head;
    int tail;
    int count;

public:
    CircularQueue(int capacity);
    ~CircularQueue();

    void enqueue(double value);
    double dequeue();
    double peek() const;    //oldest value
    double getAverage() const;
    bool isFull()  const;
    bool isEmpty() const;
    int getCount() const;   //number of elements currently in queue
    int getCapacity() const; //max number of elements queue can hold
};

#endif