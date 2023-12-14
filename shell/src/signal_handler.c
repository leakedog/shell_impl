#include "../include/signal_handler.h"


pid_t pids_of_foreground_children[MAX_NUMBER_OF_FOREGROUND_CHILDREN];
pid_t pids_of_background_children[MAX_NUMBER_OF_BACKGROUND_CHILDREN];
int status_background_children[MAX_NUMBER_OF_BACKGROUND_CHILDREN];
int n_foreground_children = 0;
volatile int n_foreground_chilren_alive = 0;
int n_finished_background_children = 0;
sigset_t child_set, not_child_set, old_set;


void SetForegroundChildren(int cnt) {
    n_foreground_children = cnt;
    n_foreground_chilren_alive = 0;
}

void ClearForegroundChildren() {
    SetForegroundChildren(0);
}

void SetForegroundChildPid(int child_id, pid_t cpid) {
    n_foreground_chilren_alive++;
    pids_of_foreground_children[child_id] = cpid;
}

void DecreaseActiveForeignChildren() {
    n_foreground_chilren_alive--;
}

bool IsForegroundChildren(pid_t pid) {
    int is_foreground = 0;
    for(int i = 0; i < n_foreground_children; i++){
        if(pids_of_foreground_children[i] == pid){
            is_foreground = 1;
        }
    }
    return is_foreground;
}

void Handler(int sig) {
    pid_t child;
    int status;
    do {
        
        child = waitpid(-1, &status, WNOHANG);
        if(child <= 0) break;
       
        if(IsForegroundChildren(child)){
            n_foreground_chilren_alive--;
        }
        else{
            pids_of_background_children[n_finished_background_children] = child;
            status_background_children[n_finished_background_children] = status;
            n_finished_background_children++;
        }
    } while(true);
}

void SetDefaultHandler() {
    signal(SIGINT, SIG_DFL);
}

void RunAsBackground() {
    if (setsid() == -1) {
        fprintf(stderr, "lsh: error running in background\n");
        exit(EXEC_FAILURE);
    }
}

void WaitForegroundChildren() {
    while(n_foreground_chilren_alive){
        sigset_t orig_mask;
        sigprocmask(SIG_BLOCK, &not_child_set, &orig_mask);
        sigsuspend(&not_child_set);
        sigprocmask(SIG_SETMASK, &orig_mask, NULL);
    }
}


void SetDefaultBlock() {
    sigprocmask(SIG_SETMASK, &old_set, NULL);
}

void SetChildBlock() {
    sigprocmask(SIG_BLOCK, &child_set, &old_set);
}

void InitializeSignals() {
    /// Ignore signals
    signal(SIGINT, SIG_IGN);
    /// Set signal sets
    struct sigaction act;
    act.sa_handler = Handler;
    act.sa_flags = SA_RESTART;
    sigemptyset(&act.sa_mask);
    sigemptyset(&child_set);
    sigaddset(&child_set, SIGCHLD);
    sigfillset(&not_child_set);
    sigdelset(&not_child_set, SIGCHLD);
    sigaction(SIGCHLD, &act, NULL);
    sigprocmask(SIG_BLOCK, &child_set, &old_set);
    SetChildBlock();
}

void PrintAllFinishedSignals() {
    for(int i = 0; i < n_finished_background_children; i++){
        if(WIFSIGNALED(status_background_children[i])) {
            fprintf(stdout, "Background process (%d) terminated. (killed by signal (%d))\n",
                    pids_of_background_children[i], status_background_children[i]);
        }
        else{
            fprintf(stdout, "Background process (%d) terminated. (exited with status (%d))\n",
                    pids_of_background_children[i], status_background_children[i]);
        }
    }
    n_finished_background_children = 0;
}