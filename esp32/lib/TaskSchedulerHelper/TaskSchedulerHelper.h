#ifndef TASK_SCHEDULER_HELPER_H
#define TASK_SCHEDULER_HELPER_H

#include <Arduino.h>
#include <stdint.h>
#define _TASK_STATUS_REQUEST
#include <TaskScheduler.h>

// Shorthand for initializing tasks
void initTask(Task& task, unsigned long interval, long iterations, TaskCallback callback, TaskOnEnable onEnableCb, TaskOnDisable onDisableCb) {
  task.setInterval(interval);
  task.setIterations(iterations);
  if (callback    != NULL) { task.setCallback(callback); }
  if (onEnableCb  != NULL) { task.setOnEnable(onEnableCb); }
  if (onDisableCb != NULL) { task.setOnDisable(onDisableCb); }
}

void initTask(Task& task, unsigned long interval, long iterations, TaskCallback callback, TaskOnEnable onEnableCb, TaskOnDisable onDisableCb, StatusRequest* trigger, bool triggerDelay = false) {
  if (callback    != NULL) { task.setCallback(callback); }
  if (onEnableCb  != NULL) { task.setOnEnable(onEnableCb); }
  if (onDisableCb != NULL) { task.setOnDisable(onDisableCb); }

  if (triggerDelay == true) {
    task.waitForDelayed(trigger, interval, iterations);
  } else {
    task.waitFor(trigger, interval, iterations);
  }
}

#endif
