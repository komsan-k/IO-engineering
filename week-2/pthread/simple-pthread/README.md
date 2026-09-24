# Simple Arduino ESP32 pthread Example --- Four Tasks

## Objective

Create a simple Arduino program for the **ESP32** using `pthread` to run
**four concurrent tasks**. Each task periodically displays a different
message on the Serial Monitor.

## Task Configuration

  Task     Message                   Delay
  -------- --------------------- ---------
  Task 1   `Hello from Task 1`     1000 ms
  Task 2   `Hello from Task 2`     1500 ms
  Task 3   `Hello from Task 3`     2000 ms
  Task 4   `Hello from Task 4`     2500 ms

------------------------------------------------------------------------

## Complete Arduino Code

``` cpp
#include <Arduino.h>
#include <pthread.h>

void *task1(void *arg)
{
  while (1)
  {
    Serial.println("Hello from Task 1");
    delay(1000);
  }
  return NULL;
}

void *task2(void *arg)
{
  while (1)
  {
    Serial.println("Hello from Task 2");
    delay(1500);
  }
  return NULL;
}

void *task3(void *arg)
{
  while (1)
  {
    Serial.println("Hello from Task 3");
    delay(2000);
  }
  return NULL;
}

void *task4(void *arg)
{
  while (1)
  {
    Serial.println("Hello from Task 4");
    delay(2500);
  }
  return NULL;
}

void setup()
{
  Serial.begin(115200);
  delay(1000);

  Serial.println("ESP32 pthread - Four Tasks");

  pthread_t thread1;
  pthread_t thread2;
  pthread_t thread3;
  pthread_t thread4;

  pthread_create(&thread1, NULL, task1, NULL);
  pthread_create(&thread2, NULL, task2, NULL);
  pthread_create(&thread3, NULL, task3, NULL);
  pthread_create(&thread4, NULL, task4, NULL);
}

void loop()
{
  // pthread tasks run independently
  delay(1000);
}
```

------------------------------------------------------------------------

## How It Works

Four thread variables are created:

``` cpp
pthread_t thread1;
pthread_t thread2;
pthread_t thread3;
pthread_t thread4;
```

Each thread is started using `pthread_create()`:

``` cpp
pthread_create(&thread1, NULL, task1, NULL);
pthread_create(&thread2, NULL, task2, NULL);
pthread_create(&thread3, NULL, task3, NULL);
pthread_create(&thread4, NULL, task4, NULL);
```

General syntax:

``` cpp
pthread_create(&thread, NULL, taskFunction, NULL);
```

Each task contains an infinite loop and displays its own message at a
different interval.

------------------------------------------------------------------------

## Expected Serial Monitor Output

``` text
ESP32 pthread - Four Tasks

Hello from Task 1
Hello from Task 2
Hello from Task 3
Hello from Task 4
Hello from Task 1
Hello from Task 2
Hello from Task 1
Hello from Task 3
...
```

The exact order may vary because the four threads execute concurrently.

------------------------------------------------------------------------

## Key Concepts

This example demonstrates:

-   ESP32 `pthread` programming
-   Four concurrent tasks
-   `pthread_create()`
-   Independent task execution
-   Different task delays
-   Serial Monitor output
