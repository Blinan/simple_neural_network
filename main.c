#include<stdio.h>
#include<time.h>
#include<stdlib.h>
#include <string.h>
#include <math.h>
#include <limits.h>
#include "log.h"
//--------------------------------
#define MATRIX_TYPE double
#define _DETAILED_ 0
/* 档位: 0/1/2/3 四等级: 0->3 逐渐详细 (default-2)
 * level - 显示详情的函数
 * 0 - M_print （除设定的输出外，不额外显示其他计算细节信息）
 * 1 - M_Uptri_/ M_Lowtri_/ M_Diatri_
 * 2 - M_full/ M_Inverse/ M_eigen_val/ M_rank / M_Uptri_/ M_Lowtri_/ M_Diatri_/ M_print
 * 3 - M_free/ M_mul/ M_full/ M_Inverse/ M_rank/ M_mul / M_Uptri_/ M_Lowtri_/ M_Diatri_/ M_print
 * */
/* 进度条显示 */
/*ERROR TABLE - 错误提示*/
#define M_mul_001        "@ERROR: Matrix_Dimensions Wrong!\n\tDetails:(M_mul_001)_mat_left->column != _mat_right->row\n"
#define M_add_sub_003    "@ERROR: Matrix_Dimensions Wrong!\n\tDetails:(M_add_sub_003)_mat_subed != _mat_minus\n"
#define PRECISION "%.3lf\t"


//--------------------------------




//define useful function
//--------------------------------
void swapInt(int *x,int *y){
    int temp = *x;
    *x = *y;
    *y = temp;
}
void swapDouble(double *x,double *y){
    double temp = *x;
    *x = *y;
    *y = temp;
}

void *cat(void *l, void *r, char type){
    switch (type)
    {
    case 'c':{
    
        char *lc = (char *)l;
        char *rc = (char *)r;
        int length_lc = strlen(lc);
        int length_rc = strlen(rc);
        char *result = (char *)malloc(sizeof(char)*length_lc+length_rc);
        for(int i = 0; i < length_lc; ++i){
            result[i] = lc[i];
        }
        for(int i = 0; i < length_rc; ++i){
            result[i+length_lc] = rc[i]; 
        }
        result[length_lc+length_rc]='\0';
        return result;
    }
    default:{
        fprintf(stderr,"[WARNING] THIS PART ONLY SUPPORT CHAR NOW");
        return NULL;
        break;
    }
    }   
    
}
void *ncat(void *l, void *r, int cat_num,char type){
    if(strlen((char *)r)==0)return l;
        switch (type)
    {
    case 'c':{
    
        char *lc = (char *)l;
        char *rc = (char *)r;
        int length_lc = strlen(lc);
        int length_rc = cat_num;    //major diff with function of cat
        char *result = (char *)malloc(sizeof(char)*(length_lc+length_rc));
        for(int i = 0; i < length_lc; ++i){
            result[i] = lc[i];
        }
        for(int i = 0; i < length_rc; ++i){
            result[i+length_lc] = rc[i]; 
        }
        result[length_lc+length_rc]='\0';
        return result;
    }
    default:{
        fprintf(stderr,"[WARNING] THIS PART ONLY SUPPORT CHAR NOW");
        return NULL;
        break;
    }
    }   
}

char *str_split(const char *str,const char *sep){
    //Return a list of the substrings in the string, using sep as the separator string.
    char *substrings = "";
    int compare_len = strlen(sep);
    int last_match_position = 0,i;
    for(i = 0; i < strlen(str); ++i){
        //log_debug("i=%d, str+i=%c,sep=%c  strcmp=%d",i,*(str+i), *sep, strncmp(str+i,sep,compare_len));
        if(strncmp(str+i,sep,compare_len) == 0){
            substrings = ncat(substrings, str + last_match_position, i - last_match_position, 'c');
            printf("substrings=%s\n",substrings);
            last_match_position = i + compare_len;
            i+=compare_len-1;
        }
    }
    //if(last_match_position!=strlen(str))
    substrings = ncat(substrings, str + last_match_position, i - last_match_position, 'c');
    printf("substrings=%s\n",substrings);
    return substrings;
}



