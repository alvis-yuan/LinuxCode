#define MAXLINE 80
int main()
{
    int n;
    int fd[2];
    pid_t pid;
    char line[MAXLINE];
    if (pipe(fd) < 0)
        perror("pipe");
    if ((pid = fork()) < 0)
        perror("fork");
    if (pid > 0)
    {
        //father
        close(fd[0]);
    }
    else
    {
        close(fd[1]);
        n = read(fd[0], line, MAXLINE);
        write(stdout, line, n);//写到标准输出上看一下效果
    }
    return 0;

}
