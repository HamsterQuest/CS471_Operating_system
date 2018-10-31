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

/*
 *
 * Declaration
 *
 */
struct wait_line *wait_node();

//create wait line at E, N, W, S entrance
struct wait_line{
	int car_num;
	int car_dir;
	int car_dest;
	struct wait_line *next;
};

//allow the maximum number of car in intersection
volatile int car_in;

//only one thread prints at a time for clarity
//ensure atomically shout out when release lock
struct lock *lock_print;

//general purpose lock for mutex
struct lock *lock_general;

//lock for each block of intersection
struct lock *lock_NE;
struct lock *lock_NW;
struct lock *lock_SW;
struct lock *lock_SE;

struct wait_line *E_head;
struct wait_line *N_head;
struct wait_line *W_head;
struct wait_line *S_head;

//lock for traffic lines Q
struct lock *line_N;
struct lock *line_S;
struct lock *line_W;
struct lock *line_E;

/*
number of cars created.
 */
#define NCARS 20

static
void init(){
	lock_print = lock_create("print");
	lock_general = lock_create("general");

	car_in = 0;

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

//create new node
struct wait_line *wait_node(){
	struct wait_line *node;
	node = kmalloc(sizeof(struct wait_line));
	if (node == NULL) {
		return NULL;
	}
	node->next = NULL;
	node->car_num = 0;
	node->car_dir = 0;
	return node;
}

//add cars to queue line using linked node
static
void add_Q(unsigned long carnumber, unsigned int car_direction,
			unsigned int car_destination){

	struct wait_line *head;
	struct wait_line *new_car;

	new_car = wait_node();
	new_car->car_num = carnumber;
	new_car->car_dir = car_direction;
	new_car->car_dest = car_destination;

	//get the appropriate head for the line
	switch(car_direction){
	case(0):head = E_head;
			break;
	case(1):head = N_head;
			break;
	case(2):head = W_head;
			break;
	case(3):head = S_head;
			break;
	}

	//insert new node
	if(head == NULL){
		head = new_car;
		switch(car_direction){
		case(0):E_head = head;
				break;
		case(1):N_head = head;
				break;
		case(2):W_head = head;
				break;
		case(3):S_head = head;
				break;
		}
	}
	else{
		struct wait_line *node;
        	node = head;
		while(node->next != NULL){
			node = node->next;
		}
		node->next = new_car;
	}
}

//remove head of the line
static
void de_Q(struct wait_line *line){
	if(line != NULL){
	struct wait_line *head;
	struct wait_line *temp;

	head = line->next;
	temp = line;

	switch(temp->car_dir){
	case(0):E_head = head;
			break;
	case(1):N_head = head;
			break;
	case(2):W_head = head;
			break;
	case(3):S_head = head;
			break;
	}

	temp->next = NULL;
        temp->car_num = 0;
        temp->car_dir = 0;
	temp->car_dest = 0;
        kfree(temp);
	}
}

//queueing up in line
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
	kprintf("Car number %lu enters block %s \n",carnumber,lock1->name);
	lock_release(lock1);
	kprintf("Car number %lu leaves block %s \n",carnumber,lock1->name);
	kprintf("Car number %lu reach destination \n",carnumber);
	lock_release(lock_print);

	//updated the number of car in intersection
	lock_acquire(lock_general);
	car_in--;
	lock_release(lock_general);
}

//Just printing out the route of the car making the move
//Acquiring lock on the move
static
void trace_gostraight(unsigned long carnumber,
    struct lock* lock1,struct lock* lock2){
	lock_acquire(lock1);
	lock_acquire(lock_print);	//let me finish saying
	kprintf("Car number %lu enters block %s \n",carnumber,lock1->name);
	lock_release(lock_print);

	lock_acquire(lock2);
	lock_acquire(lock_print);	//let me finish saying
	lock_release(lock1);
	kprintf("Car number %lu leaves block %s \n",carnumber,lock1->name);
	kprintf("Car number %lu enters block %s \n",carnumber,lock2->name);
	lock_release(lock2);
	kprintf("Car number %lu leaves block %s \n",carnumber,lock2->name);
	kprintf("Car number %lu reach destination \n",carnumber);
	lock_release(lock_print);

	//updated the number of car in intersection
	lock_acquire(lock_general);
	car_in--;
	lock_release(lock_general);
}

//Just printing out the route of the car making the move
//Acquiring lock on the move
static
void trace_turnleft(unsigned long carnumber,
    struct lock* lock1,struct lock* lock2,struct lock* lock3){
	lock_acquire(lock1);
	lock_acquire(lock_print);	//let me finish saying
	kprintf("Car number %lu enters block %s \n",carnumber,lock1->name);
	lock_release(lock_print);

	lock_acquire(lock2);
	lock_acquire(lock_print);	//let me finish saying
	lock_release(lock1);
	kprintf("Car number %lu leaves block %s \n",carnumber,lock1->name);
	kprintf("Car number %lu enters block %s \n",carnumber,lock2->name);
	lock_release(lock_print);

	lock_acquire(lock3);
	lock_acquire(lock_print);	//let me finish saying
	lock_release(lock2);
	kprintf("Car number %lu leaves block %s \n",carnumber,lock2->name);
	kprintf("Car number %lu enters block %s \n",carnumber,lock3->name);
	lock_release(lock3);
	kprintf("Car number %lu leaves block %s \n",carnumber,lock3->name);
	kprintf("Car number %lu reach destination \n",carnumber);
	lock_release(lock_print);

	//updated the number of car in intersection
	lock_acquire(lock_general);
	car_in--;
	lock_release(lock_general);
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
void go(struct wait_line *line){
	switch(line->car_dest){
		case(0):
			turnright(line->car_dir, line->car_num);
			break;
		case(1):
			gostraight(line->car_dir, line->car_num);
			break;
		case(2):
			turnleft(line->car_dir, line->car_num);
			break;
	}
}

static
void stoplight(){
	unsigned int i = 0;
	while(E_head != NULL || N_head != NULL || W_head != NULL || S_head != NULL)
	{
		if(car_in > 3)	//if there are 3 cars in intersection, do nothing
		{break;}

		//round robin through each line and signal car to go
		if( i % 4 == 0 && E_head != NULL)
		{
			car_in++;
			go(E_head);
			de_Q(E_head);
		}
		if( i % 4 == 1 && N_head != NULL)
		{
			car_in++;
			go(N_head);
			de_Q(N_head);
		}
		if( i % 4 == 2 && W_head != NULL)
		{
			car_in++;
			go(W_head);
			de_Q(W_head);
		}
		if( i % 4 == 3 && S_head != NULL)
		{
			car_in++;
			go(S_head);
			de_Q(S_head);
		}

		//don't skip a car line if there are already 3 cars in intersection
		if(car_in < 4)
		{i++;}
	}
}

static
void
approachintersection(void * unusedpointer,
                     unsigned long carnumber)
{
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

	add_Q(carnumber,car_direction,car_destination); //add car to queue
	release_dir_lock(car_direction);
}

int
createcars(int nargs,
           char ** args)
{
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

        //all cars created, let's them move now
		stoplight();

        return 0;
}


