/* Name:  Myron Zhao and Michael Ebert
 * Section:  3
 * Description:  This program is a MIPS simulator that simulates a 5 stage
 * 		 pipelined datapath. It offers 8 commands that can be viewed by
 * 		 typing in 'h'. It will run in either an interactive mode or a
 * 		 script	mode.
 */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

int reg[27];
int dataMem[8192];
int pc = 0;
char *if_id = "empty";
char *id_exe = "empty";
char *exe_mem = "empty";
char *mem_wb = "empty";

char* cleanWord(char *word){
    char *temp = word;
    int len = strlen(word);
    while(len--){
        if (*temp == '\n' || *temp == '#'){
            *temp = '\0';
        }
        temp++;
    }
    return word;
}

int getReg(char* reg){ 
    strcpy(reg, strtok(NULL, ",()	 "));
    cleanWord(reg);
    int num = 0;
    if(!strcmp(reg, "$0") || !strcmp(reg, "$zero")){
        num = 0;
    }
    else if(!strcmp(reg, "$1") || !strcmp(reg, "1")){
        num = 27;
    }
    else if(!strcmp(reg, "$v0") || !strcmp(reg, "v0")){
    	num = 1;
    }
    else if(!strcmp(reg, "$v1") || !strcmp(reg, "v1")){
        num = 2;
    }
    else if(!strcmp(reg, "$a0") || !strcmp(reg, "a0")){
    	num = 3;
    }
    else if(!strcmp(reg, "$a1") || !strcmp(reg, "a1")){
	num = 4;
    }
    else if(!strcmp(reg, "$a2") || !strcmp(reg, "a2")){
	num = 5;
    }
    else if(!strcmp(reg, "$a3") || !strcmp(reg, "a3")){
	num = 6;
    }
    else if(!strcmp(reg, "$t0") || !strcmp(reg, "t0")){
	num = 7;
    }
    else if(!strcmp(reg, "$t1") || !strcmp(reg, "t1")){
	num = 8;
    }
    else if(!strcmp(reg, "$t2") || !strcmp(reg, "t2")){
	num = 9;
    }
    else if(!strcmp(reg, "$t3") || !strcmp(reg, "t3")){
	num = 10;
    }
    else if(!strcmp(reg, "$t4") || !strcmp(reg, "t4")){
	num = 11;
    }
    else if(!strcmp(reg, "$t5") || !strcmp(reg, "t5")){
	num = 12;
    }
    else if(!strcmp(reg, "$t6") || !strcmp(reg, "t6")){
	num = 13;
    }
    else if(!strcmp(reg, "$t7") || !strcmp(reg, "t7")){
	num = 14;
    }
    else if(!strcmp(reg, "$s0") || !strcmp(reg, "s0")){
	num = 15;
    }
    else if(!strcmp(reg, "$s1") || !strcmp(reg, "s1")){
	num = 16;
    }
    else if(!strcmp(reg, "$s2") || !strcmp(reg, "s2")){
	num = 17;
    }
    else if(!strcmp(reg, "$s3") || !strcmp(reg, "s3")){
	num = 18;
    }
    else if(!strcmp(reg, "$s4") || !strcmp(reg, "s4")){
	num = 19;
    }
    else if(!strcmp(reg, "$s5") || !strcmp(reg, "s5")){
	num = 20;
    }
    else if(!strcmp(reg, "$s6") || !strcmp(reg, "s6")){
	num = 21;
    }
    else if(!strcmp(reg, "$s7") || !strcmp(reg, "s7")){
	num = 22;
    }
    else if(!strcmp(reg, "$t8") || !strcmp(reg, "t8")){
	num = 23;
    }
    else if(!strcmp(reg, "$t9") || !strcmp(reg, "t9")){
	num = 24;
    }
    else if(!strcmp(reg, "$sp") || !strcmp(reg, "sp")){
	num = 25;
    }
    else if(!strcmp(reg, "$ra") || !strcmp(reg, "ra")){
    	num = 26;
    }
    else{
        printf("Invalid register ");
    }
    return num;
}

