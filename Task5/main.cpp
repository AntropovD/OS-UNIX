#include <algorithm>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/select.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <vector>
#include <unistd.h>

#define BLOCK_SIZE 6

struct matrix {
    std::vector<int> data;
    int size;

    struct matrix get_submatrix(int a, int b, int n) {
        struct matrix result;
        result.data.assign(BLOCK_SIZE * BLOCK_SIZE, 0);

        for (int i = 0; i < BLOCK_SIZE; i++)
             for (int j = 0; j < BLOCK_SIZE; j++)
                 result.data[(i * BLOCK_SIZE) + j] = this->data[(a+i) * this->size + b + j];

        result.size = n;
        return result;
    }

    void print(FILE *fd) {
        fprintf(fd, "%d\n", this->size);
        for (int i = 0; i < this->size; i++) {
            for (int j = 0; j < this->size; j++)
                fprintf(fd, "%d ", this->data[i * this->size + j]);
            fprintf(fd, "\n");
        }
    }

    void read_from_fd(FILE *fd) {
        int n, tmp;
        fscanf(fd, "%d", &n);
        for (int i=0; i<n*n; i++) {
            fscanf(fd, "%d", &tmp);
            this->data.push_back(tmp);
        }
        this->size = n;
    }
};

struct my_pipe {
    int read;
    int write;
};

void get_input_data(char *filename);
void update_input_data();
void update_matrix(matrix *M);

void extract_blocks();
void send_sub_matrixes_to_handles(int x, int y, struct matrix M1, struct matrix M2);
void handle_matrixes(struct my_pipe p);
struct matrix multiply_matrix(struct matrix M1, struct matrix M2);

void wait_all_processes();

void select_read(int fd);
void select_write(int fd);

void print_result(char *filename);
struct matrix build_matrix_from_parts();

std::vector<my_pipe> pipes;
std::vector<pid_t> pids;

struct matrix matrix1;
struct matrix matrix2;

int old_size;
int file_exist (char *filename);

int main(int argc, char **argv)
{
    if (argc != 3) {
        printf("Parallel matrix multiplication\n");
        printf("Usage: %s input result\n", argv[0]);
        return 0;
    }

    if (!file_exist(argv[1])) {
        printf("File doesnt exist\n");
        exit(1);
    }

    get_input_data(argv[1]);

    extract_blocks();

    wait_all_processes();

    print_result(argv[2]);
    return 0;
}

void get_input_data(char *filename)
{
    FILE *fd = fopen(filename, "r");

    matrix1.read_from_fd(fd);    
    matrix2.read_from_fd(fd);

    update_input_data();
    fclose(fd);
}

void update_input_data()
{
    update_matrix(&matrix1);
    update_matrix(&matrix2);
}

// Если размеры матрицы не делятся на BLOCK_SIZE
// то добьем нулевыми значениями
void update_matrix(matrix *M)
{
    int shift_value = BLOCK_SIZE - M->size % BLOCK_SIZE;
    int n = M->size;
    old_size = n;
    std::vector<int> new_data;

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            new_data.push_back(M->data[i * M->size + j]);
        }
        for (int j = 0; j < shift_value; j++)
            new_data.push_back(0);
    }

    for (int i = 0; i < shift_value; i++)
        for (int j = 0; j < n + shift_value; j++)
             new_data.push_back(0);
    M->data.clear();

    M->data = new_data;
    M->size = n + shift_value;

}

void extract_blocks()
{
    int n = matrix1.size;
    for (int i = 0; i < n; i+= BLOCK_SIZE) {
        for (int j = 0; j < n; j += BLOCK_SIZE) {
            struct matrix sub_matrix1 = matrix1.get_submatrix(i, j, BLOCK_SIZE);
            struct matrix sub_matrix2 = matrix2.get_submatrix(i, j, BLOCK_SIZE);

            send_sub_matrixes_to_handles(i, j, sub_matrix1, sub_matrix2);
        }
    }    
}

void send_sub_matrixes_to_handles(int x, int y, matrix M1, matrix M2)
{
    int fd[2];

    pid_t childpid;
    pipe(fd);
    if((childpid = fork()) == -1) {
        printf("Error! Cannot fork\n");
        exit(1);
    }
    my_pipe pipe;
    pipe.read = fd[0];
    pipe.write = fd[1];

    if(childpid == 0) {
        handle_matrixes(pipe);
        exit(0);
    } else {        
        FILE *fd = fdopen(pipe.write, "w");
        fprintf(fd, "%d %d\n", x, y);
        M1.print(fd);
        M2.print(fd);
        fclose(fd);

        pipes.push_back(pipe);
        pids.push_back(childpid);
    }
}

