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

#define WHITESPACE_AND_PUNCTUATION "$,(): \t\n"

typedef struct labelList{
    struct labelList *next;
    char *data;
    int lineNum;
} labelList;

int reg[27];
int dataMem[8192];
int arr[100][4];
int pc = 0;
int sim_pc = 0;
int num_instr = 0;
int cycles = 0;
char *if_id = "empty";
char *id_exe = "empty";
char *exe_mem = "empty";
char *mem_wb = "empty";

void stripComments(char* line){
    char* commentPos;
    commentPos = strchr(line, '#');
	if(commentPos == NULL){
		commentPos = line + strlen(line);
	}
	*commentPos = '\0';
	return;
}
	

void stripCommentsAndLabels(char* line){
	char *colonAddr, *strippedLine;
	int newLineLength;
	int colonPos;
	//first strip comments
	stripComments(line);
	//now strip label
	colonAddr = strchr(line, ':');
	if(colonAddr == NULL){
		colonAddr = line - 1;
	}
	colonPos = colonAddr - line;
	//don't include the colon in the line
	newLineLength = strlen(line) - (colonPos+1);
	strippedLine = malloc(sizeof(char) * newLineLength+1);
	if(newLineLength>0){
		strncpy(strippedLine, colonAddr+1, newLineLength);
	}
	strippedLine[newLineLength] = '\0';
	strcpy(line, strippedLine);
	free(strippedLine);
	return;
}

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

int strToReg(char* reg){ 
    //strcpy(reg, strtok(NULL, ",()	 "));
    //cleanWord(reg);
    int num;
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
        //might be direct numbered register
        num = atoi(reg);
        if(num < 0 || num > 27){
            num = -1;
            printf("Invalid register ");
        }
    }
    return num;
}

//returns -1 if not a label
int strToLabel(char* arg, labelList* labelHead){
    while(labelHead != NULL){
        //if this is correct label
        if(!strcmp(labelHead->data, arg)){
            return labelHead->lineNum;
        }
        //otherwise
        labelHead = labelHead->next;
    }
    //if no correct labels
    return -1;
}

//if arg is a label, returns the instruction address the label points to.
int strToImm(char* arg, labelList* labelHead){
    int instrAddr = strToLabel(arg, labelHead);
    if(instrAddr != -1){
        return instrAddr;
    }
    else{
        return atoi(arg);
    }
}

char* numToInstr(int num){
	switch(num){
		case 0 :
			return "add";
			break;
		case 1 :
			return "and";
			break;
		case 2 :
			return "or";
			break;
		case 3 :
			return "sub";
			break;
		case 4 :
			return "slt";
			break;
		case 5 :
			return "sll";
			break;
		case 6 :
			return "addi";
			break;
		case 7 :
			return "lw";
			break;
		case 8 :
			return "sw";
			break;
		case 9 :
			return "j";
			break;
		case 10 :
			return "jal";
			break;
		case 11 :
			return "jr";
			break;
		case 12 :
			return "beq";
			break;
		case 13 :
			return "bne";
			break;
		default:
		    return "ERR";
			break;
	}
}
int instrToNum(char* instr){
	if(!strcmp(instr, "add")){
		return 0;
	}
	if(!strcmp(instr, "and")){
		return 1;
	}
	if(!strcmp(instr, "or")){
		return 2;
	}
	if(!strcmp(instr, "sub")){
		return 3;
	}
	if(!strcmp(instr, "slt")){
		return 4;
	}
	if(!strcmp(instr, "sll")){
		return 5;
	}
	if(!strcmp(instr, "addi")){
		return 6;
	}
	if(!strcmp(instr, "lw")){
		return 7;
	}
	if(!strcmp(instr, "sw")){
		return 8;
	}
	if(!strcmp(instr, "j")){
		return 9;
	}
	if(!strcmp(instr, "jal")){
		return 10;
	}
	if(!strcmp(instr, "jr")){
		return 11;
	}
	if(!strcmp(instr, "beq")){
		return 12;
	}
	if(!strcmp(instr, "bne")){
		return 13;
	}
	//invalid instruction!
	return 999;
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
				pc = pc + r3 - 1;
			break;
		case 13 :	// bne
			if(reg[r1] != reg[r2])
				pc = pc + r3 - 1;
			break;
		default:
			break;
		
	}
	num_instr++;
}