//END define useful function
//--------------------------------

//define CSV function
//--------------------------------
#define vec_type_t double

typedef struct _vector
{
    vec_type_t size;
    vec_type_t capacity;
    vec_type_t *data;
}vector;

vector *vector_create(void)
{
	vector* h = (vector*)malloc(sizeof(vector));
	h->capacity = 0;
	h->size = 0;

	return h;
}
vector* vector_realloc(vector* h, vec_type_t type_size)
{
	vec_type_t new_capacity = (h->capacity == 0) ? 1 : h->capacity * 2;
	vector* new_h = (vector*)realloc(h, sizeof(vector) + new_capacity * type_size);
	new_h->capacity = new_capacity;

	return new_h;
}
vector *vector_alloc_capacity(vector *vec,int cap, vec_type_t type_size){
	vec->data = (vec_type_t*)malloc(cap * type_size);
	vec->capacity = cap;

	return vec;
}

void vector2D_print(vector**vector2D, int row, int maxcolum){
    for(int i = 0; i < 10; ++i){
        for(int j = 0; j < vector2D[i]->size; ++j){
            //log_info("vector2D[%d]->data[%d] = %d",i , j,vector2D[i]->data[j]);
        }
        
        log_info("vector2D[%d]->data[%d] = %f",i , 0,vector2D[i]->data[0]);
    }
}



vector **readlines(FILE *fileobj,vector ** vector2D,int maxrow, int maxcolumn){
    int buffer_size = 20, i, c;
    char *buffer = (char *)malloc(sizeof(char)*buffer_size);
    for(i = 0; i < buffer_size; ++i)
                buffer[i]='\0';
    //printf("buffer[0]= %d",buffer[0]);
    for(i = 0; i < maxrow; ++i){
        vector2D[i] = vector_create();
        vector * temp = vector2D[i]; 
        vector2D[i] = vector_alloc_capacity(vector2D[i], maxcolumn, sizeof(vec_type_t));
        temp = vector2D[i]; 
        int i = 0;
    }
    int nu_line = 0;
    int nu_digit=0;    //digit position in a line
    c = fgetc(fileobj);
    while(c != EOF){
        
        for(i=0; c != ',' && c!= '\n'; ++i){
            buffer[i]=c;
            c = fgetc(fileobj);
        }
        
        if(c == ',' || c == '\n'){
            vector2D[nu_line]->data[nu_digit] = atof(buffer);
            //万恶之源，忘记初始化buffer
            for(i = 0; i < buffer_size; ++i)
                buffer[i]='\0';

            // if(nu_line < 5&&nu_digit < 5)
            // log_info("vector2D[%d]->data[%d]=%d",nu_line,nu_digit,vector2D[nu_line]->data[nu_digit]);
            // if(nu_line < 5&&nu_digit > 780)
            // log_info("vector2D[%d]->data[%d]=%d",nu_line,nu_digit,vector2D[nu_line]->data[nu_digit]);
            
            ++nu_digit;
        }
        if(c == '\n'){
            vector2D[nu_line]->size = nu_digit;
            ++nu_line;
            //log_info("nu_digit = %d", nu_digit);
            nu_digit = 0;
            //万恶之源，忘记初始化buffer
            for(i = 0; i < buffer_size; ++i)
                buffer[i]='\0';
        }
        c = fgetc(fileobj);

    }
    //log_trace("finish readlines");
    return vector2D;
} 
//END define CSV function
//--------------------------------

//define data struction of matrix
//--------------------------------

typedef struct _Matrix{
    int row;
    int column;
    MATRIX_TYPE * data;
} Matrix;




