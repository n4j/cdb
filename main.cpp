#include <cstring>
#include <iostream>
#include <cstdio>

#include "cdb.h"

int main() {

   InputBuffer* input_buffer = new_input_buffer();

   while(true) {

       printf("Size type [%lu]", sizeof(size_t));

       print_prompt();
       read_input(input_buffer);

       if('.' == input_buffer->buffer[0]) {
           switch(do_meta_command(input_buffer)) {
               case META_COMMAND_SUCCESS:
                   break;
               case META_COMMAND_UNRECOGNIZED_COMMAND:
                   printf("Unrecognized meta command [%s]", input_buffer->buffer);
                   continue;
           }
       }

       Statement statement;
       switch(prepare_statement(input_buffer, &statement)) {
           case PREPARE_SUCCESS:
               printf("[INFO] - Statement prepared\n");
               break;
           case PREPARE_SYNTAX_ERROR:
               printf("Invalid number of parameters in statement [%s], expected 3 parameters\n",
                      input_buffer->buffer);
               continue;
           case PREPARE_UNRECOGNIZED_STATEMENT:
               printf("Unrecognized statement [%s]\n", input_buffer->buffer);
               continue;
       }

       execute_statement(input_buffer, &statement);
   }
}

InputBuffer* new_input_buffer() {
    auto *buffer = (InputBuffer*)malloc(sizeof(InputBuffer));
    buffer->buffer = nullptr;
    buffer->input_length = 0;
    buffer->buffer_length = 0;
    return buffer;
}

void print_prompt() {
    printf("sql> ");
}

void read_input(InputBuffer* buffer) {
   ssize_t bytes_read =
           getline(&(buffer->buffer), &(buffer->buffer_length), stdin);

   if(bytes_read < 0) {
       perror("read: ");
       exit(EXIT_FAILURE);
   }

   buffer->input_length = bytes_read;
   // replace trailing newline with null terminator
   buffer->buffer[bytes_read-1] = 0;
}

void close_input_buffer(InputBuffer* buffer) {
    free(buffer->buffer);
    buffer->buffer = nullptr;
    buffer->input_length = 0;
    buffer->buffer_length = 0;
    free(buffer);
}

MetaCommandResult do_meta_command(InputBuffer* input_buffer) {
    if(strcmp(input_buffer->buffer, ".exit") == 0) {
        close_input_buffer(input_buffer);
        exit(EXIT_SUCCESS);
    }
    return META_COMMAND_UNRECOGNIZED_COMMAND;
}

PrepareResult prepare_statement(InputBuffer* input_buffer, Statement* statement) {
    if(0 == strncmp(input_buffer->buffer, "select", 6)) {
        statement->type = STATEMENT_SELECT;
        return PREPARE_SUCCESS;
    }

    if(0 == strncmp(input_buffer->buffer, "insert", 6)) {
        int args_assigned = sscanf(input_buffer->buffer,
                                   "insert %d %s %s",
                                   &(statement->row.id),
                                   statement->row.email,
                                   statement->row.username
        );
        printf("[DEBUG] - args assigned [%d]\n", args_assigned);
        if(args_assigned < 3) {
            return PREPARE_SYNTAX_ERROR;
        }
        statement->type = STATEMENT_INSERT;
        return PREPARE_SUCCESS;
    }

    return PREPARE_UNRECOGNIZED_STATEMENT;
}

void execute_statement(InputBuffer* input_buffer, Statement* statement) {
    switch(statement->type) {
        case STATEMENT_SELECT:
            printf("Upgrade your subscription to execute SELECT statement\n");
            break;

        case STATEMENT_INSERT:
            printf("Upgrade your subscription to execute INSERT statement\n");
            break;
    }
}

void serialize_row(Row* source, uint8_t* destination) {
    memcpy(destination, &(source->id), ID_SIZE);
    memcpy((destination + USERNAME_OFFSET), &(source->username), USERNAME_SIZE);
    memcpy((destination + EMAIL_OFFSET), &(source->email), EMAIL_SIZE);
}

void deserialize_row(uint8_t* source, Row* destination) {
    memcpy(&(destination->id), source, ID_SIZE);
    memcpy(&(destination->username),(source + USERNAME_OFFSET), USERNAME_SIZE);
    memcpy(&(destination->email), (source + EMAIL_OFFSET), EMAIL_SIZE);
}

void* row_slot(Table* table, int row_num) {
    auto page_num = row_num / ROWS_PER_PAGE;
    auto* page = table->pages[page_num];
    if (nullptr == page) {
       table->pages[page_num] =  page = (uint8_t*)malloc(PAGE_SIZE);
    }

    // get the offset within a page, first
    // within a page, what would be the slot for this row?
    auto row_offset = row_num % ROWS_PER_PAGE;

    // translate slot number to an offset from the start of the page
    auto byte_offset =  row_offset * ROW_SIZE;
    return (page + byte_offset);
}