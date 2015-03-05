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

/*
 * instruction: contains the string of the instruction in the register.
 * branchTaken: is this branch taken (depreciated)
 * branchLocation: the CORRECT location of the PC after this instruction.
 *                 (NOT always the location if the branch is taken.)
 * branchCorrect: did the predictor correctly predict whether this branch was taken?
 *                 (if predictor was wrong, PC will snap to branchLocation 
 *                 sometime in the pipeline.)
 *
 *Note: branchTaken is depreciated. It shouldn't be necessary anymore,
 * but I don't want to risk deleting it and having bugs come up.
 */
typedef struct{
    char* instruction;
    int branchTaken;
    int branchLocation;
    int branchCorrect;
}interstageRegister;

//stores the register values of the MIPS registers that we are implementing.
int reg[28];
//Program counter
int pc = 0;

// simulated RAM
int dataMem[8192];

//instruction memory
int arr[100][4];

//used for pipeline things.
int sim_pc = 0;
int num_instr = 0;
int cycles = 0;

interstageRegister if_id = {.instruction = "empty", .branchTaken = 0, .branchLocation = 0, .branchCorrect = 0};
interstageRegister id_exe = {.instruction = "empty", .branchTaken = 0, .branchLocation = 0, .branchCorrect = 0};
interstageRegister exe_mem = {.instruction = "empty", .branchTaken = 0, .branchLocation = 0, .branchCorrect = 0};
interstageRegister mem_wb = {.instruction = "empty", .branchTaken = 0, .branchLocation = 0, .branchCorrect = 0};

//Predictor stuff
int GHR = 0;
int GHRSize = 0;
int *selectorTable;
int correctPredictions = 0;
int totalPredictions = 0;

/*strips everything after the first '#' in the string, including the '#'.
 * 
 */
void stripComments(char* line){
    char* commentPos;
    commentPos = strchr(line, '#');
    if(commentPos == NULL){
    commentPos = line + strlen(line);
    }
    *commentPos = '\0';
    return;
}

/*clamps |target| to a value between |min| and |max|.
 */
static int clampInt(int target, int min, int max){
    if(target < min){
    return min;
    }
    if(target > max){
    return max;
    }
    return target;
}


void initializeSelectorTable(void){
    selectorTable = calloc(sizeof(int), (1<<GHRSize));
}

void destroySelectorTable(void){
    free(selectorTable);
}

/* Predicts whether the next instruction will branch or not.
 * returns 1 if it will branch, 0 if it will not.
 * To change the prediction algorithm, do it here.
 */
int branchPredict(void){
    totalPredictions++;
    return selectorTable[GHR] >= 2;
}

/* updates the global history register (used in branch prediction).
 */
void updateGHR(int pathTaken){
GHR <<= 1;
GHR &= (1 << GHRSize) - 1;
GHR |= (1 & pathTaken);
}

/* Updates the branch prediction data.
 * To change the prediction algorithm, this will probably also need to be changed.
 */
void updatePredict(int pathTaken){
    selectorTable[GHR] += ((pathTaken*2) - 1);
    selectorTable[GHR] = clampInt(selectorTable[GHR], 0, 3);
    updateGHR(pathTaken);
} 

/* Strips comments and labels from a string.
 * Removes everything including and after the first '#'
 * and before and including the  first ':'.
 *
 * Note: this means that multiple labels on the same line are not supported.
 */
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

/* Cleans a word.
 */
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

/* Takes in a register string, and outputs our internal number for the register.
 * 
 * Note: the number returned does NOT necessarily equal the actual number of the
 * register. Ex: register $1 is 27 in our implementation.
 *
 * IF YOU ADD REGISTERS: make sure to increase the size of the reg[] global array.
 */
int strToReg(char* reg){ 
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

/* takes a string and outputs the address of the instruction that the label points to.
 * returns -1 if not a label
 */
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

/*Takes in an argument (could be a label or an integer) and returns the value of
 * that argument. If arg is an int, it returns that int. If arg is a label,
 * returns the address of the instruction that the label points to.
 */
int strToImm(char* arg, labelList* labelHead){
    int instrAddr = strToLabel(arg, labelHead);
    if(instrAddr != -1){
        return instrAddr;
    }
    else{
        return atoi(arg);
    }
}

/* Takes a number, and returns the string of that corresponding instruction.
 */
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

/*Takes a string instruction, and returns the number of that instruction.
 */
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
	printf("invalid instruction %s. Interpreting as add.\n", instr);
	return 0;
}

