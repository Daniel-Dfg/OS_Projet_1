#include "chat_handler.hpp"
#include "exception_handler.hpp"
#include <iostream>
#include <cstring>


ChatHandler::ChatHandler(const std::string &username1_, const std::string &username2_, const bool &bot_, const bool &manual_)
    : user1_name{username1_}, user2_name{username2_}, bot{bot_}, manual{manual_} {
    // Tout initialiser
    DIR *temp = opendir("temp");
    if (temp) {
        ExceptionHandler::return_code_check(closedir(temp));
    } else {
        ExceptionHandler::return_code_check(mkdir("temp", FOLDER_PERMISSION));
    }

    // Initialize FIFO paths
    path_from_user1 = "temp/" + user1_name + "_" + user2_name + ".chat";
    path_from_user2 = "temp/" + user2_name + "_" + user1_name + ".chat";

    // Initialize file descriptors
    file_desc1 = open(path_from_user1.c_str(), O_PATH);
    file_desc2 = open(path_from_user2.c_str(), O_PATH);

    // Create Named Pipes (FIFO)
    if (file_desc1 < 0) {
        ExceptionHandler::return_code_check(mkfifo(path_from_user1.c_str(), FIFO_PERMISSION));
    }
    if (file_desc2 < 0) {
        ExceptionHandler::return_code_check(mkfifo(path_from_user2.c_str(), FIFO_PERMISSION));
    }
    close(file_desc1);
    close(file_desc2);
}

void ChatHandler::send_message_to(const std::string &recipient) {
    std::string path = (recipient == user2_name) ? path_from_user1 : path_from_user2;
    char message_to_send[1024];
    while (true) {
        file_desc1 = open(path.c_str(), O_WRONLY);
        if (fgets(message_to_send, sizeof(message_to_send), stdin) == NULL) {
            if (feof(stdin)) {
                std::cerr << "End of input reached." << std::endl;
                close(file_desc1);
                exit(EXIT_SUCCESS);
            } else if (ferror(stdin)) {
                std::cerr << "Error reading input: " << errno << std::endl;
                close(file_desc1);
                exit(EXIT_FAILURE);
            }
        }
        ssize_t bytes_written = write(file_desc1, message_to_send, strlen(message_to_send));
        if (bytes_written < 0) {
            std::cerr << "Error writing to file: " << strerror(errno) << std::endl;
            close(file_desc1);
            exit(EXIT_FAILURE);
        }

        close(file_desc1);
        close(file_desc1);
    }
}

void ChatHandler::receive_message_from(const std::string &sender) {
    std::string path = (sender == user2_name) ? path_from_user2 : path_from_user1;
    std::string ansi_beginning = "\x1B[4m";
    std::string ansi_end = "\x1B[0m";

    if (bot) {
        ansi_beginning = "";
        ansi_end = "";
    }

    char received_message[1024];
    while (true) {
        file_desc2 = open(path.c_str(), O_RDONLY);
        if (file_desc2 < 0) {
            std::cerr << "Error opening file: " << strerror(errno) << std::endl;
            exit(EXIT_FAILURE);
        }

        ssize_t bytes_read = read(file_desc2, received_message, sizeof(received_message) - 1);
        if (bytes_read < 0) {
            std::cerr << "Error reading from file: " << strerror(errno) << std::endl;
            close(file_desc2);
            exit(EXIT_FAILURE);
        }

        received_message[bytes_read] = '\0'; // Null-terminate the string
        printf("[%s%s%s] %s", ansi_beginning.c_str(), sender.c_str(), ansi_end.c_str(), received_message);
        close(file_desc2);
    }
}
