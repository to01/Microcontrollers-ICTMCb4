#ifndef QUEUE_H
#define QUEUE_H

#include "Make_Code_Mechanics.h"

#define MAXSIZE 6
#define COLORCODE 4

class Queue
{
public:
    uint8_t front;
    uint8_t rear;
    uint16_t queue[MAXSIZE][COLORCODE];

    Queue()
    {
        front = -1;
        rear = -1;
    }

    // checks if the queue is full
    bool isFull()
    {
        return rear == MAXSIZE - 1;
    }

    // enqueue the guess
    void enqueue(GameColors arr[], uint8_t size)
    {
        if (isFull())
        {
            dequeue();
        }

        rear++;
        for (uint8_t i = 0; i < size; i++)
        {
            queue[rear][i] = arr[i].ILI9341Color;
        }
    }

    // reset marker positions so the enqueue can overwrite the oldest guess
    // could change so the newest guess will be shown at the botom row and every other guess will be shifted up
    void dequeue()
    {
        front = -1;
        rear = -1;
    }
};

#endif // QUEUE_H