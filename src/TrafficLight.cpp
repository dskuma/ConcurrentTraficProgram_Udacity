#include <iostream>
#include <random>
#include <chrono>
#include "TrafficLight.h"


/* Implementation of class "MessageQueue" */

 
template <typename T>
T MessageQueue<T>::receive()
{
    std::unique_lock<std::mutex> __lockqueue(_mutex);
    //ADD CONDITION VARIABLE
    _condition.wait(__lockqueue,[this] { return !_queue.empty();  });
    //TrafficLight::TrafficLightPhase _phase = _queue.back()
    T msg =  _queue.back();
    _queue.pop_back();
    return msg;
    // FP.5a : The method receive should use std::unique_lock<std::mutex> and _condition.wait() 
    // to wait for and receive new messages and pull them from the queue using move semantics. 
    // The received object should then be returned by the receive function. 
}

template <typename T>
void MessageQueue<T>::send(T &&msg)
{
    std::unique_lock<std::mutex> __lockqueue(_mutex);
    {
        _queue.push_back(msg); 
        _condition.notify_one();
    }
    // as well as _condition.notify_one() to add a new message to the queue and afterwards send a notification.

}


void TrafficLight::waitForGreen()
{
    while(true)
    {
        TrafficLight::TrafficLightPhase _phase =  lightMessages.receive();
        if(_phase == TrafficLight::TrafficLightPhase::GREEN)
        {
            return;
        }
    }
    // FP.5b : add the implementation of the method waitForGreen, in which an infinite while-loop 
    // runs and repeatedly calls the receive function on the message queue. 
    // Once it receives TrafficLightPhase::green, the method returns.
}

TrafficLight::TrafficLight() 
{
    _currentPhase = TrafficLightPhase::RED;
}

TrafficLight::TrafficLightPhase TrafficLight::getCurrentPhase()
{
    return _currentPhase;
}


void TrafficLight::simulate()
{
     threads.emplace_back(std::thread(&TrafficLight::cycleThroughPhases, this));

}

// virtual function which is executed in a threa


void TrafficLight::cycleThroughPhases()
{   

    std::random_device __randomDevice;  
    std::mt19937 gen(__randomDevice());
    std::uniform_int_distribution<int> __distributor(4000, 6000);
    int randomTime = __distributor(gen);
    auto start = std::chrono::high_resolution_clock::now();
    while(true)
    {
        // auto duration = std::chrono::duration_cast<std::chrono::seconds>(time_now - time_at_last_ping);
        auto now = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now - start).count();
        //std::cout << duration << " " << randomTime << "\n";
        if( duration >= randomTime )
        {
           // std::cout<<" elapsedMilliSeconds\n";
           // _currentPhase = (_currentPhase == TrafficLightPhase::RED) ? TrafficLightPhase::GREEN : TrafficLightPhase::RED;
           if(_currentPhase == TrafficLightPhase::RED)
           {
             _currentPhase = TrafficLightPhase::GREEN;
             lightMessages.send(std::move(TrafficLightPhase::GREEN));
           }
           else{
            _currentPhase = TrafficLightPhase::RED;
            lightMessages.send(std::move(TrafficLightPhase::RED));
           }
           start = std::chrono::high_resolution_clock::now();
           randomTime = __distributor(gen);
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    // FP.2a : Implement the function with an infinite loop that measures the time between two loop cycles 
    // and toggles the current phase of the traffic light between red and green and sends an update method 
    // to the message queue using move semantics. The cycle duration should be a random value between 4 and 6 seconds. 
    // Also, the while-loop should use std::this_thread::sleep_for to wait 1ms between two cycles. 
}
