#include "ush.h"

static int mx_bg_get_job_num(Prompt *shell, Process *proc) {
    int job_num;
    int args = 0;

    for (int i = 0; proc->argv[i] != NULL; i++) args++;
    if (args > 2) {
        mx_printerr("ush: bg: too many arguments\n"); 
        return -1;
    } else if (args == 1 && (job_num = shell->jobs_stack->last) < 1) {
        mx_printerr("bg: no current job\n");
        return -1;
    } else {
        if ((job_num = mx_check_job_args(shell, proc)) < 1) return -1;
    }
    return job_num;
}

int mx_bg(Prompt *shell, Process *proc) {
    pid_t p_gid = 0;
    int job_num = 0;

    mx_set_last_job(shell);
    if ((job_num = mx_bg_get_job_num(shell, proc)) < 1)  return 1;

    if ((p_gid = mx_get_pgid_by_job_num(shell, job_num)) < 1) {
        mx_strjoin_arr(proc->argv[0], ": ", proc->argv[1],": no such job\n");
        return 1;
    }

    if (kill(-p_gid, SIGCONT) < 0) {
        mx_strjoin_arr(proc->argv[0], ": job not found: ", proc->argv[1], "\n");
        return 1;
    }

    mx_set_job_status(shell, job_num, _STATUS_CONTINUED);
    mx_print_job_status(shell, job_num, 0);
    return 0;
}
