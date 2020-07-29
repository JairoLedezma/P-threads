/*
Name:Jairo Ledezma
class:cosc 3360
putpose: Create a program using the pthread library around the problem of a porr ventilated tunnel.
The tunnel lets n number of cars in at a time and each direction. There should be the use of one 
mutex and condition variable
*/
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <iostream>
#include <string>
#include <sstream>
using namespace std;


struct arguements{ // struct holds the arguements
    char serialNumber;// max number of soth bound cars third line
    int tunnelTravelTime;// time the cars have to travel through the tunnel
};

static int numSCar = 0;// number of south cars
static int numNCar = 0;// number of north cars
static int maxNNBCars;// max number of north bound cars
static int maxNSBCars;//max number of south bound cars
static int maxNCars;//max number of cars in the tunnel
static int curNNBCars;//current number of north bound cars
static int curNSBCars;// current number of south bound cars
static int curNCars;// current number of cars
static int NCarsToWait=0;// Number of cars that had to wait
static pthread_mutex_t lock; // mutex lock
pthread_cond_t clear = PTHREAD_COND_INITIALIZER;// initialize condition


void *southBoundcar(void *arg){
        numSCar++;
        int c = numSCar;
        struct arguements *argptr ;
        argptr = (struct arguements *) arg;//recasts the void args to that of struct
        cout << "Southbound car #" << c << " arrives at the tunnel"<<endl;
        pthread_mutex_lock(&lock);// locks mutex
        // critical 
        if((curNCars == maxNCars) || (curNSBCars == maxNSBCars)){// checks to see if it has to wait
            NCarsToWait++;// had to do a seperate if or else it would increase until every time the while looped
            cout << "--Southbound car "<<c<<" has to wait\n";
        }
        while((curNCars == maxNCars) || (curNSBCars == maxNSBCars)){ // while waiting for conditions to clear, wait for a signal
           // pthread_cond_signal(&clear);
            pthread_cond_wait(&clear, &lock);  
        }
        curNSBCars++;
        curNCars++;//updates counters
        pthread_mutex_unlock(&lock);// unlocks mutex
        cout<< "Southbound car #" << c << " enters the tunnel"<< endl;
        sleep(argptr->tunnelTravelTime); // sleeps for the travel time : we are in the tunnel as of now
        pthread_mutex_lock(&lock);// locks mutex
            curNSBCars--;//updates counters
            curNCars--;
            cout<< "Southbound car #"<< c<<" exits the tunnel"<< endl;
            pthread_cond_broadcast(&clear);// wakes all of the threads
        pthread_mutex_unlock(&lock);//unlocks mutex
        // print a message
        //exit the tunnel and eliminate
        pthread_exit(NULL);
        return NULL;
}

void *northBoundcar(void *arg){
        numNCar++;
        int c = numNCar;
        struct arguements *argptr ;
        argptr = (struct arguements *) arg;// recasts to struct
        cout << "Northbound car #" << c << " arrives at the tunnel"<<endl;
         pthread_mutex_lock(&lock);//locks the mutex
         if((curNCars == maxNCars) || (curNNBCars == maxNNBCars)){// checks to see if it has to wait
            NCarsToWait++;// had to do a seperate if or else it would increase until every time the while looped
            cout << "--Northbound car "<<c<<" has to wait\n";
        }
        while((curNCars == maxNCars) || (curNNBCars == maxNNBCars)){   // while waiting, wait for signal to go through tunnel
            //pthread_cond_signal(&clear);
            pthread_cond_wait(&clear, &lock);// waitng for broadcast, open mutex
            
        }
        curNNBCars++;
        curNCars++;//updates counters
        pthread_mutex_unlock(&lock);// unlock mutex
            cout << "Northbound car #"<< c << " enters the tunnel"<< endl;
            sleep(argptr->tunnelTravelTime); // sleeps for the travel time : we are in the tunnel as of now
        pthread_mutex_lock(&lock);//lock mutex
            curNNBCars--;//updates counters
            curNCars--;
            cout<< "Northbound car #"<< c<<" exits the tunnel"<< endl;
            pthread_cond_broadcast(&clear);// broadcasts that it is out of tunnel
        pthread_mutex_unlock(&lock);// releases lock
        // print a message
        //exit the tunnel and eliminate
        pthread_exit(NULL);
        return NULL;
}


int main(){
    
    int MAX_THREAD = 128;// Max set of threads
    // creating pthreads
    // declares a valid child function
    // Decalre a thread ID
    pthread_t cartid[MAX_THREAD];//size of 128
    int iterator=0;
    pthread_mutex_init(&lock, NULL);//creates a mutex= sets to 1

    //takes in the first 3 inputs in every file(Ncars, max north, max south)
    cin>>maxNCars>>maxNNBCars>>maxNSBCars;
    
    cout<< "Maximum number of cars in the tunnel:"<<maxNCars<<endl
    << "Maximum number of northbound cars: " << maxNNBCars<<endl
    << "Maximum nmber of southbound cars: " << maxNSBCars<<endl;
    
    int delay;
    char direction;
    int travelTime;
    
    while (cin >> delay >> direction >> travelTime) { // takes in input as long as there is input
        // creates a struct object
        struct arguements args;
        //sets the parameters
        
        sleep(delay);// sleeps : represents a wait until the next car arrives
        // Create a child thread that will execute the right function
        if (direction == 'N') { // cars is going in the north direction
            args.serialNumber = direction;
            args.tunnelTravelTime = travelTime;
            pthread_create(&cartid[iterator], NULL, northBoundcar, (void *) &args);// creates the appropriate thread
            iterator++;
        } else { // car is going the south direction
            args.serialNumber = direction;
            args.tunnelTravelTime = travelTime;
            pthread_create(&cartid[iterator], NULL, southBoundcar, (void *) &args);//creates the appropriate thread
            iterator++;
        } // if-else
       
    }//while
   //
   
    for(int i=0; i<iterator ;i++){
        pthread_join(cartid[i], NULL);//joins the threads
    }
    cout<< numNCar << " Northbound car(s) crossed the tunnel"<<endl;
    cout<< numSCar << " Southbound car(s) crossed the tunnel"<<endl;// final outputs
    cout<< NCarsToWait << " car(s) had to wait"<<endl;
    return 0;
}