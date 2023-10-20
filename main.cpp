
/*

Version : 1.0 

Oct 13th 2023

*/



#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

using namespace std;

#define NEW 0
#define READY 1
#define RUNNING 2
#define WAITING 3
#define TERMINATING 4

const char* stateNames[] = {"NEW", "READY", "RUNNING", "WAITING", "TERMINATING"};

struct PCB {
    int pid;
    int arrival_time;
    int total_cpu_time;
    int io_frequency;
    int io_duration;
    int remaining_cpu_time;
    int state;
    int oldState;
};

// Function to handle state transitions
pair<string, string> handleStateTransition(PCB* process, int newState) {
    int oldState = process->state;

    cout << "Process " << process->pid;

    if (oldState == newState) {
        cout << " remains in " << stateNames[oldState] << "." << endl;
        return {stateNames[oldState], stateNames[newState]};
    }

    // handles transitions for running state
    if (oldState == RUNNING) {
        if (newState == TERMINATING) {
            cout << " transitions from " << stateNames[oldState] << " to " << stateNames[newState] << "." << endl;
        } else if (newState == WAITING) {
            cout << " transitions from " << stateNames[oldState] << " to " << stateNames[newState] << "." << endl;
        } else {
            cerr << "invalid state" << endl;
            return {"ERROR", "ERROR"};
        }
    } else {
        
        cout << " transitions from " << stateNames[oldState] << " to " << stateNames[newState] << "." << endl;
    }

    process->oldState = oldState;
    process->state = newState;

    return {stateNames[oldState], stateNames[newState]};
}

int main() {
    // read input file
    vector<PCB*> processList;

    ifstream file("test_case_2.csv");

    if (!file.is_open()) {
        cerr << "unable to open file" << endl;
        return -1;
    }

    string line;
    while (getline(file, line)) {
        PCB* p = new PCB;

        stringstream ss(line);
        string token;

        cout << "processing line " << line << endl;

        getline(ss, token, ',');
        try {
            p->pid = stoi(token);
        } catch (const std::invalid_argument& e) {
            cerr << "error converting PID to integer " << e.what() << endl;
            continue;
        }

        getline(ss, token, ',');
        p->arrival_time = stoi(token);

        getline(ss, token, ',');
        p->total_cpu_time = stoi(token);

        getline(ss, token, ',');
        p->io_frequency = stoi(token);

        getline(ss, token, ',');
        p->io_duration = stoi(token);

        // set initial state to READY for processes with arrival_time of 0
        p->state = (p->arrival_time == 0) ? READY : NEW;

        //set initial oldState to the same as the initial state
        p->oldState = p->state;

        // initialize remaining CPU time
        p->remaining_cpu_time = p->total_cpu_time;

        processList.push_back(p);
    }

    file.close();
    cout << "Time " << " PID " << "Old State  " << " New State " << endl;

    // main simulation loop
    int currentTime = 0;
    while (!processList.empty()) {

        // update states and state transitions
        auto it = processList.begin();
        while (it != processList.end()) {
            PCB* process = *it;

            if (process->arrival_time == currentTime) {

                // check if the process is in the NEW state
                if (process->state == NEW) {
                    auto stateTransition = handleStateTransition(process, READY);
                    if (stateTransition.first != stateTransition.second) {
                        cout << "Time: " << currentTime << " PID: " << process->pid
                             << " Old State: " << stateTransition.first
                             << " New State: " << stateTransition.second << endl;
                    }
                }
                // check if the process is in the READY state
                else if (process->state == READY) {
                    
                    if (process->remaining_cpu_time > 0) {
                        auto stateTransition = handleStateTransition(process, RUNNING);
                        if (stateTransition.first != stateTransition.second) {
                            cout << "Time: " << currentTime << " PID: " << process->pid
                                 << " Old State: " << stateTransition.first
                                 << " New State: " << stateTransition.second << endl;
                        }
                    } else {
                        auto stateTransition = handleStateTransition(process, WAITING);
                        if (stateTransition.first != stateTransition.second) {
                            cout << "Time: " << currentTime << " PID: " << process->pid
                                 << " Old State: " << stateTransition.first
                                 << " New State: " << stateTransition.second << endl;
                        }
                    }
                }
                // check if the process is in the WAITING state
                else if (process->state == WAITING) {
                    // handle I/O completion and transition to READY, RUNNING, or TERMINATING
                    process->io_duration--;
                    if (process->io_duration == 0) {
                        if (process->remaining_cpu_time > 0) {
                            auto stateTransition = handleStateTransition(process, RUNNING);
                            if (stateTransition.first != stateTransition.second) {
                                cout << "Time: " << currentTime << " PID: " << process->pid
                                     << " Old State: " << stateTransition.first
                                     << " New State: " << stateTransition.second << endl;
                            }
                        } else {
                            auto stateTransition = handleStateTransition(process, TERMINATING);
                            if (stateTransition.first != stateTransition.second) {
                                cout << "Time: " << currentTime << " PID: " << process->pid
                                     << " Old State: " << stateTransition.first
                                     << " New State: " << stateTransition.second << endl;
                            }
                            // remove terminating from list
                            it = processList.erase(it);
                            delete process;
                            continue;
                        }
                    }
                }
                // check if the process is in the RUNNING state
                else if (process->state == RUNNING) {
                    // handle burst completion or preemption
                    process->remaining_cpu_time--;
                    if (process->remaining_cpu_time == 0) {
                        auto stateTransition = handleStateTransition(process, TERMINATING);
                        if (stateTransition.first != stateTransition.second) {
                            cout << "Time: " << currentTime << " PID: " << process->pid
                                 << " Old State: " << stateTransition.first
                                 << " New State: " << stateTransition.second << endl;
                        }
                        // remove the terminating process from the list
                        it = processList.erase(it);
                        delete process;
                        continue;
                    } else {
                        
                        auto stateTransition = handleStateTransition(process, READY);
                            if (stateTransition.first != stateTransition.second) {
                            cout << "Time: " << currentTime << " PID: " << process->pid
                            << " Old State: " << stateTransition.first
                            << " New State: " << stateTransition.second << endl;
                            }
                            }
                            }
                            }
                                    ++it;
                    }

                    // prints the state of each process at the end of the time unit
                    for (PCB* process : processList) {
                        cout << "       " << currentTime << "       " << process->pid
                            << "       " << stateNames[process->oldState]
                            << "       " << stateNames[process->state] << endl;

                        // resets the old state after printing
                        process->oldState = process->state;
                    }

                   
                    ++currentTime;

                    if (currentTime == 25) {
                        break;
                    }
                }

                //deallocates memory of PCB
                for (PCB* process : processList) {
                    delete process;
                }

                return 0;
}