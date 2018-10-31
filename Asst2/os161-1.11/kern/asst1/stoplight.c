/*stoplight.c
 *
 * 31-1-2003 : GWA : Stub functions created for CS161 Asst1.
 *
 * NB: You can use only lock
 */


/*
 *
 * Includes
 *
 */

#include <types.h>
#include <lib.h>
#include <test.h>
#include <thread.h>
#include <synch.h>

/********************************
 * Declaration
 ********************************/
//Once all cars reach destination, destroy all locks
volatile int car_done;
struct lock *lock_car_done;

//only one thread prints at a time for clarity
//ensure atomically shout out when release lock
struct lock *lock_print;

//general purpose lock for mutex
struct lock *lock_general;
struct lock *lock_general2;

//for fake samaphore implemented using lock
struct lock *lock_sem_wait;
struct lock *lock_sem_post;

//maximum number of car in intersection
volatile int car_in;

//lock for each block of intersection
struct lock *lock_NE;
struct lock *lock_NW;
struct lock *lock_SW;
struct lock *lock_SE;

//lock for traffic lines
struct lock *line_N;
struct lock *line_S;
struct lock *line_W;
struct lock *line_E;

/*
number of cars created.
 */
#define NCARS 20

//initializing the above declaration
static
void init(){
	car_done = 0;
	lock_car_done = lock_create("car_done");

	lock_print = lock_create("print");
	lock_general = lock_create("general");
	lock_general2 = lock_create("general2");

	lock_sem_wait = lock_create("sem_wait");
	lock_sem_post = lock_create("sem_post");

	car_in = 3;

	//lock for intersection block to avoid "accident"
	lock_NE = lock_create("NE");
	lock_NW = lock_create("NW");
	lock_SW = lock_create("SW");
	lock_SE = lock_create("SE");

	//lock for traffic lines, 1 car per direction can use intersection
	line_N = lock_create("N");
	line_W = lock_create("W");
	line_S = lock_create("S");
	line_E = lock_create("E");
}

//exclusive right to use intersection from each direction
static
void acquire_dir_lock(unsigned long car_direction){
	switch(car_direction){
	case(0):lock_acquire(line_E);
			break;
	case(1):lock_acquire(line_N);
			break;
	case(2):lock_acquire(line_W);
			break;
	case(3):lock_acquire(line_S);
			break;
	}
}

static
void release_dir_lock(unsigned long car_direction){
	switch(car_direction){
	case(0):lock_release(line_E);
			break;
	case(1):lock_release(line_N);
			break;
	case(2):lock_release(line_W);
			break;
	case(3):lock_release(line_S);
			break;
	}
}

//Just printing out the route of the car making the move
//Acquiring lock on the move
static
void trace_turnright(unsigned long carnumber,
    struct lock* lock1){
	lock_acquire(lock1);
	lock_acquire(lock_print);	//let me finish saying
	kprintf("Car %lu enters block %s \n",carnumber,lock1->name);
	lock_release(lock1);
	kprintf("Car %lu leaves block %s \n",carnumber,lock1->name);
	kprintf("Car %lu reaches destination \n",carnumber);
	lock_release(lock_print);

	lock_acquire(lock_car_done);
	car_done++;
	lock_release(lock_car_done);
}

//Just printing out the route of the car making the move
//Acquiring lock on the move
static
void trace_gostraight(unsigned long carnumber,
    struct lock* lock1,struct lock* lock2){
	lock_acquire(lock1);
	lock_acquire(lock_print);	//let me finish saying
	kprintf("Car %lu enters block %s \n",carnumber,lock1->name);
	lock_release(lock_print);

	lock_acquire(lock2);
	lock_acquire(lock_print);	//let me finish saying
	lock_release(lock1);
	kprintf("Car %lu leaves block %s \n",carnumber,lock1->name);
	kprintf("Car %lu enters block %s \n",carnumber,lock2->name);
	lock_release(lock2);

	kprintf("Car %lu leaves block %s \n",carnumber,lock2->name);
	kprintf("Car %lu reaches destination \n",carnumber);
	lock_release(lock_print);

	lock_acquire(lock_car_done);
	car_done++;
	lock_release(lock_car_done);
}

