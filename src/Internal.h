
// exported by the application

void handleEvent(void* event);


// exported by the engine
void initFES();

void runSim(double time);

void schedule(void* event, double timestamp);

double currentTime();
