#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>
#include<math.h>
#include<string.h>

int size, all;
char** instruction;
struct chain{
	char* name;
	int* arr;
};
struct chain** link;

void print(int innum, int outnum);

struct chain*  buildEle(struct chain* link, int size,char* name){
	link ->name = (char*)malloc(100*sizeof(char));
	strcpy(link ->name, name);
	link->arr = (int*)malloc(size * sizeof(int));
	int j;
	for(j = 0; j < size; j++){
		link->arr[j] = -1;
	}
	return link;
} 

struct component{
	char** input;
	int* input_index;
	int innum;
	char* name;
	int name_index;
	int instruct_index;
	char** selectors;
	struct component* next;	
};

struct component* head;
struct component* end;

void buildcomp(struct component* component, int innum, int index){
	component -> selectors = (char**)malloc(innum*sizeof(char*));
	component -> input = (char**)malloc(innum*sizeof(char*));
	component -> input_index = (int*)malloc(innum*sizeof(int));
	int i;
	for(i = 0; i<innum; i++){
		component -> input[i] = (char*)malloc(100*sizeof(char));
	}
	for(i = 0; i < innum; i++){
		component -> selectors[i] = (char*)malloc(100*sizeof(char));
	}
	component -> innum = innum;
	component -> name = (char*)malloc(100 *sizeof(char));
	component -> instruct_index = index;
	component -> next = NULL;
}
void freecomponent(struct component* component){
	free(component -> name);
	free(component -> input_index);
	int i;
	for(i = 0; i< component -> innum; i++){
		free(component -> input[i]);
		free(component -> selectors[i]);
	}
	free(component -> input);
	free(component -> selectors);
	free(component);
} 
void buildinput(struct chain** link, int size, int innum){
	int i, j, greycode;
	for(i=0; i<size;i++){
		greycode = (i >> 1) ^ i;
		for(j = 1; j<=innum; j++){
			link[j-1] -> arr[i] = (greycode & (1 << (innum-j))) >> (innum-j);
		}
	}
}

int And(int one, int two){
	return one & two;
}

int Or(int one, int two){
	return one | two;
}

int Xor(int one, int two){
	return one ^ two;
}

int Not(int one){
	if(one == 0) return 1;
	else return 0;
}

int nor(int one, int two){
	return Not(one | two);
}

int nand(int one, int two){
	return Not(one & two);
}

void decoder(struct component* a){
	int i = a -> innum;
	int j = (int)pow(2, i);
	int b, greycode, c;
	int** f = (int**)malloc(j*sizeof(int*));
	for(b = 0; b < j; b++){
		f[b] = (int*)malloc((i+j)*sizeof(int));
	}
	for(b = 0; b < j; b++){
		greycode = (b >> 1) ^ b;
		for(c = 1; c <= i; c++){
			f[b][c-1] = (greycode & (1 << (i-c))) >> (i-c);	
		}
	}
	for(b = 0; b < j; b++){
		for(c = i; c < (i+j); c++){
			f[b][c] = 0;
			if(b == (c - i)) f[b][c] = 1;
		}
	}
	int z, y;
	for(b = 0; b < j; b++){
		for(c = 0; c <size; c++){
			bool isequal = true;
				for(y =0; y < i; y++){
					if(link[a -> input_index[y]] -> arr[c] != f[b][y]) {isequal = false; break;}
				}
				if(isequal == true) {
					struct component* temp;
					temp = a;
					for(z = i; z < (i+j); z++){
						link[temp -> name_index] -> arr[c] = f[b][z];
						temp = temp -> next;
					}
				}
		}
	}
	for(z = 0; z < j; z++){
		free(f[z]);
	}
	free(f);
}

int getindex(char* name){
        int i;
        bool a = false;
        if((name[0]-'0') == 0 || (name[0]-'0') == 1 ){return 0;}
        for(i = 0; i < all; i++){
                if(strcmp(link[i] -> name, name) == 0){
                        a = true;
                        break;
                }
        }
	
        if(a == false){
                link[all] = buildEle(link[all], size, name);
                i = all;
                all ++;
        }
        return i;
}