/* Executes an instruction with the corresponding arguments.
 * PREREQUISITES: PC has NOT been incremented yet (i.e., it is at the same
 * instruction that is currently executing).
 * If the instruction is a jomp or branch, modifies the PC to point to the next
 * instruction.
 */
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
			pc = r1;
			break;
		case 11 :	// jr
			pc = reg[r1];
			break;
		case 12 :	// beq
			if(reg[r1] == reg[r2])
				pc = pc + r3 + 1;
			break;
		case 13 :	// bne
			if(reg[r1] != reg[r2])
				pc = pc + r3 + 1;
			break;
		default:
			break;
		
	}
	num_instr++;
}

/* Doesn't actually do anything except change the PC. Used for instructions that
 * will be invalidated by a branch.
*/
void fakeexecute(int instr, int r1, int r2, int r3){
	switch(instr){
		case 9 :	// j
			pc = r1;
			break;
		case 10 :	// jal
			//reg[26] = pc + 1;
			pc = r1;
			break;
		case 11 :	// jr
			pc = reg[r1];
			break;
		case 12 :	// beq
			if(reg[r1] == reg[r2])
				pc = pc + r3 + 1;
			break;
		case 13 :	// bne
			if(reg[r1] != reg[r2])
				pc = pc + r3 + 1;
			break;
		default:
			break;
		
	}
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

/* Takes in a variety of parameters. Basic function is to take an instruction
 * argument as a string, and using the instruction, return the correct integer
 * value for that argument. 
 *
 * Example: for ADD instructions, transforms the arg
 * into a register for all arguments (as all arguments of ADD are registers).
 * for LW/SW, arguments 1 and 3 are registers, but argument 2 is an immediate,
 * so correctArg() will return the correct register for arguments 1 and 3, and
 * the correct immediate value for argument 2.
 *
 * Note: argNum starts from 1, not 0 - arg 0 is the instruction!
 */
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
                
                int labelAddr = strToLabel(arg, labelHead);
                //if arg is a label
                if(labelAddr != -1){
                    return labelAddr - ( instrNum + 1 );
                }
                else{//is not a label, is a relative
                    return atoi(arg);
                }
    	    }
    	    break;
    	default:
    	    printf("Error: instruction %i not valid.",instr);
    	    return 999;
    		break;
    }
}

/* Next 5 functions: Hackishly simulate a 5-stage processor pipeline.
*/
void writebackStage(void){
    mem_wb.instruction = "empty";
    return;
}

void memoryStage(void){
    //has the beq squash
    if(!strcmp(mem_wb.instruction, "empty")){
        mem_wb.instruction = exe_mem.instruction;
        mem_wb.branchTaken = exe_mem.branchTaken;
        mem_wb.branchLocation = exe_mem.branchLocation;
        mem_wb.branchCorrect = exe_mem.branchCorrect;
        if(exe_mem.branchCorrect == 0
        && (!strcmp(exe_mem.instruction, "beq")
         || !strcmp(exe_mem.instruction, "bne"))){
            exe_mem.instruction = "squash";
            id_exe.instruction = "squash";
            if_id.instruction = "squash";
            pc = exe_mem.branchLocation;
        }
        else{
            exe_mem.instruction = "empty";
        }
    }
}

void executeStage(void){
    //this stage has the LW stall
    //if exe_mem register is free
    if(!strcmp(exe_mem.instruction, "empty")){
        exe_mem.instruction = id_exe.instruction;
        exe_mem.branchTaken = id_exe.branchTaken;
        exe_mem.branchLocation = id_exe.branchLocation;
        exe_mem.branchCorrect = id_exe.branchCorrect;
        //need to figure out how detectStall works, feed in right parameters
        //I do know that detectStall checks for "lw", so no need for that here
        if(detectStall(pc - 1,id_exe.instruction)){
            id_exe.instruction = "stall";
        }
        else{
            id_exe.instruction = "empty";
        }
    }
}
        
void decodeStage(void){
    //if id_exe register is free
    if(!strcmp(id_exe.instruction, "empty")){
        id_exe.instruction = if_id.instruction;
        id_exe.branchTaken = if_id.branchTaken;
        id_exe.branchLocation = if_id.branchLocation;
        id_exe.branchCorrect = if_id.branchCorrect;
        //j or jal or jr (add later) AND jumped
        if(1 == 1
        && (!strcmp(if_id.instruction, "j")
         || !strcmp(if_id.instruction, "jr")
         || !strcmp(if_id.instruction, "jal"))){
            if_id.instruction = "squash";
            pc = if_id.branchLocation;
        }
        else{
            if_id.instruction = "empty";
        }
    }
}

