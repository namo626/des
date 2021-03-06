
// exported by the application

void handleEvent(void* event);

typedef struct Customer Customer;

double totalWait(Customer* cust);

void freeCustomer(Customer* cust);

void generate(int genID, char* name);


// exported by the customer list structure
typedef struct CustList CustList;

CustList* mkCustList();

int custCount(CustList* clist);

void insertCust(CustList* clist, Customer* cust);

double maxWaitTime(CustList* clist);

double minWaitTime(CustList* clist);

double avgWaitTime(CustList* clist);

void freeCustList(CustList* clist);

// exported by the engine
void initFES();

void freeFES();

void runSim(double time);

void schedule(void* event, double timestamp);

double currentTime();