void multiplexer(struct component* comp){
	int i = comp -> innum;
	int j = (int)log2(i);
	int** srr = (int** )malloc(i * sizeof(int*));
	int a, graycode,b, c;
	bool isequal;
	for(a = 0; a < i; a++){
		srr[a]=(int*)malloc(j*sizeof(int));
	}
	for(a = 0; a < i; a++){
                graycode = (a >> 1) ^ a;
                for(c = 1; c <= j; c++){
                        srr[a][c-1] = (graycode & (1 << (j-c))) >> (j-c);
                }
        }
	for(a = 0; a < size; a++){
		isequal = true;
		for(c = 0; c < i; c++){
			isequal = true;
			for(b = 0; b < j; b++){
				int r;
                                if(strcmp(comp -> selectors[b], "0")==0|| strcmp(comp -> selectors[b], "1")==0) r = comp -> selectors[b][0]-'0';
                                else r = link[getindex(comp -> selectors[b])] -> arr[a];
                                if(r != srr[c][b]) { isequal = false; break;}
			}
			if(isequal == true){
				break;
			}
		}
		if(strcmp(comp -> input[c],"1") == 0 || strcmp(comp -> input[c],"0") == 0){
			link[comp -> name_index] -> arr[a] = comp -> input[c][0] - '0';	
		} else {
			link[comp -> name_index] -> arr[a] = link[comp -> input_index[c]] -> arr[a];
		}
	
	
	}
	for(a = 0; a < i; a++){	
		free(srr[a]);
	}
	free(srr);
}


void setinputindex(){
	struct component* ptr = head;
	int i;
	while(ptr -> next != head){
		for(i = 0; i < ptr-> innum; i++){
			ptr -> input_index[i] = getindex(ptr->input[i]);
		}
		ptr = ptr -> next;
	}
	for(i = 0; i < ptr-> innum; i++){
                ptr -> input_index[i] = getindex(ptr->input[i]);
        }
}

bool step(FILE* fp, int innum, int outnum, int all){
	bool isfinish = false;
	struct component* n = (struct component* )malloc(sizeof(struct component));
	char* a = (char*)malloc(12*sizeof(char));
	char* st = (char*)malloc(50*sizeof(char));
	if(fscanf(fp,"%s",a)!=1){
		free(a);
		free(st);
		free(n);
		isfinish = true;
		return isfinish;
	}
	int b = 0;
	int i, j;
	for(i = 0; i < 8; i++){
		if(strcmp(instruction[i],a) == 0){
			break;
		}
	}
	if(strcmp(a,"NOT") == 0){
		b = 1;
	} else if(strcmp(a,"AND")==0 || strcmp(a, "OR") == 0 || strcmp(a, "NAND")==0 || strcmp(a, "NOR") == 0 || strcmp(a, "XOR") == 0){
		b = 2;
	} else {
		fscanf(fp, "%d", &b);
	}
	struct component* ptr;
	if(head == NULL){
		buildcomp(n, b, i);
		head = n;
		head -> next = head;
		end = head;
	} else {
		ptr = head;
		while(ptr -> next != head){
			ptr = ptr -> next;
		}
		buildcomp(n, b, i);
		ptr -> next = n;
		n -> next = head;
		end = n;
	}
	if(strcmp(a,"MULTIPLEXER") == 0){
		for(j = 0; j < b; j++){
			fscanf(fp,"%s", st);
			strcpy(n -> input[j],st);
		}
		int t = (int)log2(b);
		for(j = 0; j < t; j++){
			fscanf(fp, "%s", st);
			strcpy(n -> selectors[j],st);
		}
		fscanf(fp, "%s", st);
		strcpy(n -> name,st);
		n -> name_index = getindex(n -> name);
	} else if (strcmp(a, "DECODER") == 0){
		for(j = 0; j < b; j++){
			fscanf(fp, "%s", st);
			strcpy(n -> input[j],st);
		}
		int s = pow(2, b);
		fscanf(fp, "%s", st);
		strcpy(n -> name,st);
		n -> name_index = getindex(n -> name);
		for(j = 1; j < s; j++){
			struct component* c = (struct component*)malloc(sizeof(struct component));;
			buildcomp(c , b, i);
			ptr = n;
			n -> next = c;
			c -> next = head;
			end = c;
			ptr = ptr -> next;
			n = n -> next;
			n -> input = ptr -> input;
			fscanf(fp, "%s", st);
			strcpy(n -> name,st);
			n -> name_index = getindex(n -> name);
		}
	} else if(strcmp(a,"NOT") == 0){
		fscanf(fp, "%s", st);
		strcpy(n -> input[0],st);
		fscanf(fp, "%s", st);
		strcpy(n -> name,st);
		n -> name_index = getindex(n -> name);
	} else {
		fscanf(fp, "%s", st);
		strcpy(n -> input[0],st);
		fscanf(fp, "%s", st);
		strcpy(n -> input[1],st);
		fscanf(fp, "%s", st);
		strcpy(n -> name,st);
		n -> name_index = getindex(n -> name);
	}
	free(a);
	free(st);
	return isfinish;
}


