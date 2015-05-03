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

#include <grpc/support/port_platform.h>

#ifdef GPR_WINSOCK_SOCKET

#include <grpc/support/alloc.h>
#include <grpc/support/log.h>

#include "src/core/iomgr/iocp_windows.h"
#include "src/core/iomgr/iomgr.h"
#include "src/core/iomgr/iomgr_internal.h"
#include "src/core/iomgr/pollset.h"
#include "src/core/iomgr/pollset_windows.h"
#include "src/core/iomgr/socket_windows.h"

grpc_winsocket *grpc_winsocket_create(SOCKET socket) {
  grpc_winsocket *r = gpr_malloc(sizeof(grpc_winsocket));
  memset(r, 0, sizeof(grpc_winsocket));
  r->socket = socket;
  gpr_mu_init(&r->state_mu);
  grpc_iomgr_ref();
  grpc_iocp_add_socket(r);
  return r;
}

/* Call the pending operation callbacks to abort them. Holding the socket's
   mutex state should be fine, as we shouldn't be trying to lock it again
   in that abortion case, as it'd mean otherwise we're queuing another
   operation, which is senseless. */
static void shutdown_op(grpc_winsocket_callback_info *info) {
  if (!info->cb) return;
  info->cb(info->opaque, 0);
}

static void async_socket_shutdown(void *info, int success) {
  grpc_winsocket *socket = (grpc_winsocket *) info;
  gpr_mu_lock(&socket->state_mu);
  shutdown_op(&socket->read_info);
  shutdown_op(&socket->write_info);
  gpr_mu_unlock(&socket->state_mu);
}

/* Schedule a shutdown of the socket operations. Will call the pending
   operations to abort them. We need to do that this way because of the
   various callsites of that function, which happens to be in various
   mutex hold states. */
void grpc_winsocket_shutdown(grpc_winsocket *socket) {
  grpc_iomgr_add_delayed_callback(async_socket_shutdown, socket, 0);
}

/* Abandons a socket. Either we're going to queue it up for garbage collecting
   from the IO Completion Port thread, or destroy it immediately. */
void grpc_winsocket_orphan(grpc_winsocket *winsocket) {
  SOCKET socket = winsocket->socket;
  if (!winsocket->closed_early) {
    grpc_iocp_socket_orphan(winsocket);
  }
  if (winsocket->closed_early) {
    grpc_winsocket_destroy(winsocket);
  }
  closesocket(socket);
  grpc_iomgr_unref();
}

void grpc_winsocket_destroy(grpc_winsocket *winsocket) {
  gpr_mu_destroy(&winsocket->state_mu);
  gpr_free(winsocket);
}

#endif  /* GPR_WINSOCK_SOCKET */
