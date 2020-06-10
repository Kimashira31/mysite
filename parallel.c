#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <time.h>
#define swap(type,x,y) do{type t=x; x=y; y=t;}while(0)

int SRC;
int MSRC;
int JUMP;
int **matrix1;
int **matrix2;
int **ansMatrix;
int ompSelect;
int *thread;
int threadData=1;

void command_line_read(const char *argv,int *RC) /*コマンドラインから値を読み込んでint型にする*/
{
	if((*RC=atoi(argv))<=0){
		fprintf(stderr,"parameter erorr...\n./kadai0526 int (int) (int)\n");
		fprintf(stderr,"start (goal) (jump)\n");
		exit(EXIT_FAILURE);
	}
}

void mode_select(int *random,int *test,int *clean) /*初期設定を選択*/
{
	do{
		printf("omp position select { none(0) / top(1) / middle(2) / bottom(3) } : ");
		scanf("%d",&ompSelect);
	}while(!(ompSelect>=0 && ompSelect<=3));
	do{
		printf("Print matrix? { Yes(0) / No(1) } : ");
		scanf("%d",test);
	}while(!(*test==0 || *test==1));

	if(*test==0){
		*clean=1;
		do{
			printf("random num? { No(0) / Yes(1) } : ");
			scanf("%d",random);
		}while(!(*random==0 || *random==1));
	}else{
		*random=1;
		do{
			printf("only num? { Yes(0) / No(1) } : ");
			scanf("%d",clean);
		}while(!(*clean==0 || *clean==1));
	}
}

void operate_thread(char th[]) /*スレッドについて*/
{
	int thn;
	printf("origin Max thread:%d\n",omp_get_max_threads());
	command_line_read(th,&thn);
	omp_set_num_threads(thn);
	printf("Max thread:%d\n",omp_get_max_threads());
	if(threadData==0)
		thread=calloc(thn,sizeof(int));
}

void start_set(int argc,char *argv[],int *random,int *test,int *clean) /*初期設定を行う*/
{
	switch (argc){
		case 2:
			command_line_read(*++argv,&SRC);
			MSRC=SRC;
			JUMP=1;
			break;
		case 3:
			command_line_read(*++argv,&SRC);
			command_line_read(*++argv,&MSRC);
			if(SRC>MSRC)
				swap(int,SRC,MSRC);
			JUMP=1;
			break;
		case 4:
			command_line_read(*++argv,&SRC);
			command_line_read(*++argv,&MSRC);
			if(SRC>MSRC)
				swap(int,SRC,MSRC);
			command_line_read(*++argv,&JUMP);
			if((MSRC==SRC) && JUMP==1);
			else if(JUMP<=0 || JUMP>(MSRC-SRC)){
				fprintf(stderr,"jump error...\n");
				exit(EXIT_FAILURE);
			}
			break;
		case 5:
			command_line_read(*++argv,&SRC);
			command_line_read(*++argv,&MSRC);
			if(SRC>MSRC)
				swap(int,SRC,MSRC);
			command_line_read(*++argv,&JUMP);
			if((MSRC==SRC) && JUMP==1);
			else if(JUMP<=0 || JUMP>(MSRC-SRC)){
				fprintf(stderr,"jump error...\n");
				exit(EXIT_FAILURE);
			}
			printf("thread use number data?(warning:speed down)\n");
			do{
				printf("Yes(0) / No(1) :");
				scanf("%d",&threadData);
			}while(!(threadData==0 || threadData==1));
			operate_thread(*++argv);
			break;
	}
	mode_select(random,test,clean);
	putchar('\n');
}

int **data_cell_pointer() /* int*型のメモリの確保 */
{
	int **d;
	if((d=calloc(SRC,sizeof(int *)))==NULL){
		printf("memory is not available...\n");
		exit(EXIT_FAILURE);
	}
	return d;
}

int *data_cell_int() /*int型のメモリの確保*/
{
	int *d;
	if((d=calloc(SRC,sizeof(int)))==NULL){
		printf("memory is not available...\n");
		exit(EXIT_FAILURE);
	}
	return d;
}

void data_matrix() /*行列のメモリ確保*/
{
	int i;
	matrix1=data_cell_pointer();
	matrix2=data_cell_pointer();
	ansMatrix=data_cell_pointer();

	for(i=0;i<SRC;i++)
		matrix1[i]=data_cell_int();
	for(i=0;i<SRC;i++)
		matrix2[i]=data_cell_int();
	for(i=0;i<SRC;i++)
		ansMatrix[i]=data_cell_int();
}

void num_input_matrix(int **matrix) /*行列に値を入力*/
{
	int i,j;
	for(i=0;i<SRC;i++){
		for(j=0;j<SRC;j++){
			printf("%dR%dC : ",i+1,j+1);
			scanf("%d",&matrix[i][j]);
		}
		putchar('\n');
	}
}

void random_input_matrix(int **matrix) /*行列にランダムな値を入力*/
{
	int i,j;
	for(i=0;i<SRC;i++){
		for(j=0;j<SRC;j++)
			matrix[i][j]=-99+rand()%199;
	}
}

