#ifndef TEEC_TEST_CASE_LOGGER_H
#define TEEC_TEST_CASE_LOGGER_H

#include <stddef.h>
#include <tee_client_api.h>

extern TEEC_UUID last_uuid;

void log_test_case(TEEC_UUID *uuid, uint32_t cmd_id, TEEC_Operation *op);

#endif /*TEEC_TEST_CASE_LOGGER_H*/
