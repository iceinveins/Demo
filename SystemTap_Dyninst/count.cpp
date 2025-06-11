#include <stdio.h>
#include <fcntl.h>
#include "BPatch.h"
#include "BPatch_process.h"
#include "BPatch_function.h"
#include "BPatch_Vector.h"
#include "BPatch_thread.h"
#include "BPatch_point.h"

void usage() {
    fprintf(stderr, "Usage: count <process_id> <function>\n");
}

// Global information for counter
BPatch_variableExpr *counter = NULL;

void createCounter(BPatch_process *app, BPatch_image *appImage) {
    int zero = 0;
    counter = app->malloc(*appImage->findType("int"));
    counter->writeValue(&zero);
}

bool interceptfunc(BPatch_process *app,
                   BPatch_image *appImage,
                   char *funcName) {
    BPatch_Vector<BPatch_function *> func;
    appImage->findFunction(funcName, func);
    if(func.size() == 0) {
        fprintf(stderr, "Unable to find function to instrument()\n");
        exit (-1);
    }
    BPatch_Vector<BPatch_snippet *> incCount;
    BPatch_Vector<BPatch_point *> *points;
    points = func[0]->findPoint(BPatch_entry);
    if ((*points).size() == 0) {
        exit (-1);
    }

    BPatch_arithExpr counterPlusOne(BPatch_plus, *counter, BPatch_constExpr(1));
    BPatch_arithExpr addCounter(BPatch_assign, *counter, counterPlusOne);

    return app->insertSnippet(addCounter, *points);
}

void printCount(BPatch_thread *thread, BPatch_exitType) {
    int val = 0;
    counter->readValue(&val, sizeof(int));
    fprintf(stderr, "Function executed %d times.\n", val);
}

int main(int argc, char *argv[]) {
    int pid;
    BPatch bpatch;
    if (argc != 3) {
        usage();
        exit(1);
    }
    pid = atoi(argv[1]);
    BPatch_process *app = bpatch.processAttach(NULL, pid);
    if (!app) exit (-1);
    BPatch_image *appImage = app->getImage();
    createCounter(app, appImage);
    fprintf(stderr, "Finding function %s(): ", argv[2]);
    BPatch_Vector<BPatch_function*> countFuncs;
    fprintf(stderr, "OK\nInstrumenting function %s(): ", argv[2]);
    interceptfunc(app, appImage, argv[2]);
    bpatch.registerExitCallback(printCount);
    fprintf(stderr, "OK\nWaiting for process %d to exit...\n", pid);
    app->continueExecution();
    while (!app->isTerminated()) {
        bpatch.waitForStatusChange();
    }
    return 0;
}