int detectStall (int pcount, char *idexe) {
	int lwReg;
	int stall = 0;
	if (pcount  && !strcmp(idexe, "lw")) {
		lwReg = arr[pcount - 1][1];
		if (arr[pcount][0] < 5) { // if depends on both r2 and r3
			if (arr[pcount][2] == lwReg || arr[pcount][3] == lwReg)
				stall = 1;
		}
		else if (arr[pcount][0] == 5 || arr[pcount][0] == 6) { // depends on only on r2
			if (arr[pcount][2] == lwReg) 
				stall = 1;
		}
		else if (arr[pcount][0] == 7) { // depends only on r3
			if (arr[pcount][3] == lwReg)
				stall = 1;
		}
		else if (arr[pcount][0] == 8) { // depends on r1 and r3
			if (arr[pcount][1] == lwReg || arr[pcount][3] == lwReg)
				stall = 1;
		}
		else if (arr[pcount][0] == 11) { // depends only on r1
			if (arr[pcount][1] == lwReg)
				stall = 1;
		}
		else if (arr[pcount][0] > 11) { // depends on r1 and r2
			if (arr[pcount][1] == lwReg || arr[pcount][2] == lwReg)
				stall = 1;
		}
	}
	return stall;
}
/*
//EX hazard
    if((incomingInstruction.input1 == otherInstruction.output
    || incomingInstruction.input2 == otherInstruction.output)
    && otherInstrucion.output != register0){
        //forward instruction - implement later
    }
    //memory hazard also does forwarding
    */
    


//Note: argNum starts from 1, not 0 - arg 0 is the instruction!
int correctArg(int instrNum, int instr, int argNum, char* arg, labelList* labelHead){
    switch(instr){
        //All registers
    	case 0 :	// add
    	case 1 :	//and
    	case 2 :	// or
    	case 3 :	// sub
    	case 4 :	// slt
    	case 5 :	// sll
    	case 11 :	// jr
            return strToReg(arg);
            break;
        //arg1, arg2 are registers, arg3 is immediate
    	case 6 :	// addi
    	    if(argNum == 1 || argNum == 2){
    	        return strToReg(arg);
    	    }
    	    else{
    	        return strToImm(arg, labelHead);
    	    }
    	    break;
        //arg1, arg3 are registers, arg2 is immediate
    	case 7 :	// lw
    	case 8 :	// sw
    	     if(argNum == 1 || argNum == 3){
    	        return strToReg(arg);
    	    }
    	    else{
    	        return strToImm(arg, labelHead);
    	    }
    	    break;
    	case 9 :	// j
    	case 10 :	// jal
    		return strToImm(arg, labelHead);
    		break;
    	//beq and bne require a relative address, not absolute
    	case 12 :	// beq
    	case 13 :	// bne
    	    if(argNum == 1 || argNum == 2){
    	        return strToReg(arg);
    	    }
    	    else{
    	        return strToImm(arg, labelHead) - (instrNum + 1);
    	    }
    	    break;
    	default:
    	    printf("Error: instruction %i not valid.",instr);
    	    return 999;
    		break;
    }
}
/*
struct MIPSemulator{
    if_id;
    id_ex;
    ex_mem;
    mem_wb;
    int* regFile;
    
}
MIPSemulatorStep(MIPSemulator* emulator){
    //first make it w/o hazard detection
    MIPSemulator_writeBack();
    MIPSemulator_memory();
    MIPSemulator_execute();
    MIPSemulator_decode();
    MIPSemulator_fetch();
}

MIPSemulator_fetch(MIPSemulator* this){
    this->if_id = getInstruction(programCounter);
    programCounter++;
}

MIPSemulator_decode(MIPSemulator* this){
    //get arguments from registers, including branch targets
    reg_ctrl = if_id stuff;
    id_ex->arg1 = regFile[if_id->regarg1];
    //etc.
}
MIPSemulator_execute(MIPSemulator* this){
    //do computation based on things in id_ex;
}

//write results to register file
MIPSemulator_writeBack(MIPSemulator* emulator){
    emulator->regFile[mem_wb->target] = mem_wb->data;
}

//access memory
MIPSemulator_memory(){
    //stuff
    mem_wb = morestuff;
}
    */

