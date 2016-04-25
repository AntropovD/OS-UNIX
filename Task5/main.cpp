#include <stdio.h>
#include <stdlib.h>
#include <vector>

struct matrix {
    std::vector<int> data;
    int width;
    int height;

    struct matrix get_submatrix(int a, int b, int c, int d) {
        std::vector<int> data;
        if (c < a || d < b) {
            printf("Error! Wrong parameters");
            exit(2);
        }
        int size = this->height * this->width;
        int x, y;
        for (int i=0; i < size; i++) {
            x = i / width;
            y = i % width;
            if (a <= x && x <= c && b <= y && b <= d)
                data.push_back(this->data[i]);
        }
        struct matrix result;
        result.data = data;
        result.height = c - a;
        result.width = d - b;

        return result;
    }

    void print() {
        printf("%d %d\n", this->height, this->width);
        for (int i = 0; i < this->height; i++) {
            for (int j = 0; j < this->width; j++)
                printf("%d ", this->data[i * this->width + j]);
            printf("\n");
        }
    }
};

struct matrix matrix1;
struct matrix matrix2;

void get_input_data(char *filename);
void read_from_fd_to_matrix(FILE *fd, struct matrix *M);

int main(int argc, char **argv)
{
    if (argc != 4) {
        printf("Parallel matrix multiplication\n");
        printf("Usage: %s input result\n", argv[0]);
        //return 0;
    }

    char *input = (char*)"input";
    char *output = (char *)"result";

    get_input_data(input);


    struct matrix sub = matrix1.get_submatrix(0,0, 3, 3);
    sub.print();




    return 0;
}



void get_input_data(char *filename)
{
    FILE *fd = fopen(filename, "r");

    read_from_fd_to_matrix(fd, &matrix1);
    read_from_fd_to_matrix(fd, &matrix2);

    if (matrix1.width != matrix2.height) {
        printf("Error! Matrix sizes dont approach.\n");
        matrix1.data.clear();
        matrix2.data.clear();
        exit(1);
    }
    fclose(fd);
}

void read_from_fd_to_matrix(FILE *fd, matrix *M)
{
    int n, m, tmp;
    fscanf(fd, "%d %d", &n, &m);

    for (int i=0; i<n*m; i++) {
        fscanf(fd, "%d", &tmp);
        M->data.push_back(tmp);
    }
    M->height = n;
    M->width = m;
}
