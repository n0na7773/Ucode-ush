#include "ush.h"

static int fg_send_signal(Prompt *shell, Process *proc, int pg_id, int job_num) {
    if (kill(-pg_id, SIGCONT) < 0) {
        mx_strjoin_arr("fg", ": job not found: ", proc->argv[1], "\n");
        return 1;
    }

    int stat;
    tcsetpgrp(STDIN_FILENO, pg_id);
    mx_set_job_status(shell, job_num, _STATUS_CONTINUED);
    mx_print_job_status(shell, job_num, 0);
    stat = mx_wait_job(shell, job_num);
    if (mx_job_completed(shell, job_num)) mx_remove_job(shell, job_num);
    signal(SIGTTOU, _SIG_IGN);
    tcsetpgrp(STDIN_FILENO, getpid());
    signal(SIGTTOU, _SIG_DFL);
    tcgetattr(STDIN_FILENO, &shell->jobs[job_num]->tmodes);
    tcsetattr(STDIN_FILENO, TCSADRAIN, &shell->jobs[job_num]->tmodes);
    return stat;
}

static int fg_get_job_num (Prompt *shell, Process *proc) {
    int job_num, args_num = 0;

    for (int i = 0; proc->argv[i] != NULL; i++) args_num++;
    if (args_num > 2) {
        mx_printerr("ush: fg: too many arguments\n");
        return -1;
    } else if (args_num == 1) {
        if ((job_num = shell->jobs_stack->last) < 1) {
            mx_printerr("fg: no current job\n");
            return -1;
        }
    } else {
        if ((job_num = mx_check_args(shell, proc)) < 1) return -1;
    }
    return job_num;
}

int mx_fg(Prompt *shell, Process *proc) {
    int stat, job_num = 0;;
    pid_t pg_id = 0;

    mx_set_last_job(shell);
    if ((job_num = fg_get_job_num(shell, proc)) < 1) return 1;

    if ((pg_id = mx_get_pgid_by_job_num(shell, job_num)) < 1) {
        mx_strjoin_arr(proc->argv[0], ": ", proc->argv[1],": no such job\n");
        return 1;
    }
    
    stat = fg_send_signal(shell, proc, pg_id, job_num);
    return stat;
}
