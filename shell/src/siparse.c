#include "../include/siparse.h"
#include "../input_parse/siparseutils.h"
#include "../include/utils.h"


void FailedWhileParsing() {
    perror(SYNTAX_ERROR_STR);
    exit(EXIT_FAILURE);
}

const char* FindFirst(const char* start, const char* end, char c) {
    while (start != end && *start != c) {
        ++start;
    }
    return start;
}


void SkipTabs(const char** cur_it, const char* end) {
    while (*cur_it != end && **cur_it == '\t') {
        ++(*cur_it);
    }
}

const char* AppendRedir(command* command, const char* start_it, const char* buf_end, redir* redir) {
    
    SkipTabs(&start_it, buf_end);

    if (start_it == buf_end) {
        return NULL;
    }

    const char* new_it = FindFirst(start_it, buf_end, '\t');
    
    redir->filename = copytobuffer(start_it, new_it - start_it);

    append_to_redirs(command->redirs, redir);

    return new_it;
}



command* ParseCommand(const char* buf_start, const char* buf_end) {
    command* cur_command = new_command();

    for (const char* it = buf_start; it != buf_end; ++it) {

        SkipTabs(&it, buf_end);

        if (it == buf_end) {
            break;
        }

        char c = *it;
        printf("%c\n", c);

        if (c == '>' || c == '<') {
            int redir_type = ROUT;
            const char* start_it = it + 1;
            if (c == '<') {
                redir_type = ROUT;
                if ((it + 1) != buf_end && *(it + 1) == '<') {
                    redir_type = RAPPEND;
                    start_it = it + 2;
                }
            } else {
                redir_type = RIN;
            }

            redir* redir = new_redir();
            redir->flags = redir_type;
            it = AppendRedir(cur_command, start_it, buf_end, redir);

            if (it == NULL) {
                return NULL;
            }

        } else {
            /// that's an argument
            const char* end_it = FindFirst(it, buf_end, '\t');
            char* arg = copytobuffer(it, end_it - it);
            append_to_args(cur_command->args, arg);
            it = end_it;
        }
    }
    

    return cur_command;
}

pipelineseq* parseline(char* temp_buffer) {

    resetutils();
    
    int last_line_of_command = 0;
    pipeline* cur_pipeline = NULL;
    pipelineseq* pipeline_seq = NULL;
    for (int char_id = 0; char_id < MAX_LINE_LENGTH; char_id++) {
        char c = temp_buffer[char_id];
        if (c == '|') {
            command* command = ParseCommand(temp_buffer + last_line_of_command, temp_buffer + char_id);
            if (cur_pipeline == NULL) {
                cur_pipeline = start_pipeline(command);
            } else {
                cur_pipeline = append_to_pipeline(cur_pipeline, command);
            }
        } else if (c == ';') {
            if (cur_pipeline == NULL) {
                continue;
            }
            if (pipeline_seq == NULL) {
                pipeline_seq = start_pipelineseq(cur_pipeline);
            } else {
                pipeline_seq = append_to_pipelineseq(pipeline_seq, cur_pipeline);
            }
            cur_pipeline = NULL;
        }
    }

    return pipeline_seq;
}