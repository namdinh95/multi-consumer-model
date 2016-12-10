/* Name: Nam Dinh
 * Email: nvd130030@utdallas.edu
 * Course: CS 3376.502
 */

#include <iostream>
#include "cdk.h"
#include <boost/thread.hpp>
#include <boost/interprocess/sync/interprocess_semaphore.hpp>
#include <string>
#include <sstream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <signal.h>


#define MATRIX_WIDTH 5
#define MATRIX_HEIGHT 5
#define BOX_WIDTH 16
#define MATRIX_NAME_STRING "Test Matrix"
#define NUM_THREADS 25

using namespace std;

boost::mutex lockUpdate;	// Mutrx to protect screen updates
boost::barrier barrier(NUM_THREADS + 1);	// Barrier for 26 threads

WINDOW *window;
CDKSCREEN *cdkscreen;
CDKMATRIX *myMatrix;           // CDK Screen Matrix

vector<unsigned int> qty(NUM_THREADS);	// To hold quantity of all bins

struct semaphore
{
  semaphore() : sem(0)
  {}
  // Semaphore to update works for each thread
  boost::interprocess::interprocess_semaphore sem;
};

struct mutex_manual
{
  //mutex_manual() : mut
  //{}
  boost::mutex mut;
};

vector<semaphore *> updateWorks;
vector<mutex_manual *> addWorks;	// To hold mutexes of each bin for addming more works by producer

int stop = 0;	// To stop infinite while loop

void update_box(int, int, int, char, int);
void thread_func(int);
void signalHandler(int);

int main()
{

  for (int i = 0; i < NUM_THREADS; i++)
  {
    semaphore *semas = new semaphore();
    updateWorks.push_back(semas);
    mutex_manual *mut = new mutex_manual();
    addWorks.push_back(mut);
  }

  const char 		*rowTitles[MATRIX_HEIGHT+1] = {"0", "a", "b", "c", "d", "e"};
  const char 		*columnTitles[MATRIX_WIDTH+1] = {"0", "a", "b", "c", "d", "e"};
  int		boxWidths[MATRIX_WIDTH+1] = {BOX_WIDTH, BOX_WIDTH, BOX_WIDTH, BOX_WIDTH, BOX_WIDTH, BOX_WIDTH};
  int		boxTypes[MATRIX_WIDTH+1] = {vMIXED, vMIXED, vMIXED, vMIXED, vMIXED, vMIXED};

  /*
   * Initialize the Cdk screen.
   *
   * Make sure the putty terminal is large enough
   */
  window = initscr();
  cdkscreen = initCDKScreen(window);

  /* Start CDK Colors */
  initCDKColor();

  /*
   * Create the matrix.  Need to manually cast (const char**) to (char **)
  */
  myMatrix = newCDKMatrix(cdkscreen, CENTER, CENTER, MATRIX_WIDTH, MATRIX_HEIGHT, MATRIX_WIDTH, MATRIX_HEIGHT,
			  MATRIX_NAME_STRING, (char **) columnTitles, (char **) rowTitles, boxWidths,
				     boxTypes, 1, 1, ' ', ROW, true, true, false);

  if (myMatrix == NULL)
    {
      printf("Error creating Matrix\n");
      _exit(1);
    }

  drawCDKMatrix(myMatrix, true);

  boost::thread_group tgroup;
  boost::thread *theThread;

  for (int i = 0; i < NUM_THREADS; i++)
  {
    theThread = new boost::thread(thread_func, i);
    tgroup.add_thread(theThread);
  }

    /* so we can see results */
  sleep (5);

  barrier.wait();	// Release threads
  sleep(5);

  unsigned int accessor, works;		// Thread's ID, picked randomly to add works
  
  // Signal handler
  signal(SIGINT, signalHandler);
  signal(SIGTERM, signalHandler);
  
  while(!stop)
  {
    // Start producing works
    srand(time(NULL));
    accessor = rand() % NUM_THREADS;
    works = rand() % 11 + 10;	// Random # of works between 10 and 20

    addWorks[accessor]->mut.lock();	// Get lock to modify works number
    qty[accessor] += works;	// Add works in
    updateWorks[accessor]->sem.post();	// Signal thread's semaphore that works are updated
    addWorks[accessor]->mut.unlock();	// Release lock after done modifying
    sleep(1);
  }
   
  for (int i = 0; i < NUM_THREADS; i++)
	updateWorks[i]->sem.post();

  tgroup.join_all();	// Join all thread

  // Cleanup screen
  endCDK();

  return 0;
}

void signalHandler(int signum)
{
  if (signum == SIGINT || signum == SIGTERM)
    stop = 1;
}

void update_box(int xpos, int ypos, int thread_id, char thread_status, int qty)
{
  lockUpdate.lock();	// Protect screen update by threads
  stringstream *status = new stringstream();
  *status << "tid:" << thread_id << " S:" << thread_status << " Q:" << qty;
  setCDKMatrixCell(myMatrix, xpos, ypos, (*status).str().c_str());
  drawCDKMatrix(myMatrix, true);

  delete status;
  lockUpdate.unlock();	// Release lock for updating matrix
}

void thread_func(int myID)
{
  int xpos = myID / MATRIX_HEIGHT + 1;
  int ypos = myID % MATRIX_WIDTH + 1;
  update_box(xpos, ypos, myID, 'B', 0);
  barrier.wait();
  // All threads are done creating themselves, change to "wait" status
  update_box(xpos, ypos, myID, 'W', 0);
  unsigned int qty_backup = 1;	// To compare with sudden change of # of works by producer
  int numberNewWorks = 0;	// Number of new works added by producer

  // Start receiving works
  //updateWorks[myID]->sem.wait();
  while(1)
  {
    updateWorks[myID]->sem.wait();
    if (qty[myID] == 0)
    {
      update_box(xpos, ypos, myID, 'E', qty[myID]);
      break;
    }
    else
    {
	    addWorks[myID]->mut.lock();	// Mess with quantities from here
	    if (qty_backup < qty[myID]) // Works have been added by producer, get new semaphore
	    {
	      numberNewWorks = qty[myID] - qty_backup;
	      for (int i = 0; i < numberNewWorks; i++)
		updateWorks[myID]->sem.post();
	    }
	    update_box(xpos, ypos, myID, 'C', qty[myID]);

	    sleep(1);	// Working
	    qty_backup = qty[myID]--;
	    if (qty[myID] == 0)
	      update_box(xpos, ypos, myID, 'W', qty[myID]);
	    addWorks[myID]->mut.unlock();	// Done with messing with qty
     }
  }
}
