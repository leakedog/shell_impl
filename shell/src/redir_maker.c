#include "../include/redir_maker.h"

int RunRedirs(command* command, int* rin_file, int* rout_file) {

    if (command && command->redirs) {
        redirseq* redir_seq = command->redirs;
        do {
            redir* cur_redir = redir_seq->r;
            bool was_error = false;
            if (IS_RIN(cur_redir->flags)) {
                *rin_file = open(cur_redir->filename, O_RDONLY);
                if (*rin_file == -1) {
                    was_error = true;
                }
            }

            if(IS_RAPPEND(cur_redir->flags) || IS_ROUT(cur_redir->flags)){
                if(!IS_RAPPEND(cur_redir->flags)) {
                    *rout_file = open(cur_redir->filename,  O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
                    if (*rout_file == -1) {
                        was_error = true;
                    }
                }
                else{
                    *rout_file = open(cur_redir->filename,  O_WRONLY | O_CREAT | O_APPEND, S_IRUSR | S_IWUSR);
                    if (*rout_file == -1) {
                        was_error = true;
                    }
                }
            }

            if (was_error) {
                 if(errno == EACCES || errno == EPERM){
                    fprintf(stderr, "%s: permission denied\n" , cur_redir->filename);
                    errno = 0;
                    return REDIR_ERROR;
                }
                if(errno == ENOENT){
                    fprintf(stderr, "%s: no such file or directory\n", cur_redir->filename);
                    errno = 0;
                    return REDIR_ERROR;
                }
            }
            
            redir_seq = redir_seq->next;
        } while(redir_seq != command->redirs);
    }

    return EXIT_SUCCESS;
}

void FinishRedirs(int rin_file, int rout_file, int original_stdin, int original_stdout) {
    fflush(stdout);
    printf("closing %d %d\n", rin_file, rout_file);
    close(rout_file);
    close(rin_file);
    if (rin_file != -1) {
        dup2(original_stdin, fileno(stdin));
        close(original_stdin);
    }
    if (rout_file != -1) {
        dup2(original_stdout, fileno(stdout));
        close(original_stdout);
    }
}