static int num=0;
Matrix *Matrix_gen(int row, int column, MATRIX_TYPE *data) {/*
 * Generate a new Matrix(struct).
 * 导入_生成矩阵*/
    Matrix *_mat = (Matrix *) malloc(sizeof(Matrix));
    _mat->row = row;
    _mat->column = column;
    int size = _mat->row * _mat->column;
    _mat->data = (MATRIX_TYPE *) malloc((size) * sizeof(MATRIX_TYPE));
    log_info("%d",++num);
    memcpy(_mat->data, data, (size)*sizeof(MATRIX_TYPE));
    /*
    int i
    for(i = 0; i < size; ++i)_mat->data[i] = data[i];
    */
   return _mat;
}

Matrix *M_copy(Matrix *_mat_source){
    Matrix *mat = Matrix_gen(_mat_source->row, _mat_source->column, _mat_source->data);
    return mat;
}

Matrix *M_T(Matrix *_mat_source) {/*
 * Transpose (create).
 * 转置 */

    Matrix *mat = M_copy(_mat_source);
    int i, j;
    for(i = 0; i < mat->row-1; ++i){
        for(j = i+1; j < mat->column; ++j){
            swapDouble(mat->data + mat->column * i + j,mat->data + mat->column * i + j);
        }
    }
    swapInt(&mat->row, &mat->column);
    return mat;
}

int M_free(Matrix *_mat) {/*
 * Free the memory of the Matrix (create).
 * 释放矩阵，释放内存*/
    if(_mat == NULL){
        fprintf(stderr,"[WARNING] NO SUCH MATRIX");
        return 0;
    }
    free(_mat->data);
    free(_mat);
}

Matrix *M_numul(Matrix *_mat, MATRIX_TYPE _num) {/*
 * Number Multiplication (create).
 * 矩阵数乘*/
    Matrix *mat = M_copy(_mat);
    int size_mat = (mat->row) * (mat->column),i;
    for(i = 0; i < size_mat; ++i) (mat->data)[i] *= _num; 
    return mat;
}

Matrix *M_Zeros(int row, int column) {/*
 * Generation All-Zeros-Matrix (create).
 * 生成全零矩阵*/
    int size_mat = row * column, i;
    MATRIX_TYPE * data = (MATRIX_TYPE * )malloc(size_mat * sizeof(MATRIX_TYPE));
    for(i = 0; i < size_mat; ++i)data[i]=0;
    //memset(data, 0, size_mat * sizeof(MATRIX_TYPE));
    Matrix * zero_mat = Matrix_gen(row, column, data);
    return zero_mat; 
}

Matrix *M_Ones(int row, int column) {/*
 * Generation All-Ones-Matrix (create).
 * 生成全一矩阵*/
    int size_mat = row * column, i;
    MATRIX_TYPE * data = (MATRIX_TYPE * )malloc(size_mat * sizeof(MATRIX_TYPE));
    for(i = 0; i < size_mat; ++i)data[i]=1;
    //memset(data, 1, size_mat * sizeof(MATRIX_TYPE));
    Matrix * zero_mat = Matrix_gen(row, column, data);
    return zero_mat; 
}

Matrix *M_mul(Matrix *_mat_left, Matrix *_mat_right) {/*
 * Matrix multiplication (create new one, abbr. create).
 * _mat_result = _mat_left*_mat_right
 * 矩阵乘法 dot mul*/ 
    (_DETAILED_>=3)?printf(">>Matrix_%x * Matrix_%x =\n", _mat_left, _mat_right):0;
    /*Determine_Matrix_Dimensions*/
    Matrix *_mat_result = NULL;
    if (_mat_left->column != _mat_right->row) {
        printf(M_mul_001);
    } else {
        _mat_result = (Matrix *) malloc(sizeof(Matrix));
        int row = _mat_left->row;
        int mid = _mat_left->column;
        int column = _mat_right->column;
        int i, j, k;
        MATRIX_TYPE *_data = (MATRIX_TYPE *) malloc((row * column) * sizeof(MATRIX_TYPE));
        MATRIX_TYPE temp = 0;
        /*Ergodic*/
        for (i = 0; i < row; i++) {
            for (j = 0; j < column; j++) {
                /*Caculate Element*/
                temp = 0;
                for (k = 0; k < mid; k++) {
                    temp += (_mat_left->data[i * mid + k]) * (_mat_right->data[k * column + j]);
                }
                _data[i * column + j] = temp;
            }
        }
        _mat_result->row = row;
        _mat_result->column = column;
        _mat_result->data = _data;
    }
    (_DETAILED_>=3)?printf("\tMatrix_%x\n", _mat_result):0;
    return _mat_result;
}