void operate(int innum, int linenum){
	setinputindex();
	struct component* ptr = head;
	struct component* prv = end;
	int i;
	int j = 0;
	bool k = true;
	while(j < linenum){
		for(i=0; i < ptr -> innum; i++){
			if(link[ptr -> input_index[i]] -> arr[0] == -1){
				k = false;
				break;
			}		
		}
		if(ptr -> instruct_index == 7 && k == true){
			int check = sqrt(ptr -> innum);
			for(i = 0; i < check; i++){
				if(link[getindex(ptr -> selectors[i])]->arr[0] == -1){
					k = false;
					break;
				}
			}
		}
		if(k == false){
			prv = ptr;
			ptr = ptr -> next;
		} else {
			j++;
			struct component* temp;
			if(ptr -> instruct_index == 0){
				for(i = 0; i < size; i++){
					int a, b;
					if(strcmp(ptr -> input[0], "0") == 0 || strcmp(ptr -> input[0], "1") == 0){
						a = ptr -> input[0][0] - '0';
					} else a = link[ptr -> input_index[0]] -> arr[i];
					if(strcmp(ptr -> input[1], "0") == 0 || strcmp(ptr -> input[1], "1") == 0){
                                                b = ptr -> input[1][0] - '0';
                                        } else b = link[ptr -> input_index[1]] -> arr[i];
					link[ptr -> name_index]->arr[i] = And(a,b);
				}
				temp = ptr;
				ptr = ptr -> next;
				prv ->next = ptr;
				freecomponent(temp);
			} else if(ptr -> instruct_index == 1){
				for(i = 0; i < size; i++){
					int a;
                                        if(strcmp(ptr -> input[0], "0") == 0 || strcmp(ptr -> input[0], "1") == 0){
                                                a = ptr -> input[0][0] - '0';
                                        } else a = link[ptr -> input_index[0]] -> arr[i];
					link[ptr -> name_index] -> arr[i] = Not(a);
				}
                                temp = ptr;
                                ptr = ptr -> next;
				prv -> next = ptr;
                                freecomponent(temp);
			} else if(ptr -> instruct_index == 2){
				for(i = 0; i < size; i++){
					int a, b;
                                        if(strcmp(ptr -> input[0], "0") == 0 || strcmp(ptr -> input[0], "1") == 0){
                                                a = ptr -> input[0][0] - '0';
                                        } else a = link[ptr -> input_index[0]] -> arr[i];
                                        if(strcmp(ptr -> input[1], "0") == 0 || strcmp(ptr -> input[1], "1") == 0){
                                                b = ptr -> input[1][0] - '0';
                                        } else b = link[ptr -> input_index[1]] -> arr[i];

                                        link[ptr -> name_index]->arr[i] = Or(a,b);
                                }
                                temp = ptr;
                                ptr = ptr -> next;
				prv -> next = ptr;
                                freecomponent(temp);
			} else if(ptr -> instruct_index == 3){
				for(i = 0; i < size; i++){
					int a, b;
                                        if(strcmp(ptr -> input[0], "0") == 0 || strcmp(ptr -> input[0], "1") == 0){
                                                a = ptr -> input[0][0] - '0';
                                        } else a = link[ptr -> input_index[0]] -> arr[i];
                                        if(strcmp(ptr -> input[1], "0") == 0 || strcmp(ptr -> input[1], "1") == 0){
                                                b = ptr -> input[1][0] - '0';
                                        } else b = link[ptr -> input_index[1]] -> arr[i];

                                        link[ptr -> name_index]->arr[i] = nand(a,b);
                                }
                                temp = ptr;
                                ptr = ptr -> next;
				prv -> next = ptr;
                                freecomponent(temp);
			} else if(ptr -> instruct_index == 4){
				for(i = 0; i < size; i++){
					int a, b;
                                        if(strcmp(ptr -> input[0], "0") == 0 || strcmp(ptr -> input[0], "1") == 0){
                                                a = ptr -> input[0][0] - '0';
                                        } else a = link[ptr -> input_index[0]] -> arr[i];
                                        if(strcmp(ptr -> input[1], "0") == 0 || strcmp(ptr -> input[1], "1") == 0){
                                                b = ptr -> input[1][0] - '0';
                                        } else b = link[ptr -> input_index[1]] -> arr[i];

                                        link[ptr -> name_index]->arr[i] = nor(a,b);
                                }
                                temp = ptr;
                                ptr = ptr -> next;
				prv -> next = ptr;
                                freecomponent(temp);
			} else if(ptr -> instruct_index == 5){
				for(i = 0; i < size; i++){
					int a, b;
                                        if(strcmp(ptr -> input[0], "0") == 0 || strcmp(ptr -> input[0], "1") == 0){
                                                a = ptr -> input[0][0] - '0';
                                        } else a = link[ptr -> input_index[0]] -> arr[i];
                                        if(strcmp(ptr -> input[1], "0") == 0 || strcmp(ptr -> input[1], "1") == 0){
                                                b = ptr -> input[1][0] - '0';
                                        } else b = link[ptr -> input_index[1]] -> arr[i];

                                        link[ptr -> name_index]->arr[i] = Xor(a,b);
                                }
                                temp = ptr;
                                ptr = ptr -> next;
				prv -> next = ptr;
                                freecomponent(temp);
			} else if(ptr -> instruct_index == 6){
				decoder(ptr);
				int i;
				int p = (int)pow(2, ptr -> innum);
				for(i = 0; i < p; i++){
					struct component* temp = ptr;
					ptr = ptr -> next;
					prv -> next = ptr;
					freecomponent(temp);
				}
			} else {
				multiplexer(ptr);
                                temp = ptr;
                                ptr = ptr -> next;
				prv -> next = ptr;
                                freecomponent(temp);
			}
		}
		k = true;
	}
}