void fetchStage(void){
    if(!strcmp(if_id.instruction, "empty")){
        int unbranchedPC = pc;
        //fetch new instruction at PC
        if_id.instruction = numToInstr(arr[pc][0]);
        if((exe_mem.branchCorrect == 0
        && (!strcmp(exe_mem.instruction, "beq")
         || !strcmp(exe_mem.instruction, "bne")))
        ||(id_exe.branchCorrect == 0
        && ((!strcmp(id_exe.instruction, "beq")
         || !strcmp(id_exe.instruction, "bne"))))){
             fakeexecute(arr[pc][0], arr[pc][1], arr[pc][2], arr[pc][3]);
         }
         else{
            execute(arr[pc][0], arr[pc][1], arr[pc][2], arr[pc][3]);
            if(!strcmp(if_id.instruction, "beq")
            ||!strcmp(if_id.instruction, "bne")){
                int wasBranchTaken = (unbranchedPC != pc);
                //change branchPredict() based on what model you
                //are using(ex: predict never branch, always branch,
                // some other model...)
                int prediction = branchPredict();
                
                int predictionCorrect = (prediction == wasBranchTaken);
                 
                if_id.branchTaken = wasBranchTaken;
                if_id.branchCorrect = predictionCorrect;
                //if branch was not taken, next instruction is at
                //(currentPC + 1).
                if(!wasBranchTaken){
                    pc++;
                }
                if_id.branchLocation = pc;
                //update prediction stuff
                correctPredictions += predictionCorrect;
                updatePredict(wasBranchTaken);
                //HACK to get this to work:
                //(ideally, we wouldn't use arr[unbrachedpc][3])
                //if it predicts it does branch,
                //set the PC to the branched instruction(even if its wrong).
                //(it'll be fixed in the mem stage)
                if(prediction == 1){
                    pc = unbranchedPC + arr[unbranchedPC][3];
                }
                else { //(prediction == 0)
                    pc = unbranchedPC;
                }
            }
            else{
             if_id.branchTaken = 1;
             if_id.branchLocation = pc;
             if_id.branchCorrect = 1;
             pc = unbranchedPC;
            }
        }
        pc++;
    }
}

/* Outputs memory locations 0 to 668 into coordinates.csv, in 2 rows.
 */
void outputCoords(void){
    FILE* coordFile = fopen("coordinates.csv", "w");
    if(coordFile == NULL){
        printf("Writing coordinates.csv failed!");
    }
    //loop through each memory location, writing stuff each time.
    //668 = hardcoded # of values to print.
    for(int i = 0; i < 668; i++){
        fprintf(coordFile, "%i", dataMem[i]);
        if(i%2 == 0){
            fputs(",", coordFile);
        }
        else{
            fputs("\n", coordFile);
        }
    }
    fclose(coordFile);
}

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

    if(argc > 3){
        //does this segfault?
        GHRSize = atoi(argv[3]);
    }
    else{
        GHRSize = 2;
    }
    initializeSelectorTable();
    
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
    		while(numLines--){
                writebackStage();
                memoryStage();
                executeStage();
                decodeStage();
                fetchStage();
                cycles++;
            }
            printf("\npc	if/id	id/exe	exe/mem	mem/wb\n");
            printf("%d	%s	%s	%s	%s\n\n", pc, if_id.instruction, id_exe.instruction, exe_mem.instruction, mem_wb.instruction);
            break;
    	case 'p' :
    		printf("\npc	if/id	id/exe	exe/mem	mem/wb\n");
    		printf("%d	%s	%s	%s	%s\n\n", pc, if_id.instruction, id_exe.instruction, exe_mem.instruction, mem_wb.instruction);
    		break;
        case 'r' :
    		while (pc != maxLineNum) {
                writebackStage();
                memoryStage();
                executeStage();
                decodeStage();
                fetchStage();
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
        case 'b':
            //output branch predictor accuracy
            printf("accuracy %.2lf% (%i correct predictions, %i predictions)\n",
             ((double)correctPredictions/totalPredictions) * 100,
             correctPredictions,
             totalPredictions);
            break;
        case 'q' :
            line = NULL;
            break;
        //output a comma separated listing of the x,y coords to coordinates.csv
        case 'o':
            outputCoords();
            break;
        default:
            break;
    	}
    }
    //fclose(script);
    return 0;
}



