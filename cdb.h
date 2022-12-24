//
// Created by Neeraj "n4j" Shah on 12/24/22.
//

#ifndef CDB2_CDB_H
#define CDB2_CDB_H

#include <cstdlib>

/**
 *  _____________________
 * | macro declarations |
 * ---------------------
 */

#define COLUMN_USERNAME_SIZE 32

#define COLUMN_EMAIL_SIZE 255

#define size_of(S, A) sizeof(((S*)0)->A)

#define TABLE_MAX_PAGES 100

/**
 *  ______________________
 * | struct declarations |
 * ----------------------
 */

typedef struct  {
    char* buffer;
    size_t buffer_length;
    ssize_t input_length;

} InputBuffer;

typedef enum {
    META_COMMAND_SUCCESS,
    META_COMMAND_UNRECOGNIZED_COMMAND
} MetaCommandResult;

typedef enum {
    PREPARE_SUCCESS,
    PREPARE_SYNTAX_ERROR,
    PREPARE_UNRECOGNIZED_STATEMENT
} PrepareResult;

typedef enum {
    STATEMENT_INSERT,
    STATEMENT_SELECT
} StatementType;

typedef struct {
    uint32_t  id;
    char username[COLUMN_USERNAME_SIZE];
    char email[COLUMN_EMAIL_SIZE];
} Row;

typedef struct {
    StatementType type;
    Row row;
} Statement;

/**
 *  _______________________
 * | constant declarations|
 * -----------------------
 */

// field sizes
const uint32_t ID_SIZE = size_of(Row, id);
const uint32_t USERNAME_SIZE = size_of(Row, username);
const uint32_t EMAIL_SIZE = size_of(Row, email);

// field offsets
const uint32_t ID_OFFSET = 0; // first field so offset is 0
const uint32_t USERNAME_OFFSET = ID_OFFSET + ID_SIZE; // offset of previous + size of previous
const uint32_t EMAIL_OFFSET = USERNAME_OFFSET+ USERNAME_SIZE; // offset of previous + size of previous

const uint32_t ROW_SIZE = (ID_SIZE + USERNAME_SIZE + EMAIL_SIZE);

const uint32_t PAGE_SIZE = 4096;

const uint32_t ROWS_PER_PAGE = (PAGE_SIZE / ROW_SIZE);

const uint32_t TABLE_MAX_ROWS = (ROWS_PER_PAGE * TABLE_MAX_PAGES);

typedef struct {
    uint32_t num_rows;
    uint8_t* pages[TABLE_MAX_PAGES];
} Table;
/**
 *  ______________________
 * | method declarations |
 * ----------------------
 */

InputBuffer* new_input_buffer();

void print_prompt();

void read_input(InputBuffer* buffer);

void close_input_buffer(InputBuffer* buffer);

MetaCommandResult do_meta_command(InputBuffer* input_buffer);

PrepareResult prepare_statement(InputBuffer* input_buffer, Statement* statement);

void execute_statement(InputBuffer* input_buffer, Statement* statement);

void serialize_row(Row* source, void* destination);

void* row_slot(Table* table, uint32_t row_num);

#endif //CDB2_CDB_H
