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
 * Constants
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

volatile int car_in;
	//only one thread prints at a time for clarity
	//ensure atomically shout out when release lock
	struct lock *lock_print;

	struct lock *lock_general;

	struct lock *lock_NE;
	struct lock *lock_NW;
	struct lock *lock_SW;
	struct lock *lock_SE;

	struct lock *flow_EW;
	struct lock *flow_NS;

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
	/*
	E_head = new_wait_node();
	N_head = new_wait_node();
	W_head = new_wait_node();
	S_head = new_wait_node();*/

	//lock for intersection block to avoid "accident"
	lock_NE = lock_create("NE");
	lock_NW = lock_create("NW");
	lock_SW = lock_create("SW");
	lock_SE = lock_create("SE");

	flow_EW = lock_create("EW");
	flow_NS = lock_create("NS");

	//lock for traffic lines, 1 car per direction can use intersection
	line_N = lock_create("N");
	line_W = lock_create("W");
	line_S = lock_create("S");
	line_E = lock_create("E");
}

static
void add_Q(unsigned long carnumber, unsigned int car_direction,
			unsigned int car_destination){
	struct wait_line *head;
	struct wait_line *new_car;
	new_car = wait_node();
	new_car->car_num = carnumber;
	new_car->car_dir = car_direction;
	new_car->car_dest = car_destination;
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
static
void de_Q(struct wait_line *line){
	kprintf("inside de_Q for car %d\n", line->car_num);
	if(line != NULL){
	struct wait_line *head;
	struct wait_line *temp;
	
	kprintf("1");
	head = line->next;
	kprintf("2");
	temp = line;
	kprintf("3");
	kprintf("4");

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
	kprintf("5");
	}
}
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
static
void trace_turnright(unsigned long carnumber,
    struct lock* lock1){
	lock_acquire(lock1);
	lock_acquire(lock_print);
	kprintf("Car number %lu enters block %s \n",carnumber,lock1->name);
	lock_release(lock1);
	kprintf("Car number %lu leaves block %s \n",carnumber,lock1->name);
	kprintf("Car number %lu reach destination \n",carnumber);
	lock_release(lock_print);

	kprintf("inside trace 1\n");
	lock_acquire(lock_general);
	car_in--;
	lock_release(lock_general);
}
static
void trace_gostraight(unsigned long carnumber,
    struct lock* lock1,struct lock* lock2){
	lock_acquire(lock1);
	lock_acquire(lock_print);
	kprintf("Car number %lu enters block %s \n",carnumber,lock1->name);
	lock_release(lock_print);

	lock_acquire(lock2);
	lock_acquire(lock_print);
	lock_release(lock1);
	kprintf("Car number %lu leaves block %s \n",carnumber,lock1->name);
	kprintf("Car number %lu enters block %s \n",carnumber,lock2->name);
	lock_release(lock2);
	kprintf("Car number %lu leaves block %s \n",carnumber,lock2->name);
	kprintf("Car number %lu reach destination \n",carnumber);
	lock_release(lock_print);

	kprintf("inside trace 1\n");
	lock_acquire(lock_general);
	car_in--;
	lock_release(lock_general);
}
static
void trace_turnleft(unsigned long carnumber,
    struct lock* lock1,struct lock* lock2,struct lock* lock3){
	lock_acquire(lock1);
	lock_acquire(lock_print);
	kprintf("Car number %lu enters block %s \n",carnumber,lock1->name);
	lock_release(lock_print);

	lock_acquire(lock2);
	lock_acquire(lock_print);
	lock_release(lock1);
	kprintf("Car number %lu leaves block %s \n",carnumber,lock1->name);
	kprintf("Car number %lu enters block %s \n",carnumber,lock2->name);
	lock_release(lock_print);

	lock_acquire(lock3);
	lock_acquire(lock_print);
	lock_release(lock2);
	kprintf("Car number %lu leaves block %s \n",carnumber,lock2->name);
	kprintf("Car number %lu enters block %s \n",carnumber,lock3->name);
	lock_release(lock3);
	kprintf("Car number %lu leaves block %s \n",carnumber,lock3->name);
	kprintf("Car number %lu reach destination \n",carnumber);
	lock_release(lock_print);

	kprintf("inside trace 1");
	lock_acquire(lock_general);
	car_in--;
	lock_release(lock_general);
}

static
void
gostraight(unsigned long cardirection,
           unsigned long carnumber)
{
        /*
         * Avoid unused variable warnings.
         */
        //int cardirection; //0 East, 1 North, 2 West, 3 South
        switch(cardirection){
		case(0):trace_gostraight(carnumber,lock_NE,lock_NW); break;
		case(1):trace_gostraight(carnumber,lock_NW,lock_SW); break;
		case(2):trace_gostraight(carnumber,lock_SW,lock_SE); break;
		case(3):trace_gostraight(carnumber,lock_SE,lock_NE); break;
		}
        (void) cardirection;
        (void) carnumber;
}

static
void
turnleft(unsigned long cardirection,
         unsigned long carnumber)
{
        /*
         * Avoid unused variable warnings.
         */
        //int cardirection; //0 East, 1 North, 2 West, 3 South
		switch(cardirection){
		case(0):trace_turnleft(carnumber,lock_NE,lock_NW,lock_SW); break;
		case(1):trace_turnleft(carnumber,lock_NW,lock_SW,lock_SE); break;
		case(2):trace_turnleft(carnumber,lock_SW,lock_SE,lock_NE); break;
		case(3):trace_turnleft(carnumber,lock_SE,lock_NE,lock_NW); break;
		}
        (void) cardirection;
        (void) carnumber;
}

static
void
turnright(unsigned long cardirection,
          unsigned long carnumber)
{
        /*
         * Avoid unused variable warnings.
         */
        //int cardirection; //0 East, 1 North, 2 West, 3 South
        switch(cardirection){
		case(0):trace_turnright(carnumber,lock_NE); break;
		case(1):trace_turnright(carnumber,lock_NW); break;
		case(2):trace_turnright(carnumber,lock_SW); break;
		case(3):trace_turnright(carnumber,lock_SE); break;
		}
        (void) cardirection;
        (void) carnumber;
}

static
void go(struct wait_line *line){
	switch(line->car_dest){
		case(0):
			turnright(line->car_dir, line->car_num);
			kprintf("inside go 1");
			break;
		case(1):
			gostraight(line->car_dir, line->car_num);
			kprintf("inside go 2");
			break;
		case(2):
			turnleft(line->car_dir, line->car_num);
			kprintf("inside go 3");
			break;
	}
}

static
void stoplight(){
	unsigned int i = 0;
	while(E_head != NULL || N_head != NULL || W_head != NULL || S_head != NULL)
	{

	//	lock_acquire(lock_general);
		if(car_in > 3)
		{
			lock_release(lock_general);
			break;
		}
		if( i % 4 == 0 && E_head != NULL)
		{
			car_in++;
			kprintf("E_head call go for car %d", E_head->car_num);
			go(E_head);
			de_Q(E_head);
		}
		if( i % 4 == 1 && N_head != NULL)
		{
			car_in++;
			kprintf("N_head call go for car %d", N_head->car_num);
			go(N_head);
			de_Q(N_head);
		}
		if( i % 4 == 2 && W_head != NULL)
		{
			car_in++;
			kprintf("W_head call go for car %d", W_head->car_num);
			go(W_head);
			de_Q(W_head);
		}
		if( i % 4 == 3 && S_head != NULL)
		{
			car_in++;
			kprintf("S_head call go for car %d", S_head->car_num);
			go(S_head);
			de_Q(S_head);
		}
		if(car_in < 4)
		{i++;}
	//	lock_release(lock_general);
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

	acquire_dir_lock(car_direction);
	//lock_acquire(lock_general);

	kprintf("Car %lu is approaching from %s and wants to %s \n",
    		carnumber, car_dir_str[car_direction] , car_dest_str[car_destination] );

	add_Q(carnumber,car_direction,car_destination); //add car to queue
        //lock_release(lock_general);
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
		stoplight();
        return 0;
}