void getInstr(int num, char **instr){
	switch(num){
		case 0 :
			*instr = "add";
			break;
		case 1 :
			*instr = "and";
			break;
		case 2 :
			*instr = "or";
			break;
		case 3 :
			*instr = "sub";
			break;
		case 4 :
			*instr = "slt";
			break;
		case 5 :
			*instr = "sll";
			break;
		case 6 :
			*instr = "addi";
			break;
		case 7 :
			*instr = "lw";
			break;
		case 8 :
			*instr = "sw";
			break;
		case 9 :
			*instr = "j";
			break;
		case 10 :
			*instr = "jal";
			break;
		case 11 :
			*instr = "jr";
			break;
		case 12 :
			*instr = "beq";
			break;
		case 13 :
			*instr = "bne";
			break;
		default:
			break;
	}
}

void execute(int instr, int r1, int r2, int r3){
	switch(instr){
		case 0 :	// add
			reg[r1] = reg[r2] + reg[r3];
			break;
		case 1 :	// and
			reg[r1] = reg[r2] & reg[r3]; 
			break;
		case 2 :	// or
			reg[r1] = reg[r2] | reg[r3];
			break;
		case 3 :	// sub
			reg[r1] = reg[r2] - reg[r3];
			break;
		case 4 :	// slt
			reg[r1] = reg[r2] < reg[r3];
			break;
		case 5 :	// sll
			reg[r1] = reg[r2] << r3;
			break;
		case 6 :	// addi
			reg[r1] = reg[r2] + r3;
			break;
		case 7 :	// lw
			reg[r1] = dataMem[reg[r3] + r2];
			break;
		case 8 :	// sw
			dataMem[reg[r3] + r2] = reg[r1];
			break;
		case 9 :	// j
			pc = r1;
			break;
		case 10 :	// jal
			reg[26] = pc + 1;
			pc = r1 - 1;
			break;
		case 11 :	// jr
			pc = reg[r1] - 1;
			break;
		case 12 :	// beq
			if(reg[r1] == reg[r2])
				pc = r3 - 1;
			break;
		case 13 :	// bne
			if(reg[r1] != reg[r2])
				pc = r3 - 1;
			break;
		default:
			break;
		
	}
}

