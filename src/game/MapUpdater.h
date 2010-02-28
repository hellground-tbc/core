#ifndef _MAP_UPDATER_H_INCLUDED
#define _MAP_UPDATER_H_INCLUDED


#include <ace/Thread_Mutex.h>
#include <ace/Condition_Thread_Mutex.h>

#include "DelayExecutor.h"

class Map;

class MapUpdater
{
public:
  MapUpdater ();
  virtual ~MapUpdater ();

  friend class MapUpdateRequest;

  /// schedule update on a map, the update will start
  /// as soon as possible ,
  /// it may even start before the call returns
  int schedule_update(Map& map, ACE_UINT32 diff);

  /// Wait untill all pending updates finish
  int wait ();

  /// Start the worker threads
  int activate (size_t num_threads);

  /// Stop the worker threads
  int deactivate (void);

  bool activated ();

private:
  /// hook called by worker threads
  void update_finished ();

  DelayExecutor m_executor;
  ACE_Condition_Thread_Mutex m_condition;
  ACE_Thread_Mutex m_mutex;
  size_t pedning_requests;
};

#endif //_MAP_UPDATER_H_INCLUDED