Matrix *M_easy_mul(Matrix *_mat_left, Matrix *_mat_right){
    //M_easy_mul({1,2,3,4,5} ,{1,2,3,4,5}) = {1,4,9,16,25}
    int size = _mat_left->row*_mat_left->column, i;
    Matrix *mat = M_Zeros(_mat_left->row, _mat_left->column);
    for(int i = 0; i < size; ++i){
        mat->data[i] = _mat_left->data[i] * _mat_right->data[i];
    }
    return mat;
}

Matrix *M_rand(Matrix *_mat){

    Matrix *mat = M_Zeros(_mat->row, _mat->column);
    struct tm time;
    srand(time.tm_hour*60*60+time.tm_min*60+time.tm_sec);
    int size_mat = mat->row * mat->column, i;
    for(i = 0; i <size_mat; ++i)mat->data[i] = ((double)(rand()))/RAND_MAX - 0.5;
    return mat;
}

double sigmoid(double x){
    return 1/(1+exp(-x));
}
Matrix *M_sigmoid(Matrix* _mat){
    Matrix *mat = M_copy(_mat);
    int size_mat = mat->row * mat->column, i;
    for(i = 0; i < size_mat; ++i)mat->data[i] = sigmoid(mat->data[i]);
    return mat;
}

Matrix *M_add_sub(MATRIX_TYPE scale_mat_subed, Matrix *_mat_subed, MATRIX_TYPE scale_mat_minus, Matrix *_mat_minus) {/*
 * Addition/ Subtraction between Matrix-s (create).
 * 矩阵加减法 */
    Matrix *_mat_result = NULL;
    if ((_mat_subed->column == _mat_minus->column) && (_mat_subed->row == _mat_minus->row)) {
        _mat_result = M_Zeros(_mat_subed->row, _mat_subed->column);
        int size = (_mat_subed->row) * (_mat_subed->column), i;
        for (i = 0; i < size; i++) {
            _mat_result->data[i] = (_mat_result->data[i]) * scale_mat_subed - (_mat_minus->data[i]) * scale_mat_minus;
        }
    } else {
        printf(M_add_sub_003);
    }
    return _mat_result;
}

int M_max_index(Matrix *_mat){
    //return max value index in matrix'data
    double max = INT_MIN;
    int size = _mat->row*_mat->column, i;
    int max_index=0;
    for(i = 0; i < size; ++i){
        if(max < _mat->data[i]){
            max = _mat->data[i];
            max_index = i;
        }
    }
    return max_index;
}

int M_print(Matrix *_mat) {/*
 * Matrix Print, Display.
 * 打印矩阵 */
    printf(">>Matrix_%x:\n", _mat);
    int i, j;
    for (i = 0; i < _mat->row; i++) {
        for (j = 0; j < _mat->column; j++) {
            printf(PRECISION, _mat->data[i * (_mat->column) + j]);
        }
        printf("\n");
    }
    return 0;
}
//END define data struction of matrix 
//--------------------------------

//define data structure neural network
//--------------------------------

typedef struct _neuralNetwork{
    int input_nodes;
    int hidden_nodes;
    int output_nodes;
    double learningrate;
    Matrix *wih;
    Matrix *who;
} neuralNetwork;

neuralNetwork *Net_init(neuralNetwork *self,int input_nodes, int hidden_nodes, int output_nodes, double learningrate){
    self->input_nodes = input_nodes;
    self->hidden_nodes = hidden_nodes;
    self->output_nodes = output_nodes;
    self->learningrate = learningrate;
    self->wih = M_Zeros(hidden_nodes, input_nodes);
    self->wih = M_rand(self->wih);
    self->who = M_Zeros(output_nodes, hidden_nodes);
    self->who = M_rand(self->who);

    log_trace("Net_init DONE");
    return self;
}