int main(int argc, char* argv[]){

    typedef struct labelList{
        struct labelList *next;
        char *data;
        int lineNum;
    } labelList;

    FILE * asmFile;
    FILE * script;
    char fileName[100];
    char *line = malloc(100);
    char *label = malloc(100);
    char *temp = label;
    char *word = temp;
    char *r = malloc(6);
    labelList *head = NULL;
    labelList *cur;
    int isLabel = 0;
    int lineNum = -1;
    int jLine = -1;
    int labeled = 0;
    char *addNull;
    char instr = 0;
    int i = 0;
    int arr[100][4];
    int maxLineNum = 0;
    int mStart = 0;
    int mEnd = 0;

    for(i=0;i<8192;i++){
	dataMem[i] = 0;
    }

    for(i=0;i<27;i++){
        reg[i] = 0;
    }

    asmFile = fopen(argv[1], "r");
    if(asmFile == NULL) perror ("Error opening file\n");
    else{
        while(fgets(line, 100, asmFile)){
            word = strtok(line, ", 	");
            if(*word != '#' && *word != '\n'){
                lineNum++;
            }
            do{
                temp = word;
                if(labeled && *temp == '\n'){
                    lineNum--;
                }
                labeled = 0;
                while(*temp){
                    if(*temp == '#'){
                        break;
                    }
                    else if(*temp == ':'){
                        cur = malloc(sizeof(labelList));
                        cur->data = malloc(50);
                        strcpy(cur->data, word);
                        addNull = memchr(cur->data, ':', strlen(cur->data));
                        *addNull = '\0';
                        cur->next = head;
                        cur->lineNum = lineNum;
                        head = cur;
                        labeled = 1;
                    }
                    temp++;
                }
            }while(word = strtok(NULL, ",	 "));
        }

        rewind(asmFile);
	maxLineNum = lineNum;
        lineNum = -1;

skipcomment:
        while(fgets(line, 100, asmFile)){
            word = strtok(line, "$,():	 ");
            do{
                if(*word == '#'){
                    goto skipcomment;
                }
                else if(!strcmp(word, "add")){
                    lineNum++;
        	    arr[lineNum][0] = 0; 
		    arr[lineNum][1] = getReg(r);
	            arr[lineNum][2] = getReg(r);
		    arr[lineNum][3] = getReg(r);
                    goto skipcomment;
                }
                else if(!strcmp(word, "and")){
                    lineNum++;
        	    arr[lineNum][0] = 1; 
		    arr[lineNum][1] = getReg(r);
	            arr[lineNum][2] = getReg(r);
		    arr[lineNum][3] = getReg(r);
                    goto skipcomment;
                }
                else if(!strcmp(word, "or")){
                    lineNum++;
        	    arr[lineNum][0] = 2; 
		    arr[lineNum][1] = getReg(r);
	            arr[lineNum][2] = getReg(r);
		    arr[lineNum][3] = getReg(r);
                    goto skipcomment;
                }
                else if(!strcmp(word, "sub")){
                    lineNum++;
        	    arr[lineNum][0] = 3; 
		    arr[lineNum][1] = getReg(r);
	            arr[lineNum][2] = getReg(r);
		    arr[lineNum][3] = getReg(r);
                    goto skipcomment;
                }
                else if(!strcmp(word, "slt")){
                    lineNum++;
        	    arr[lineNum][0] = 4; 
		    arr[lineNum][1] = getReg(r);
	            arr[lineNum][2] = getReg(r);
		    arr[lineNum][3] = getReg(r);
                    goto skipcomment;
                }
                else if(!strcmp(word, "sll")){
                    lineNum++;
        	    arr[lineNum][0] = 5; 
		    arr[lineNum][1] = getReg(r);
	            arr[lineNum][2] = getReg(r);
		    arr[lineNum][3] = atoi(cleanWord(strtok(NULL, ",	 ")));
                    goto skipcomment;
                }
                else if(!strcmp(word, "addi")){
                    lineNum++;
        	    arr[lineNum][0] = 6; 
		    arr[lineNum][1] = getReg(r);
	            arr[lineNum][2] = getReg(r);
		    arr[lineNum][3] = atoi(cleanWord(strtok(NULL, ",	 ")));
                    goto skipcomment;
                }
                else if(!strcmp(word, "lw")){
                    lineNum++;
        	    arr[lineNum][0] = 7; 
		    arr[lineNum][1] = getReg(r);
	            arr[lineNum][2] = atoi(strtok(NULL, "(),	 "));
		    arr[lineNum][3] = getReg(r);
                    goto skipcomment;
                }
                else if(!strcmp(word, "sw")){
                    lineNum++;
        	    arr[lineNum][0] = 8; 
		    arr[lineNum][1] = getReg(r);
	            arr[lineNum][2] = atoi(strtok(NULL, "(),	 "));
		    arr[lineNum][3] = getReg(r);
                    goto skipcomment;
                }
                else if(!strcmp(word, "j")){
                    lineNum++;
                    label = cleanWord(strtok(NULL, "(),	 "));
                    cur = head;
                    while(cur){
                        if(!strcmp(label, cur->data)){
                            jLine = cur->lineNum;
                        }
                        cur = cur->next;
                    }
        	    arr[lineNum][0] = 9; 
		    arr[lineNum][1] = jLine;
                    goto skipcomment;
                }
                else if(!strcmp(word, "jal")){
                    lineNum++;
                    label = cleanWord(strtok(NULL, "(),	 "));
                    cur = head;
                    while(cur){
                        if(!strcmp(label, cur->data)){
                            jLine = cur->lineNum;
                        }
                        cur = cur->next;
                    }
        	    arr[lineNum][0] = 10;
		    arr[lineNum][1] = jLine;
                    goto skipcomment;
                }
                else if(!strcmp(word, "jr")){
                    lineNum++;
		    arr[lineNum][0] = 11;
		    arr[lineNum][1] = getReg(r);
                    goto skipcomment;
                }
                else if(!strcmp(word, "beq")){
                    lineNum++;
		    arr[lineNum][0] = 12;
		    arr[lineNum][1] = getReg(r);
		    arr[lineNum][2] = getReg(r);
                    strcpy(label, cleanWord(strtok(NULL, ",	 ")));
                    cur = head;
                    while(cur){
                        if(!strcmp(label, cur->data)){
                            jLine = cur->lineNum;
                        }
                        cur = cur->next;
                    }
		    arr[lineNum][3] = jLine;
                    goto skipcomment;
                }
                else if(!strcmp(word, "bne")){
                    lineNum++;
		    arr[lineNum][0] = 13;
		    arr[lineNum][1] = getReg(r);
		    arr[lineNum][2] = getReg(r);
                    strcpy(label, cleanWord(strtok(NULL, ",	 ")));
                    cur = head;
                    while(cur){
                        if(!strcmp(label, cur->data)){
                            jLine = cur->lineNum;
                        }
                        cur = cur->next;
                    }
		    arr[lineNum][3] = jLine;
                    goto skipcomment;
                }
                else if(strcmp(word, "\n")){
                    cur = head;
                    while(cur){
                        if(!strcmp(word, cur->data)){
                            isLabel = 1;
                        }
                        cur = cur->next;
                    }
                    if(!isLabel){
                        printf("invalid instruction: %s\n", word);
                        return 0;
                    }
                }
            }while(word = strtok(NULL, ",()	 "));
        }
    }
    fclose(asmFile);
   
    if(argv[2] != NULL){
    	script = fopen(argv[2], "r");
    } 
    while(line){
	printf("mips> ");
	if(argv[2] != NULL){
		fgets(line, 100, script);
		printf("%s", line);
	}
	else{
		fgets(line, 100, stdin);
	}
	instr = *strtok(line, " ");
	switch(instr){
	case 'h' :
		printf("\nh = show help\n");
		printf("d = dump register state\n");
		printf("p = show pipeline registers\n");
		printf("s = step through a single clock cycle step (i.e. simulate 1 cycle and stop)\n");
		printf("s num = step through num clock cycles\n");
		printf("r = run until the program ends and display timing summary\n");
		printf("m num1 num2 = display data memory from location num1 to num2\n");
		printf("c = clear all registers, memory, and the program counter to 0\n");
		printf("q = exit the program\n");
		break;
            case 'd' :
		printf("\npc = %d\n", pc);
		printf("$0 = %d 		$v0 = %d		$v1 = %d		$a0 = %d\n", reg[0], reg[1], reg[2], reg[3]);
		printf("$a1 = %d		$a2 = %d		$a3 = %d		$t0 = %d\n", reg[4], reg[5], reg[6], reg[7]);
		printf("$t1 = %d		$t2 = %d		$t3 = %d		$t4 = %d\n", reg[8], reg[9], reg[10], reg[11]);
		printf("$t5 = %d		$t6 = %d		$t7 = %d		$s0 = %d\n", reg[12], reg[13], reg[14], reg[15]);
		printf("$s1 = %d		$s2 = %d		$s3 = %d		$s4 = %d\n", reg[16], reg[17], reg[18], reg[19]);
		printf("$s5 = %d		$s6 = %d		$s7 = %d		$t8 = %d\n", reg[20], reg[21], reg[22], reg[23]);
		printf("$t9 = %d		$sp = %d		$ra = %d		\n", reg[24], reg[25], reg[26]);
		break;
            case 's' :
		mem_wb = exe_mem;
		exe_mem = id_exe;
		id_exe = if_id;
		getInstr(arr[pc][0], &if_id);
		printf("pc	if/id	id/exe	exe/mem	mem/wb\n");
		printf("%d	%s	%s	%s	%s\n", pc, if_id, id_exe, exe_mem, mem_wb);
		
		label = strtok(NULL, " ");
		if(label == NULL){
			i = 1;
		}
		else{
			i = atoi(label);
		}	
		printf("	%d instruction(s) executed\n", i);
		while(i--){
			execute(arr[pc][0], arr[pc][1], arr[pc][2], arr[pc][3]);
			pc++;
		}
                break;
	    case 'p' :
		printf("pc	if/id	id/exe	exe/mem	mem/wb\n");
		printf("%d	%s	%s	%s	%s\n", pc, if_id, id_exe, exe_mem, mem_wb);
		break;
            case 'r' :
		while(pc != maxLineNum + 1){
			execute(arr[pc][0], arr[pc][1], arr[pc][2], arr[pc][3]);
			pc++;
		}
		printf("Program complete\n");
                break;
            case 'm' :
		mStart = atoi(strtok(NULL, " "));
		mEnd = atoi(strtok(NULL, " "));
                for(i = mStart; i<=mEnd; i++){
			printf("[%d] = %d\n", i, dataMem[i]);
		}
		break;
            case 'c' :
		pc = 0;
   		for(i=0;i<8192;i++){
			dataMem[i] = 0;
    		}
    		for(i=0;i<27;i++){
        		reg[i] = 0;
    		}
		printf("	Simulator reset\n");	
		break;
            case 'q' :
                return;
            default:
                break;
	}
    }
    fclose(script);
    return 0;
}

