
#include <kernel.h>


// **************************
// run the train application
// **************************

#define CMD_END '\015'
#define TRAIN_ID "20"

int window_id;

// append src to the tail of dest
// return the dest tail (after appending) for further operations
char* append_string(char* src, char* dest) {
	while (*src != 0) {
		*dest++ = *src++;
	}
	return dest;
}

void comm_train(char* command, char* result, int input_len) {
	COM_Message com_msg;
	com_msg.output_buffer = command;
   	com_msg.len_input_buffer = input_len;
   	com_msg.input_buffer = result;
	send(com_port, &com_msg);
	sleep(5);
}

void change_switch(int switch_id, char switch_fork) {
	char command[5];
	command[0] = 'M';
	command[1] = switch_id + '0';
	command[2] = switch_fork;
	command[3] = CMD_END;
	command[4] = 0;
	comm_train(command, "", 0);
}

void clear_contact() {
	comm_train("R\015", "", 0);
}

// convert an integer (up to 2 digits) into string
// save the string into str
void int_to_str(int integer, char* str) {
	if (integer / 10) {
		*str++ = integer / 10 + '0';
	}
	*str++ = integer % 10;
	*str = 0;
}

int retrieve_contact(char* contact_id) {
	clear_contact();
	char command[3];
	command[0] = 'C';
	char result[3];
	char* tail = append_string(contact_id, command + 1);
	*tail = CMD_END;
	comm_train(command, result, 3);
	return *result == '*' && *(result + 1) == '1';
}

void change_train_speed(int speed) {
	char command[7];
	command[0] = 'L';
	char* tail = append_string(TRAIN_ID, command + 1);
	command[tail++ - command] = 'S';
	command[tail++ - command] = speed + '0';
	command[tail - command] = CMD_END;
	comm_train(command, "", 0);
}

void change_train_direction() {
	char command[7];
	command[0] = 'L';
	char* tail = append_string(TRAIN_ID, command + 1);
	command[tail++ - command] = 'D';
	command[tail - command] = CMD_END;
	comm_train(command, "", 0);
}

void train_process(PROCESS self, PARAM param) {
	window_id = wm_create(10, 2, 50, 20);
	if (retrieve_contact("4") || retrieve_contact("3") || retrieve_contact("6")) {
		wm_print(window_id, "zamboni found\n");
	}
	change_switch(1, 'G');
	change_switch(4, 'G');
	change_switch(5, 'G');
	change_switch(8, 'G');
	change_switch(9, 'R');
	
	int contact2_status = retrieve_contact("2");
	int contact5_status = retrieve_contact("5");
	int contact8_status = retrieve_contact("8");
	int contact11_status = retrieve_contact("11");
	int contact12_status = retrieve_contact("12");
	if (contact8_status && contact11_status) {
		wm_print(window_id, "configuration 1\n");
	} else if (contact2_status && contact12_status) {
		wm_print(window_id, "configuration 2\n");
	} else if (contact2_status && contact11_status) {
		wm_print(window_id, "configuration 3\n");
	} else if (contact5_status && contact12_status) {
		wm_print(window_id, "configuration 4\n");
	}
	change_train_speed(4);
	become_zombie();
}

void init_train() {
	create_process(train_process, 5, 0, "Train Process");
	resign();
}