//Just printing out the route of the car making the move
//Acquiring lock on the move
static
void trace_turnleft(unsigned long carnumber,
    struct lock* lock1,struct lock* lock2,struct lock* lock3){
	lock_acquire(lock1);
	lock_acquire(lock_print);	//let me finish saying
	kprintf("Car %lu enters block %s \n",carnumber,lock1->name);
	lock_release(lock_print);

	lock_acquire(lock2);
	lock_acquire(lock_print);	//let me finish saying
	lock_release(lock1);
	kprintf("Car %lu leaves block %s \n",carnumber,lock1->name);
	kprintf("Car %lu enters block %s \n",carnumber,lock2->name);
	lock_release(lock_print);

	lock_acquire(lock3);
	lock_acquire(lock_print);	//let me finish saying
	lock_release(lock2);
	kprintf("Car %lu leaves block %s \n",carnumber,lock2->name);
	kprintf("Car %lu enters block %s \n",carnumber,lock3->name);
	lock_release(lock3);
	kprintf("Car %lu leaves block %s \n",carnumber,lock3->name);
	kprintf("Car %lu reaches destination \n",carnumber);
	lock_release(lock_print);

	lock_acquire(lock_car_done);
	car_done++;
	lock_release(lock_car_done);
}

static
void
gostraight(unsigned long cardirection,
           unsigned long carnumber)
{
        //int cardirection; //0 East, 1 North, 2 West, 3 South
        switch(cardirection){
		case(0):trace_gostraight(carnumber,lock_NE,lock_NW); break;
		case(1):trace_gostraight(carnumber,lock_NW,lock_SW); break;
		case(2):trace_gostraight(carnumber,lock_SW,lock_SE); break;
		case(3):trace_gostraight(carnumber,lock_SE,lock_NE); break;
		}
		/*
		         * Avoid unused variable warnings.
         */
        (void) cardirection;
        (void) carnumber;
}

static
void
turnleft(unsigned long cardirection,
         unsigned long carnumber)
{
        //int cardirection; //0 East, 1 North, 2 West, 3 South
		switch(cardirection){
		case(0):trace_turnleft(carnumber,lock_NE,lock_NW,lock_SW); break;
		case(1):trace_turnleft(carnumber,lock_NW,lock_SW,lock_SE); break;
		case(2):trace_turnleft(carnumber,lock_SW,lock_SE,lock_NE); break;
		case(3):trace_turnleft(carnumber,lock_SE,lock_NE,lock_NW); break;
		}

		/*
		         * Avoid unused variable warnings.
         */
        (void) cardirection;
        (void) carnumber;
}

static
void
turnright(unsigned long cardirection,
          unsigned long carnumber)
{
        //int cardirection; //0 East, 1 North, 2 West, 3 South
        switch(cardirection){
		case(0):trace_turnright(carnumber,lock_NE); break;
		case(1):trace_turnright(carnumber,lock_NW); break;
		case(2):trace_turnright(carnumber,lock_SW); break;
		case(3):trace_turnright(carnumber,lock_SE); break;
		}
		/*
		         * Avoid unused variable warnings.
         */
        (void) cardirection;
        (void) carnumber;
}

//Signal car to go by stoplight()
static
void go(unsigned int car_dir, unsigned int car_dest, unsigned long car_num){
	switch(car_dest){
		case(0):
			turnright(car_dir, car_num);
			break;
		case(1):
			gostraight(car_dir, car_num);
			break;
		case(2):
			turnleft(car_dir, car_num);
			break;
	}
}

