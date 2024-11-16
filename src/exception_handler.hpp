#ifndef EXCEPTION_HANDLER_HPP
#define EXCEPTION_HANDLER_HPP

#include <string>
#include <unordered_map>
#include <unordered_set>
#include <iostream>
#include <vector>

using std::string, std::unordered_map, std::unordered_set, std::vector, std::cerr, std::endl, std::cout;


class ExceptionHandler {
    enum ExitCodes {
        VALID = 0,
        LACKING_USERNAME = 1,
        USERNAME_TOO_LONG = 2,
        INVALID_USERNAME = 3, //Si l'utilisateur s'appelle '.', '..', etc ou que les deux noms sont les mêmes
    };

    //On va probablement devoir raffiner ces exceptions, en disant par exemple lequel des deux usernames présentés est fautif.
    static const unordered_map<ExitCodes, string> EXIT_CODE_MESSAGES;

    static void check_username_validity(vector<string> &usernames);
    static void display_error_and_exit(const ExitCodes &exit_code);
    static void display_warning(string &&warning_message);
    public:
    static void process_args(const int argc, char* argv[], bool &bot, bool &manual);
    static void return_code_check(int error);
};

#endif // EXCEPTION_HANDLER_HPP
