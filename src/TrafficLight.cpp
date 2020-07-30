#include <iostream>
#include <random>
#include <chrono>
#include <thread>
#include <cstdlib>
#include <ctime>
#include <thread>
#include "TrafficLight.h"


template <typename T>
T MessageQueue<T>::receive()
{
    // FP.5a : The method receive should use std::unique_lock<std::mutex> and _condition.wait() 
    // to wait for and receive new messages and pull them from the queue using move semantics. 
    // The received object should then be returned by the receive function. 
    std::unique_lock<std::mutex> myLock(_mutex);
    
    _condition.wait(myLock , [this]{return !_queue.empty();});
    T current = std::move(_queue.back());
    _queue.pop_back();
    return current;
}

template <typename T>
void MessageQueue<T>::send(T &&msg)
{
    // FP.4a : The method send should use the mechanisms std::lock_guard<std::mutex> 
    // as well as _condition.notify_one() to add a new message to the queue and afterwards send a notification.
    
    std::lock_guard<std::mutex> myLock(_mutex);
    _queue.push_back(std::move(msg));
    _condition.notify_one();
}



TrafficLight::TrafficLight()
{
    _currentPhase = TrafficLightPhase::red;
}

void TrafficLight::waitForGreen()
{
    // FP.5b : add the implementation of the method waitForGreen, in which an infinite while-loop 
    // runs and repeatedly calls the receive function on the message queue. 
    // Once it receives TrafficLightPhase::green, the method returns.
    while(true){
    	auto x = mq->receive();
        if(x == TrafficLightPhase::green)return;
    }
}

TrafficLightPhase TrafficLight::getCurrentPhase()
{
    return _currentPhase;
}

void TrafficLight::simulate()
{
    // FP.2b : Finally, the private method „cycleThroughPhases“ should be started in a thread when the public method „simulate“ is called. To do this, use the thread queue in the base class. 
    threads.emplace_back(std::thread(&TrafficLight::cycleThroughPhases ,this  ) );
  
    
}

// virtual function which is executed in a thread
void TrafficLight::cycleThroughPhases()
{
    // FP.2a : Implement the function with an infinite loop that measures the time between two loop cycles 
    // and toggles the current phase of the traffic light between red and green and sends an update method 
    // to the message queue using move semantics. The cycle duration should be a random value between 4 and 6 seconds. 
    // Also, the while-loop should use std::this_thread::sleep_for to wait 1ms between two cycles.
  
  	std::random_device device;
    std::mt19937 mt_engine(device());
    std::uniform_int_distribution<> distribution(4, 6);

    auto lastUpdate = std::chrono::high_resolution_clock::now();
    auto duration = distribution(mt_engine);

  
  
    
    while(true){
      	std::this_thread::sleep_for(std::chrono::milliseconds(1));
      	srand(time(0));
		double a = rand()%2 + 4;
      	std::chrono::duration<double> elapsed = std::chrono::high_resolution_clock::now() - lastUpdate;
		if(elapsed.count() >= duration){
          _currentPhase  =  (_currentPhase == TrafficLightPhase::red ? TrafficLightPhase::green : TrafficLightPhase::red);
          mq->send(std::move(_currentPhase));
          
          //reset
          lastUpdate = std::chrono::high_resolution_clock::now();
        }
    }
}