void handle_matrixes(my_pipe p)
{
    struct matrix M1;
    struct matrix M2;
    int x, y;

    select_read(p.read);
    FILE *fd = fdopen(p.read, "r");
    fscanf(fd, "%d %d", &x, &y);

    M1.read_from_fd(fd);
    M2.read_from_fd(fd);
    fclose(fd);

    struct matrix result;
    result = multiply_matrix(M1, M2);

    select_write(p.write);
    FILE *fd2 = fdopen(p.write, "w");

    fprintf(fd2, "%d %d\n", x, y);
    result.print(fd2);

    fclose(fd2);
}

struct matrix multiply_matrix(struct matrix M1, struct matrix M2)
{
    struct matrix result;
    int n = M1.size;
    result.data.assign(n * n, 0);

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            int tmp = 0;
            for (int k =0; k < n;  k++)
                tmp += M1.data[i * n + k] * M2.data[k * n + j];
            result.data[i*n + j] = tmp;
        }
    }
    result.size = n;
    return result;
}

void wait_all_processes()
{
    wait(NULL);
}


void select_read(int fd) {
    fd_set rfds;
    struct timeval tv;
    int retval;

    FD_ZERO(&rfds);
    FD_SET(fd, &rfds);

    tv.tv_sec = 5;
    tv.tv_usec = 0;

    retval = select(1024, &rfds, NULL, NULL, &tv);

    if (!retval) {
        printf("Select read error\n");
        exit(2);
    }
}

void select_write(int fd) {
    fd_set wfds;
    struct timeval tv;
    int retval;

    FD_ZERO(&wfds);
    FD_SET(fd, &wfds);

    tv.tv_sec = 5;
    tv.tv_usec = 0;

    retval = select(1024, NULL, &wfds, NULL, &tv);

    if (!retval) {
        printf("Select write error\n");
        exit(2);
    }
}

void print_result(char *filename)
{
    struct matrix result = build_matrix_from_parts();
    result.print(fopen(filename, "w"));
}

struct matrix build_matrix_from_parts()
{
    struct matrix result;
    int n = old_size;
    result.size = n;
    result.data.assign(n * n, 0);

    for(std::vector<my_pipe>::iterator it = pipes.begin(); it != pipes.end(); it++) {
        my_pipe pipe = *it;

        FILE *fd = fdopen(pipe.read, "r");
        int x, y;
        fscanf(fd, "%d %d", &x, &y);

        struct matrix sub_matrix;
        sub_matrix.read_from_fd(fd);

        for (int i = 0 ; i < BLOCK_SIZE; i++)
            for (int j = 0; j < BLOCK_SIZE; j++)
                if (i + x < n && j + y < n)
                    result.data[(i+x) * n + j+y] = sub_matrix.data[i*BLOCK_SIZE + j];
        fclose(fd);
    }

    return result;
}

int file_exist (char *filename)
{
    struct stat buffer;
    return (stat (filename, &buffer) == 0);
}

/*
void handle_matrixes(int file_descriptor)
{
    fd_set rfds;
    struct timeval tv;
    int retval;

    FD_ZERO(&rfds);
    FD_SET(file_descriptor, &rfds);

    tv.tv_sec = 5;
    tv.tv_usec = 0;

    FILE *fd = fopen("asd", "w");
    retval = select(1, &rfds, NULL, NULL, &tv);

    if (retval == -1)
        perror("select()");
    else if (retval)
    {
        fprintf(fd, "Data is available now.\n");
        char buffer[40];
        int n = read(file_descriptor, buffer, 40);

        fprintf(fd, "%s", buffer);
    }
    else
        fprintf(fd, "No data within five seconds.\n");

    fclose(fd);
}
*/
//printf("%d %d - %d %d\n", i, j, std::min(n, i + BLOCK_SIZE), std::min(n, j + BLOCK_SIZE));
/*
fd_set wfds;
struct timeval tv;
int retval();

FD_ZERO(&wfds);
*/
/*
int fd[2];
pid_t childpid;
pipe(fd);
if((childpid = fork()) == -1) {
        printf("Error! Cannot fork\n");
        exit(1);
}
if(childpid == 0) {
    //handle_matrixes(fd[0]);
    exit(0);
} else {
    char *buf = (char *)"Hello world!";
    write(fd[1], buf, sizeof(buf));
     Read in a string from the pipe *
//                nbytes = read(fd[0], readbuffer, sizeof(readbuffer));
//                printf("Received string: %s", readbuffer);
*/