//fake sem_wait and sem_post to limit maximum of 3 cars entering intersection
void fake_sem_wait(){
	lock_acquire(lock_sem_wait);
	//kprintf("In sem wait before car in intersection %d\n",car_in);
	while(car_in <= 0)	//do nothing
	{thread_yield(); }
	lock_acquire(lock_general2);
	car_in--;
	lock_release(lock_general2);
	//kprintf("In sem wait after car in intersection %d\n",car_in);
	lock_release(lock_sem_wait);
}
void fake_sem_post(){
	lock_acquire(lock_sem_post);
	//kprintf("In sem post before car in intersection %d\n",car_in);
	lock_acquire(lock_general2);
	car_in++;
	lock_release(lock_general2);
	//kprintf("In sem post after car in intersection %d\n",car_in);
	lock_release(lock_sem_post);
}

static
void stoplight(unsigned int car_dir, unsigned int car_dest, unsigned long carnum){
	fake_sem_wait(); //allow maximum 3 car in intersection
	//kprintf("car in intersection %d\n",car_in);
	if( car_dir == 0)
	{
		go(car_dir, car_dest,carnum);
		fake_sem_post();
	}
	if( car_dir == 1)
	{
		go(car_dir, car_dest,carnum);
		fake_sem_post();
	}
	if( car_dir == 2)
	{
		go(car_dir, car_dest,carnum);
		fake_sem_post();
	}
	if( car_dir == 3)
	{
		go(car_dir, car_dest,carnum);
		fake_sem_post();
	}
}

static
void
approachintersection(void * unusedpointer,
                     unsigned long carnumber)
{
	//kprintf("Inside approachintersection()");
        /*
         * Avoid unused variable and function warnings.
         */
    (void) unusedpointer;
    (void) carnumber;
	(void) gostraight;
	(void) turnleft;
	(void) turnright;
        /*
         * cardirection is set randomly.
         */
    unsigned int car_direction = random() % 4; //Where the car is approaching from
    unsigned int car_destination = random() % 3;

    //0 turn right, 1 go straight, 2 turn left
	char car_dest_str[3][16] = {{"turn right"}, {"go straight"},{"turn left"}};
	//0 East, 1 North, 2 West, 3 South
	char car_dir_str[4][2] = {{"E"},{"N"},{"W"},{"S"}};


	//If you're from same the direction, wait to be add to line
	acquire_dir_lock(car_direction);
	kprintf("Car %lu is approaching from %s and wants to %s \n",
    		carnumber, car_dir_str[car_direction] , car_dest_str[car_destination] );

	stoplight(car_direction, car_destination, carnumber);
	release_dir_lock(car_direction);

}

//destroy all locks to free memory
static void destroy_all_locks(){
	lock_destroy(lock_car_done);

	lock_destroy(lock_print);
	lock_destroy(lock_general);
	lock_destroy(lock_general2);

	lock_destroy(lock_sem_wait);
	lock_destroy(lock_sem_post);

	lock_destroy(lock_NE);
	lock_destroy(lock_NW);
	lock_destroy(lock_SW);
	lock_destroy(lock_SE);

	lock_destroy(line_N);
	lock_destroy(line_W);
	lock_destroy(line_S);
	lock_destroy(line_E);
}

int
createcars(int nargs,
           char ** args)
{
	//kprintf("Inside createcars\n");
        int index, error;

        /*
         * Avoid unused variable warnings.
         */

        (void) nargs;
        (void) args;

        /*
         * Start NCARS approachintersection() threads.
         */

	struct addrspace *t_vmspace;

	init();
        for (index = 0; index < NCARS; index++) {

                error = thread_fork("approachintersection thread",
                                    t_vmspace,
                                    index,
                                    approachintersection,
                                    NULL
                                    );

                /*
                 * panic() on error.
                 */

                if (error) {

                        panic("approachintersection: thread_fork failed: %s\n",
                              strerror(error)
                              );
                }
        }

        while(car_done != NCARS){
			thread_yield();
		}

		destroy_all_locks();

        return 0;
}