neuralNetwork *Net_train(neuralNetwork *self,MATRIX_TYPE *inputs_list,int len_inputs_list,MATRIX_TYPE *targets_list, int len_targets_list){
    Matrix *mat_inputs = Matrix_gen(len_inputs_list, 1, inputs_list);
    Matrix *mat_targets = Matrix_gen(len_targets_list, 1, targets_list);
    Matrix *hiddent_inputs = M_mul(self->wih, mat_inputs);
    Matrix *hiddent_outputs = M_sigmoid(hiddent_inputs);
    Matrix *final_inputs = M_mul(self->who, hiddent_outputs);
    Matrix *final_outputs = M_sigmoid(final_inputs);
    //2026  06  18--------------------------------------------------------------------------------------------------------------------------------------------
    Matrix *output_errors = M_add_sub(1, mat_targets, 1, final_outputs);
    Matrix *hiddent_errors = M_mul(M_T(self->who), output_errors);

    self->wih = M_rand(self->wih);
    self->who = M_rand(self->who);


    //self.who += self.lr * numpy.dot((output_errors * final_outputs*(1.0 - final_outputs) ), numpy.transpose(hiddent_outputs))
    
    Matrix *mat_ones = M_Ones(final_outputs->row, final_outputs->column);
    Matrix * who_ones_sub_final_outputs = M_add_sub(1, mat_ones, 1, final_outputs);   //(1.0 - final_outputs)
    Matrix *who__right =M_easy_mul(   M_easy_mul(output_errors, final_outputs)   ,   who_ones_sub_final_outputs);//(output_errors * final_outputs*(1.0 - final_outputs) )
    Matrix *who__result = M_numul(M_mul( who__right , M_T(hiddent_outputs)), self->learningrate); //self.lr * numpy.dot((output_errors * final_outputs*(1.0 - final_outputs) ), numpy.transpose(hiddent_outputs))
    self->who = M_add_sub(1, self->who, -1, who__result);
    

    
    //self.wih += self.lr * numpy.dot((hiddent_errors * hidden_outputs*(1.0 - hiddent_outputs) ), numpy.transpose(inputs))
    Matrix * wih_ones_sub_final_outputs = M_add_sub(1, M_Ones(hiddent_outputs->row, hiddent_outputs->column), 1, hiddent_outputs);   //(1.0 - hiddent_outputs)
    Matrix *wih__right =M_easy_mul(   M_easy_mul(hiddent_errors, hiddent_outputs)   ,   wih_ones_sub_final_outputs);//(hiddent_errors * hiddent_outputs*(1.0 - hiddent_outputs) )
    Matrix *wih__result = M_numul(M_mul( wih__right , M_T(mat_inputs)), self->learningrate);//self.lr * numpy.dot((hiddent_errors * hiddent_outputs*(1.0 - hiddent_outputs) ), numpy.transpose(inputs))
    self->wih = M_add_sub(1, self->wih, -1, wih__result);
    
    
    return self;
}

Matrix *query(neuralNetwork *self, MATRIX_TYPE *inputs_list, int len_inputs_list){
    Matrix *mat_inputs = Matrix_gen(len_inputs_list, 1, inputs_list);
    Matrix *hiddent_inputs = M_mul(self->wih, mat_inputs);
    Matrix *hiddent_outputs = M_sigmoid(hiddent_inputs);
    Matrix *final_inputs = M_mul(self->who, hiddent_outputs);
    Matrix *final_outputs = M_sigmoid(final_inputs);
    return final_outputs;
}

int is_get_score(neuralNetwork *self, MATRIX_TYPE *inputs_list,int len_inputs_list,MATRIX_TYPE *targets_list, int len_targets_list){
    Matrix *final_output=query(self, inputs_list, len_inputs_list);
    int mat_size=final_output->row*final_output->column, i;
    for(i = 0; i < mat_size; ++i){
        if(M_max_index(final_output) == M_max_index(targets_list))
        return true;
    }
    return false;
}
//END define data structure neural network
//--------------------------------