void print_matrix(int **matrix) /*行列の表示*/
{
	int i,j;
	for(i=0;i<SRC;i++){
		for(j=0;j<SRC;j++)
			printf("%8d ",matrix[i][j]);
		putchar('\n');
	}
}

void mul_matrix(int **Matrix1,int **Matrix2,int **AnsMatrix,int argc) /*行列の積の計算*/
{
	double start,end;
	int i,j,k;
	switch (ompSelect){
		case 0:
			start=omp_get_wtime();
			for(i=0;i<SRC;i++){
				for(j=0;j<SRC;j++){
					for(k=0;k<SRC;k++){
						AnsMatrix[i][j]+=(Matrix1[i][k]*Matrix2[k][j]);
						if(threadData==0)thread[omp_get_thread_num()]++;
					}
				}
			}
			end=omp_get_wtime();
			break;
		case 1:
			start=omp_get_wtime();
			#pragma omp parallel for private(j,k) //which speed1
			for(i=0;i<SRC;i++){
				for(j=0;j<SRC;j++){
					for(k=0;k<SRC;k++){
						AnsMatrix[i][j]+=(Matrix1[i][k]*Matrix2[k][j]);
						if(threadData==0)thread[omp_get_thread_num()]++;
					}
				}
			}
			end=omp_get_wtime();
			break;
		case 2:
			start=omp_get_wtime();
			for(i=0;i<SRC;i++){
				#pragma omp parallel for private(k)//which speed2
				for(j=0;j<SRC;j++){
					for(k=0;k<SRC;k++){
						AnsMatrix[i][j]+=(Matrix1[i][k]*Matrix2[k][j]);
						if(threadData==0)thread[omp_get_thread_num()]++;
					}
				}
			}
			end=omp_get_wtime();
			break;
		case 3:
			start=omp_get_wtime();
			for(i=0;i<SRC;i++){
				for(j=0;j<SRC;j++){
					#pragma omp parallel for //which speed3
					for(k=0;k<SRC;k++){
						AnsMatrix[i][j]+=(Matrix1[i][k]*Matrix2[k][j]);
						if(threadData==0)thread[omp_get_thread_num()]++;
					}
				}
			}
			end=omp_get_wtime();
			break;
	}
	printf("%6.1f",(end-start)*1000);
}

void free_matrix() /*メモリの解放*/
{
	int i;
	for(i=0;i<SRC;i++)
		free(matrix1[i]);
	for(i=0;i<SRC;i++)
		free(matrix2[i]);
	for(i=0;i<SRC;i++)
		free(ansMatrix[i]);
	free(matrix1);
	free(matrix2);
	free(ansMatrix);
}

int main(int argc,char *argv[])
{
	if(!(argc==2 || argc==3 || argc==4 || argc==5)){
		fprintf(stderr,"** Manual!!! **\n\n");
		fprintf(stderr,"./kadai0526 int (int) (int)\n");
		fprintf(stderr,"start (goal) (jump)\n\n");
		fprintf(stderr,"~ omp position ~\n");
		fprintf(stderr,"\\*position (1)*\\\n""for(i=0;i<SRC;i++)\n"
					   "\t\\*position (2)*\\\n""\tfor(j=0;j<SRC;j++)\n"
					   "\t\t\\*position (3)*\\\n""\t\tfor(k=0;k<SRC;k++)\n"
					   "\t\t\tAnsMatrix[i][j]+=(Matrix1[i][k]*Matrix2[k][j]);\n");
		return -1;
	}
	int random,test,clean;
	srand(time(NULL));
	start_set(argc,argv,&random,&test,&clean);

	for(;SRC<=MSRC;SRC+=JUMP){
		data_matrix();
		if(random==0){
			printf("matrix 1 input↓\n");
			num_input_matrix(matrix1);
			printf("matrix 2 input↓\n");
			num_input_matrix(matrix2);
		}else{
			random_input_matrix(matrix1);
			random_input_matrix(matrix2);
		}

		if(test==0){
			printf("matrix 1↓\n");
			print_matrix(matrix1);
			printf("matrix 2↓\n");
			print_matrix(matrix2);

			putchar('\n');
			mul_matrix(matrix1,matrix2,ansMatrix,argc);
			printf(" millisec calculated\n\n");
			printf("%dRC matrix product↓\n",SRC);
			print_matrix(ansMatrix);
			printf("\n\n");
		}else{
			if(clean==1)printf("%4dRC  ",SRC);
			mul_matrix(matrix1,matrix2,ansMatrix,argc);
			(clean==1)?printf(" millisec calculated\n"):putchar('\n');
		}
		free_matrix();
	}
	printf("\n***  1000 millisec = 1 sec  ***\n\n");
	if(threadData==0){
		for(int i=0;i<omp_get_max_threads();i++)
			printf("thread[%d]:%d\n",i,thread[i]);
	}
	return 0;
}