int main(int argc, char* argv[]){
    FILE * asmFile;
    FILE * script;
    char *line = malloc(100);
    char *label = malloc(100);
    char *temp = label;
    char *word = temp;
    labelList* labelHead = NULL;
    int lineNum = -1;
    char instr = 0;
    int i = 0;
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
    if(asmFile == NULL){
        perror ("Error opening file\n");
        return 1;
    }
    //parse labels
    //each label consists of 2 parts:
    //  A label name
    //  the instruction number the label is at
    int instructionNum = 0;
    while(fgets(line, 100, asmFile)){
        char* colonAddr;
        stripComments(line);
        //is there a label on this line?
        if(colonAddr = strchr(line, ':')){
            //if yes, add the label to the list.
            labelList* newLabel;
            int labelSize = 0;
            char* labelChar = colonAddr - 1;
            //get size and beginning of label string
            while(isalnum(*labelChar) && labelChar >= line){
                labelSize++;
                labelChar--;
            }
            //initialize new label
            newLabel = malloc(sizeof(*newLabel));
            newLabel->data = malloc(sizeof(char) * labelSize + 1);
            strncpy(newLabel->data, labelChar + 1, labelSize);
            newLabel->data[labelSize] = '\0';
            newLabel->lineNum = instructionNum;
            //NOTE: labels pushed in order, so
            // last label encountered = first label in list.
            newLabel->next = labelHead;
            labelHead = newLabel;
        }
        //is there an instruction on this line?
        //(actually just checks to see that there is SOMETHING that isn't a
        //label, comment, whitespace, or punctuation)
        stripCommentsAndLabels(line);
        if(strspn(line, WHITESPACE_AND_PUNCTUATION) != strlen(line)){
            //something there
            instructionNum++;
        }
    }
            
     /*       
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
    }*/

    rewind(asmFile);    
    maxLineNum = instructionNum;
    lineNum = -1;



    //Transfer instructions into memory
    while(fgets(line, 100, asmFile)){
    	stripCommentsAndLabels(line);
        word = strtok(line, WHITESPACE_AND_PUNCTUATION);
        //if there is an instruciton on this line
        if(word != NULL){
        	int i = 0;
        	lineNum++;
        	arr[lineNum][i] = instrToNum(word);
        	i++;
        	while(word = strtok(NULL, WHITESPACE_AND_PUNCTUATION)){
        		arr[lineNum][i] = correctArg(lineNum, arr[lineNum][0], i, word, labelHead);
        		i++;
        	}
        }
        //else word == null, do nothing
    }
    fclose(asmFile);
   
    if(argc > 2){
    	script = fopen(argv[2], "r");
    } 
    while(line){
        char* commandArg;
        int numLines;
    	printf("mips> ");
    	if(argc > 2){
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
            //get number of instructions to execute
    		commandArg = strtok(NULL, " ");
    		if(commandArg == NULL){
    			numLines = 1;
    		}
    		else{
    			numLines = atoi(commandArg);
    		}
    		//execute the instructions
    		static int oldPC = 0;
    		while(numLines--){
                int unbranchedPC = pc + 1;
    			execute(arr[pc][0], arr[pc][1], arr[pc][2], arr[pc][3]);
    			pc++;
    
    			if (!detectStall(sim_pc - 1, id_exe)) {
                    if(oldPC != sim_pc && sim_pc){
                        //branch taken, so squash
                        if(arr[oldPC][0] == instrToNum("j") ||
                        arr[oldPC][0] == instrToNum("jr") ||
                        arr[oldPC][0] == instrToNum("jal")){
			    sim_pc = arr[oldPC][1];
			    printf("oldPC = %d\n", oldPC);
			    printf("sim_pc = %d\n", sim_pc);
                            mem_wb = exe_mem;
                            exe_mem = id_exe;
                            id_exe = if_id;
                            if_id = "squash";
                        }
                        if(arr[unbranchedPC - 1][0] == instrToNum("beq") ||
                        arr[unbranchedPC - 1][0] == instrToNum("bne")){
                            mem_wb = exe_mem;
                            exe_mem = "squash";
                            id_exe = "squash";
                            if_id = "squash";
                        }
			//sim_pc++;
                    }
		    else {
                    	mem_wb = exe_mem;
                    	exe_mem = id_exe;
                 	id_exe = if_id;
                 	if_id = numToInstr(arr[sim_pc][0]);
                   	sim_pc++;
		    }
                }
                else {
                    mem_wb = exe_mem;
                    exe_mem = id_exe;
                    id_exe = "stall";
                }
		oldPC = unbranchedPC;
                cycles++;
                if (numLines == 0) {
                    printf("\npc	if/id	id/exe	exe/mem	mem/wb\n");
                    printf("%d	%s	%s	%s	%s\n\n", sim_pc, if_id, id_exe, exe_mem, mem_wb);
                }
            }
            break;
    	case 'p' :
    		printf("\npc	if/id	id/exe	exe/mem	mem/wb\n");
    		printf("%d	%s	%s	%s	%s\n\n", sim_pc, if_id, id_exe, exe_mem, mem_wb);
    		break;
        case 'r' :
    		while(pc != maxLineNum){
    			execute(arr[pc][0], arr[pc][1], arr[pc][2], arr[pc][3]);
    			pc++;
    		}
    		while (sim_pc != maxLineNum) {
    			if (!detectStall(sim_pc - 1, id_exe)) {
    				mem_wb = exe_mem;
    				exe_mem = id_exe;
    				id_exe = if_id;
    				if_id = numToInstr(arr[sim_pc][0]);
    				sim_pc++;
    			}
    			else {
    				mem_wb = exe_mem;
    				exe_mem = id_exe;
    				id_exe = "stall";
    			}
    			cycles++;
    		}
    		printf("\nProgram complete\n");
    		printf("CPI = %0.3lf\tCycles = %d\tInstructions = %d\n\n", (double)(cycles + 4)/num_instr, cycles + 4, num_instr);
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
    		printf("\tSimulator reset\n");	
    		break;
        case 'q' :
            line = NULL;
            break;
        default:
            break;
    	}
    }
    //fclose(script);
    return 0;
}

