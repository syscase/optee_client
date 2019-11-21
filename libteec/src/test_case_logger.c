/*
 * Test case logger
 */

#include "test_case_logger.h"
#include "afl_call.h"

#include <tee_client_api.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

TEEC_UUID last_uuid = { 0 };

void log_test_case(TEEC_UUID *uuid, uint32_t cmd_id, TEEC_Operation *op) {
  char *buffer;
  const char header[] = "[{\"target\":\"smc_driver\",\"calls\":[[{\"type\":\"buffer\",\"offset\":0,\"size\":16516}]]}]\xb7\xe3";
  size_t tmpref_buffer_size = 4097;
  size_t header_size =  sizeof(header) - 1;
  size_t uuid_size = sizeof(*uuid);
  size_t cmd_id_size = sizeof(uint32_t);
  size_t op_size = sizeof(*op);
  size_t op_params_count = sizeof(op->params)/sizeof(op->params[0]);
  size_t memref_size = tmpref_buffer_size;
  size_t size = header_size + uuid_size + sizeof(uint32_t) + op_size + op_params_count * memref_size + op_params_count * tmpref_buffer_size;

#ifdef SYSCASE_DEBUG
  printf("Allocate buffer\n");
#endif

  buffer = malloc(size);
  memset(buffer, 0, size);

#ifdef SYSCASE_DEBUG
  printf("Copy to buffer\n");
#endif

  memcpy(buffer, &header, header_size);
  if (uuid) {
    memcpy(buffer + header_size, uuid, uuid_size);
  }
  memcpy(buffer + header_size + uuid_size, &cmd_id, cmd_id_size);
  if (op) {
    memcpy(buffer + header_size + uuid_size + cmd_id_size, op, op_size);

#ifdef SYSCASE_DEBUG
    printf("Copy params to buffer\n");
#endif

    for(unsigned int i = 0; i < op_params_count; i++) {
      size_t buffer_size = tmpref_buffer_size;

      if (op->params[i].tmpref.size <= tmpref_buffer_size) {
        buffer_size = op->params[i].tmpref.size;

#ifdef SYSCASE_DEBUG
        printf("Source buffer size: %ld\n", op->params[i].tmpref.size);
        printf("Copy buffer size: %ld\n", buffer_size);
#endif

      } else {
        printf("Can not store buffer: Source buffer size %ld > target buffer size %ld\n", op->params[i].tmpref.size, tmpref_buffer_size);
      }

      size_t current_memref_size = memref_size;

      if (op->params[i].memref.size <=memref_size) {
        current_memref_size = op->params[i].memref.size;

#ifdef SYSCASE_DEBUG
        printf("Source memref size: %ld\n", op->params[i].memref.size);
        printf("Copy memref size: %ld\n", current_memref_size);
#endif

      } else {
        printf("Can not store memref: Source memref size %ld > target memref size %ld\n", op->params[i].memref.size, current_memref_size);
      }
      
      if(op->params[i].memref.parent && op->params[i].memref.size > 0) {
#ifdef SYSCASE_DEBUG
        printf("Copy memref\n");
#endif
        memcpy(buffer + header_size + uuid_size + cmd_id_size + op_size + i * memref_size, op->params[i].memref.parent, current_memref_size);
      }

      if(op->params[i].tmpref.buffer && buffer_size > 0) {
#ifdef SYSCASE_DEBUG
        printf("Copy tmpref\n");
#endif

        memcpy(buffer + header_size + uuid_size + cmd_id_size + op_size + op_params_count * memref_size + i * tmpref_buffer_size, op->params[i].tmpref.buffer, buffer_size);
      }
    }
  }

#ifdef SYSCASE_DEBUG
  printf("Write log file\n");
#endif

  log_file(buffer, size);
  free(buffer);

#ifdef SYSCASE_DEBUG
  printf("Finished writing log file\n");
#endif
}