void print(int innum, int outnum){
	int a, b;
	for(a = 0; a < size; a++){
		for(b = 0; b < (innum+outnum - 1); b++){
			printf("%d ", link[b] -> arr[a]);
		}
		printf("%d\n", link[b]-> arr[a]);
	}
}

int main(int argc, char** argv){
	char* filename = NULL;
	if(argc<2){
		printf("error\n");
		exit(0);
	}
	filename = argv[1];
	FILE* fp = fopen(filename, "r");	
	if(fp == NULL){
		printf("error\n");
		exit(0);
	}
	instruction = (char **)malloc(8 * sizeof(char*));
	int i;
	for(i = 0; i < 8; i++){
		instruction[i] = (char *)malloc(12 * sizeof(char));
	}
	strcpy(instruction[0],"AND");
	strcpy(instruction[1],"NOT");
	strcpy(instruction[2],"OR");
	strcpy(instruction[3],"NAND");
	strcpy(instruction[4],"NOR");
	strcpy(instruction[5],"XOR");
	strcpy(instruction[6],"DECODER");
	strcpy(instruction[7],"MULTIPLEXER");
	int innum = 0; 
	int outnum = 0;
	char* str = (char*)malloc(500*sizeof(char));
	link = (struct chain**)malloc(300*sizeof(struct chain*));
	for(i = 0; i < 300; i++){
		link[i] = (struct chain*)malloc(sizeof(struct chain));
	}
	fscanf(fp, "%s", str);
	fscanf(fp,"%d", &innum);
	size = pow(2,innum);
	for(i = 0; i < innum; i++){
		fscanf(fp,"%s", str);
		link[i] = buildEle(link[i], size, str);
	}
	fscanf(fp, "%s", str);
	fscanf(fp,"%d", &outnum);
	for(i = 0; i<outnum; i++){
		fscanf(fp,"%s",str);
		link[innum+i] = buildEle(link[innum+i],size, str);
	}
	all = innum + outnum;
	buildinput(link, size, innum);
	bool isfinish = false;
	int linenum = 0;
	while(isfinish == false){
		linenum++;
		isfinish = step(fp, innum, outnum, all);
	}
	linenum--;
	fclose(fp);
	operate(innum,linenum);
	print( innum, outnum);
	free(str);
	for(i = 0; i<300; i++){
		free(link[i] -> name);
		free(link[i] -> arr);
		free(link[i]);
	}
	free(link);
	for(i = 0; i<8; i++){
		free(instruction[i]);
	}
	free(instruction);
	return 0;
}

