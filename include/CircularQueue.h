#ifndef CIRCULARQUEUE_H
#define CIRCULARQUEUE_H

class CircularQueue {
private:
    double* buffer;
    int     capacity;
    int     head;
    int     tail;
    int     count;
    double  sum;        // variable used to make getAverage() time complexity to O(1)

public:
    CircularQueue(int capacity);
    ~CircularQueue();
    // Adds a value. If Queue is full, overwrites oldest value with new
    // and makes new head the next oldest value
    void enqueue(double value);
    
    // Removes and returns the oldest value. 
    double dequeue();

    // Return the oldest value without remove it. 
    double peek() const;    //oldest value
    
    double getAverage() const;
    
    bool isFull()  const;
    bool isEmpty() const;

    int getCount() const;   //number of elements currently in queue
    int getCapacity() const; //max number of elements queue can hold
};

#endif