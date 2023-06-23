#include "project02.h"
#define SIZE 4

struct Queue {
    int time;
    int opcode;
    int destination;
    int leftOperand;
    int rightOperand;
    struct Queue* next;
};

void enterQueue(struct Queue **head, struct Queue **rear, int time, int opcode, int destination, int leftOperand, int rightOperand) {
    struct Queue* stage = (struct Queue*)malloc(sizeof(struct Queue));
    stage->time = time;
    stage->opcode = opcode;
    stage->destination = destination;
    stage->leftOperand = leftOperand;
    stage->rightOperand = rightOperand;
    stage->next = NULL;

    if (*head == NULL) {
        *head = stage;
        *rear = *head;
    } else {
        (*rear)->next = stage;
        *rear = (*rear)->next;
    }

    return;
}

void printQueue (struct Queue* head, struct Queue* rear) {
    struct Queue* current = head;
    while (current != NULL) {
        printf("%d -> ", current->time);
        current = current->next;
    }
    return;
}

int moveInPipe(Simple_Pipe& simple_pipe, struct Queue **head) {
    if ((*head)->time > 1) {
        (*head)->time--;
        return 0;
    }

    int opcode = (*head)->opcode;
    int destination = (*head)->destination;
    int leftOperand = (*head)->leftOperand;
    int rightOperand = (*head)->rightOperand;

    if (opcode == 0) simple_pipe.registers[destination] = leftOperand;
    else if (opcode == 1) {
        simple_pipe.registers[destination] = simple_pipe.registers[leftOperand] + simple_pipe.registers[rightOperand];
    } else if (opcode == 2) {
        simple_pipe.registers[destination] = simple_pipe.registers[leftOperand] + rightOperand;
    } else if (opcode == 3) {
        simple_pipe.registers[destination] = simple_pipe.registers[leftOperand] - simple_pipe.registers[rightOperand];
    } else if (opcode == 4) {
        simple_pipe.registers[destination] = simple_pipe.registers[leftOperand] - rightOperand;
    } else if (opcode == 5) {
        simple_pipe.registers[destination] = simple_pipe.registers[leftOperand] * simple_pipe.registers[rightOperand];
    } else if (opcode == 6) {
        simple_pipe.registers[destination] = simple_pipe.registers[leftOperand] * rightOperand;
    } else if (opcode == 7) {
        simple_pipe.registers[destination] = (int)(simple_pipe.registers[leftOperand] / simple_pipe.registers[rightOperand]);
    } else if (opcode == 8) {
        simple_pipe.registers[destination] = (int)(simple_pipe.registers[leftOperand] / rightOperand);
    }
    
    // Move head to next node
    struct Queue* temp = *head;
    *head = (*head)->next;
    free(temp);
    return 1;
}

int findTime(unsigned char opcode) {
    if (opcode == 0x00 || opcode == 0x10 || opcode == 0x11 || opcode == 0x20 || opcode == 0x21) return 1;
    if (opcode == 0x30 || opcode == 0x31) return 2;
    return 4;
}

int findOpcode(unsigned char opcode) {
    if (opcode == 0x00) return 0;
    if (opcode == 0x10) return 1;
    if (opcode == 0x11) return 2;
    if (opcode == 0x20) return 3;
    if (opcode == 0x21) return 4;
    if (opcode == 0x30) return 5;
    if (opcode == 0x31) return 6;
    if (opcode == 0x40) return 7;
    return 8;
}

int main(int argc, char *argv[]){

    Simple_Pipe simple_pipe;
    for(int i = 0; i < REG_COUNT; i++){
        simple_pipe.registers[i] = 0;
    }

    unsigned char instruction[SIZE];
    FILE *fp;
    fp = fopen(argv[1], "rb");

    // initiate the waiting queue
    struct Queue* head = NULL;
    struct Queue* rear = NULL;

    if (fp) {
        while (fread(instruction, sizeof *instruction, SIZE, fp) == 4) {
            int time = findTime(instruction[3]);
            int opcode = findOpcode(instruction[3]);
            int destination = (int)instruction[2];
            int leftOperand = (int)instruction[1];
            int rightOperand = (int)instruction[0];

            execution_time++;
            request_done++;
            enterQueue(&head, &rear, time, opcode, destination, leftOperand, rightOperand);
            moveInPipe(simple_pipe, &head);
        }

        fclose(fp);
    }

    // complete the remaining instructions
    while (head != NULL) {
        execution_time++;
        moveInPipe(simple_pipe, &head);
    }

    // Add initial delay to execution time
    execution_time += 2;

    simple_pipe.print_regs();
    std::cout << "Total execution cycles: " << execution_time << std::endl;
    std::cout << "\nIPC: " << (request_done/(double)execution_time) << std::endl << std::endl;

    return 0;
}



void Simple_Pipe::print_regs(){
    printf("\nRegisters: \n");
    std::cout << "----------------------------------------" << std::endl;
    for(int i = 0; i < REG_COUNT; i+=2){
        std::string regl("R");
        regl.append(std::to_string(i));
        regl.append(": ");

        std::string regr("R");
        regr.append(std::to_string(i+1));
        regr.append(": ");
        if(i < 15){
            std::cout << "  " << std::setiosflags(std::ios::left) 
            << std::setw(5) << regl  << std::setw(10) << registers[i] << " |   " 
            << std::setw(5) << regr << std::setw(10) << registers[i+1] << std::endl;
            std::cout << "----------------------------------------" << std::endl;
        }else{
            std::cout << "  " << std::setiosflags(std::ios::left) 
            << std::setw(5) << regl << std::setw(10) << registers[i] << " |   " << std::endl;
            std::cout << "----------------------------------------" << std::endl;
        }
    }  
    printf("\n");
}
