/*
 *
 * Copyright 2015, Google Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *     * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 * copyright notice, this list of conditions and the following disclaimer
 * in the documentation and/or other materials provided with the
 * distribution.
 *     * Neither the name of Google Inc. nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#ifndef GRPCXX_IMPL_SYNC_H
#define GRPCXX_IMPL_SYNC_H

#include <grpc/support/sync.h>
#include <grpc/support/thd.h>

namespace grpc {

class lock;
class condition_variable;

class mutex {
 public:
  mutex() { gpr_mu_init(&mu_); }
  ~mutex() { gpr_mu_destroy(&mu_); }
 private:
  ::gpr_mu mu_;
  friend class lock;
  friend class condition_variable;
};

class lock {
 public:
  lock(mutex *mu) : mu_(mu), locked(true) { gpr_mu_lock(&mu->mu_); }
  ~lock() { release(); }
  void acquire() {
    if (!locked) gpr_mu_lock(&mu_->mu_);
    locked = true;
  }
  void release() {
    if (locked) gpr_mu_unlock(&mu_->mu_);
    locked = false;
  }
 private:
  mutex *mu_;
  bool locked;
  friend class condition_variable;
};

class condition_variable {
 public:
  condition_variable() { gpr_cv_init(&cv_); }
  ~condition_variable() { gpr_cv_destroy(&cv_); }
  void wait(lock *mu) {
    mu->locked = false;
    gpr_cv_wait(&cv_, &mu->mu_->mu_, gpr_inf_future);
    mu->locked = true;
  }
  void signal() { gpr_cv_signal(&cv_); }
  void broadcast() { gpr_cv_broadcast(&cv_); }
 private:
  gpr_cv cv_;
};

class thread {
 public:
  template<class T> thread(void (T::*fptr)(), T *obj) {
    func_ = new thread_function<T>(fptr, obj);
    start();
  }
  ~thread() { delete func_; }
  void join() { gpr_thd_join(thd); }
 private:
  void start() {
    gpr_thd_new(&thd, thread_func, (void *) func_, NULL);
  }
  static void thread_func(void *arg) {
    thread_function_base *func = (thread_function_base *) arg;
    func->call();
  }
  class thread_function_base {
   public:
    virtual ~thread_function_base() { }
    virtual void call() = 0;
  };
  template<class T>
  class thread_function : public thread_function_base {
   public:
    thread_function(void (T::*fptr)(), T *obj)
      : fptr_(fptr)
      , obj_(obj) { }
    virtual void call() { (obj_->*fptr_)(); }
   private:
    void (T::*fptr_)();
    T *obj_;
  };
  thread_function_base *func_;
  gpr_thd_id thd;
};

}  // namespace grpc

#endif  // GRPCXX_IMPL_SYNC_H
