
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
}

void change_switch(char switch_id, char switch_fork) {
	char command[5];
	command[0] = 'M';
	command[1] = switch_id;
	command[2] = switch_fork;
	command[3] = CMD_END;
	command[4] = 0;
	comm_train(command, "\0", 0);
}

void clear_contact() {
	comm_train("R\015", "\0", 0);
}

int retrieve_contact(char* contact_id) {
	clear_contact();
	char command[5];
	command[0] = 'C';
	char* tail = append_string(contact_id, command + 1);
	*tail++ = CMD_END;
	*tail = 0;
	char result[3];
	comm_train(command, result, 3);
	sleep(5);
	return *result == '*' && *(result + 1) == '1';
}

void change_train_speed(char speed) {
	char command[7];
	command[0] = 'L';
	char* tail = append_string(TRAIN_ID, command + 1);
	*tail++ = 'S';
	*tail++ = speed;
	*tail++ = CMD_END;
	*tail = 0;
	comm_train(command, "\0", 0);
}

void change_train_direction() {
	char command[7];
	command[0] = 'L';
	char* tail = append_string(TRAIN_ID, command + 1);
	*tail++  = 'D';
	*tail++ = CMD_END;
	*tail = 0;
	comm_train(command, "\0", 0);
}

void handle_config1(int is_zamboni) {
	// train 8->9->12
	change_train_speed('4');
	while (retrieve_contact("8")) {}
	change_train_speed('0');
	change_train_direction();
	change_switch('6', 'G');
	change_switch('7', 'G');
	change_train_speed('4');
	while (!retrieve_contact("12")) {}
	change_switch('7', 'R');
	change_train_speed('0');
	change_train_direction();

	// train 12->11 (wagon picked up)
	if (is_zamboni) {
		while (!retrieve_contact("10")) {}
	}
	change_switch('8', 'R');
	change_train_speed('4');
	while (retrieve_contact("11")) {}
	change_train_speed('0');
	change_train_direction();

	// train 11->12
	change_train_speed('4');
	change_switch('7', 'G');
	while (!retrieve_contact("12")) {}
	sleep(15);
	change_switch('8', 'G');
	change_train_speed('0');
	change_train_direction();



	if (is_zamboni) {
		while (!retrieve_contact("6")) {}
	}
	change_switch('5', 'R');
	change_switch('6', 'R');
	change_train_speed('4');
	while (!retrieve_contact("7")) {}
	change_train_speed('0');
	change_train_direction();

	// train 7->8 (back home)
	change_train_speed('4');
	while (!retrieve_contact("8") || retrieve_contact("7")) {}
	wm_print(window_id, "Back home.\n");
	change_train_speed('0');
	while (1) {}
}

void handle_config2(int is_zamboni) {
	// if with zamboni, wait until zamboni 15->3, then start
	if (is_zamboni) {
		while (!retrieve_contact("3")) {}
	}

	// train 12->15
	change_switch('2', 'R');
	change_switch('1', 'R');
	change_train_speed('3');
	while (!retrieve_contact("15")) {}
	change_train_speed('0');
	change_train_direction();
	change_switch('2', 'G');
	change_switch('3', 'G');

	// train 15->1->2 (wagon picked up)
	change_train_speed('4');
	while (!retrieve_contact("1")) {}
	change_switch('1', 'G');
	while (retrieve_contact("1")) {}
	sleep(15);
	change_train_speed('0');
	change_train_direction();

	// train 2->1->15
	change_train_speed('4');
	while (!retrieve_contact("15")) {}
	change_train_speed('0');
	change_train_direction();
	change_switch('1', 'R');
	change_switch('2', 'R');

	// train 15->12 (back home)
	change_train_speed('4');
	while (!retrieve_contact("12")) {}
	change_switch('1', 'G');
	wm_print(window_id, "Back home.\n");
	change_train_speed('0');
	while (1) {}
}

void handle_config3(int is_zamboni) {
	change_train_speed('4');
}

void handle_config4(int is_zamboni) {
	change_train_speed('4');
}

void handle_config(int is_zamboni) {
	if (retrieve_contact("8") && retrieve_contact("11")) {
		wm_print(window_id, "Configuration 1.\n");
		handle_config1(is_zamboni);
	} else if (retrieve_contact("2") && retrieve_contact("12")) {
		wm_print(window_id, "Configuration 2.\n");
		handle_config2(is_zamboni);
	} else if (retrieve_contact("2") && retrieve_contact("11")) {
		wm_print(window_id, "Configuration 3.\n");
		handle_config3(is_zamboni);
	} else if (retrieve_contact("5") && retrieve_contact("12")) {
		wm_print(window_id, "Configuration 4.\n");
		handle_config4(is_zamboni);
	}
}

int detect_zamboni() {
	for (int i = 0; i < 25; ++i) {
		if (retrieve_contact("7")) {
			wm_print(window_id, "Zamboni found.\n");
			return 1;
		}
	}
	return 0;
}

void train_process(PROCESS self, PARAM param) {
	window_id = wm_create(10, 2, 50, 20);
	change_switch('1', 'G');
	change_switch('4', 'G');
	change_switch('5', 'G');
	change_switch('8', 'G');
	change_switch('9', 'R');
	int is_zamboni = detect_zamboni();
	handle_config(is_zamboni);
	become_zombie();
}

void init_train() {
	create_process(train_process, 5, 0, "Train Process");
	resign();
}
