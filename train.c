
#include <kernel.h>


// **************************
// run the train application
// **************************

#define CMD_END '\015'
#define TRAIN_ID "20"

int window_id;

// to the tail of dest
char* append_string(char* src, char* dest) {
	while (*dest != 0) {
		*dest++;
	}
	while (*src != 0) {
		*dest++ = *src++;
	}
	return dest;
}

void change_switch(char switch_id, char switch_fork) {
	char command[5];
	command[0] = 'M';
	command[1] = switch_id;
	command[2] = switch_fork;
	command[3] = CMD_END;
	command[4] = 0;
	comm_train(command, "", 0);
}

void comm_train(char* command, char* result, int input_len) {
	COM_Message com_msg;
	com_msg.output_buffer = command;
   	com_msg.len_input_buffer = input_len;
   	com_msg.input_buffer = result;
	send(com_port, &com_msg);
}

void clear_contact() {
	char command[2];
	command[0] = 'R';
	command[1] = CMD_END;
	comm_train(command, "", 0);
}

void retrieve_contact(char contact_id) {
	clear_contact();
	char command[3];
	command[0] = 'C';
	command[1] = contact_id;
	command[2] = CMD_END;
	char* result;
	comm_train(command, result, 3);
	return *result == '*' && *(result + 1) == '1';
}

void change_train_speed(char speed) {
	char command[7];
	command[0] = 'L';
	char* tail = append_string(TRAIN_ID, command + 1);
	command[tail++ - command] = 'S';
	command[tail++ - command] = speed;
	command[tail++ - command] = CMD_END;
	comm_train(command, "", 0);
}

void change_train_direction() {
	char command[7];
	command[0] = 'L';
	char* tail = append_string(TRAIN_ID, command + 1);
	command[tail++ - command] = 'D';
	command[tail++ - command] = CMD_END;
	comm_train(command, "", 0);
}

void train_process(PROCESS self, PARAM param) {
	window_id = wm_create(10, 2, 50, 20);
	change_switch('1', 'G');
	change_switch('4', 'G');
	change_switch('5', 'G');
	change_switch('8', 'G');
	change_switch('9', 'R');
	change_train_speed('4');
	become_zombie();
}

void init_train() {
	create_process(train_process, 5, 0, "Train Process");
	resign();
}