//define CSV file read
//--------------------------------

    
    

//END define CSV file read
//--------------------------------
int main(){
    log_set_quiet(false);
    MATRIX_TYPE arr1[3][5]={
        1,2,3,4,5,
        6,7,8,9,10,
        11,12,13,14,15
    };
    MATRIX_TYPE arr2[5][3]={
        1,2,3,
        4,5,6,
        7,8,9,
        10,11,12,
        13,14,15
    };
    Matrix *mat1 = Matrix_gen(3,5,arr1);
    Matrix *mat2 = Matrix_gen(5,3,arr2);
    Matrix *mat3 = M_mul(mat1, mat2);
    mat3 = M_rand(mat3);
    //M_print(mat3);
    //M_print(M_T(mat1));

    const char *training_file_path = "D:\\Project\\dataset\\mnist_train.csv";
    const char *testing_file_path = "D:\\Project\\dataset\\mnist_test.csv";
    const char *test_path = "D:\\Project\\github_repository\\simple_neural_network\\test_data.csv";
    FILE *training_file_obj = fopen(training_file_path, "r");
    FILE *testing_file_obj = fopen(testing_file_path, "r");
    FILE *test_file_obj = fopen(test_path, "r");
    int num;    
//line

    
    char *line;
    int lines_num = 60000;
    
   
//END line

//prepare data
//--------------------------------
    int training_nu_rows=60000;
    int training_nu_column=785;
    vector** training_data = (vector **)malloc(sizeof(vector *)*training_nu_rows); 
    training_data = readlines(training_file_obj, training_data,training_nu_rows, training_nu_column);
    vector2D_print(training_data, training_nu_rows, training_nu_column);

    // Matrix *training_datas[training_nu_rows];

    // for(int i  = 0; i < training_nu_rows; ++i){
    //     training_datas[i] = Matrix_gen(784, 1 ,lines[i]->data+1);
    // }
    int testing_nu_rows=10000;
    int testing_nu_column=785;
    vector** testing_data = (vector **)malloc(sizeof(vector *)*testing_nu_rows); 
    testing_data = readlines(testing_file_obj, testing_data,testing_nu_rows, testing_nu_column);
    vector2D_print(testing_data, testing_nu_rows, testing_nu_column);
//END prepare data
////--------------------------------

//start traning
//--------------------------------
neuralNetwork *Net = (neuralNetwork *)malloc(sizeof(neuralNetwork));
int input_nodes=784;
int hidden_nodes=100;
int output_nodes=10;
double learningrate=0.1;
Net =  Net_init(Net, input_nodes, hidden_nodes, output_nodes, learningrate);
for(int i = 0; i < training_nu_rows; ++i){
    
    double *training_input_lists = training_data[i]->data+1;
    Matrix *training_target_lists = M_Zeros(output_nodes, 1);
    training_target_lists->data[(int)(training_data[i]->data[0])]+=1;
    Net = Net_train(Net, training_input_lists, input_nodes, training_target_lists->data, output_nodes);
}
int score=0;
for(int i = 0; i < testing_nu_column; ++i){
    double *testing_input_lists = testing_data[i]->data+1;
    Matrix *testing_target_lists = M_Zeros(output_nodes, 1);
    testing_target_lists->data[(int)(training_data[i]->data[0])]+=1;
    score +=is_get_score(Net, testing_input_lists, input_nodes, testing_target_lists, output_nodes);
}
double scorePercent = score/testing_nu_rows;
printf("scorePercent = %f\n",scorePercent);

//END start traning
//--------------------------------

    fclose(training_file_obj);
    fclose(testing_file_obj);
    fclose(test_file_obj);
//test part
//--------------------------------
    // char * c=str_split(",,,,,,,,,,,,45454,,,,,,,,,,5656,asd,as,as,as,,as",",");
    // printf("\nc = %s\n",c);
    // printf("%d", strncmp("aabb","aababc",4));

//END part
////--------------------------------
    return 0;
}