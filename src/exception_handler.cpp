/* Name            Prénom         ULBID           Matricule
 * El Hussein      Abdalrahman    aelh0063        000596003
 * Ibrahim         Dogan          idog0003        000586544
 * Daniel          Defoing        ddef0003        000589910
*/
#include "exception_handler.hpp"

const unordered_map<ExceptionHandler::ExitCodes, string> ExceptionHandler::EXIT_CODE_MESSAGES = {
    {LACKING_USERNAME, "chat pseudo_utilisateur pseudo_destinataire [--bot] [--manuel]"},
    {USERNAME_TOO_LONG, "Un nom d'utilisateur est trop long (> 30 )"},
    {INVALID_USERNAME, "Un nom d'utilisateur ne peut contenir un caractère interdit ('[', ']', '-', '/'), être ('.', '..'), ou être le même que le nom d'un autre utilisateur"}
    //etc...
};

void ExceptionHandler::display_error_and_exit(const ExitCodes &exit_code){
    auto it = EXIT_CODE_MESSAGES.find(exit_code);

    if(it == EXIT_CODE_MESSAGES.end()){
         cerr << "Erreur inconnue, code " << exit_code << endl;
     }
    else {
        cerr << it->second << endl;
    }
    exit(exit_code);
};

void ExceptionHandler::display_warning(string &&warning_message){
    cout << "AVERTISSEMENT : " << warning_message << endl;
    }

void ExceptionHandler::check_username_validity(vector<string> &usernames){
    for (const string &username : usernames){
        const unordered_set<char> invalid_chars = {'[', ']', '-', '/'};
        const short int username_size_limit = 30;
        const unordered_set<string> invalid_names = {"..", "."};
        if (username.empty()){
            display_error_and_exit(LACKING_USERNAME);
        }
        if(username.size() > username_size_limit){
            display_error_and_exit(USERNAME_TOO_LONG);
        }
        for (char c : username){
            if(invalid_chars.find(c) != invalid_chars.end()){
                display_error_and_exit(INVALID_USERNAME);
            }
        }
        for (string name: invalid_names){
            if(name == username){
                display_error_and_exit(INVALID_USERNAME);
            }
        }
    }
    if(usernames[0] == usernames[1]){
        display_error_and_exit(INVALID_USERNAME);
    }
}

void ExceptionHandler::return_code_check(int error) {
    if (error < 0) {
        cout << "error found, code : " << error << endl;
        exit(error);
    }
}

void ExceptionHandler::process_args(const int argc, char* argv[], bool &bot, bool &manuel) {
    if (argc < 3) {
        display_error_and_exit(LACKING_USERNAME);
    }
    vector<string> usernames = {argv[1], argv[2]};
    check_username_validity(usernames);

    if (argc > 5) {
        display_warning("Trop d'arguments passés (plus de 4)");
    }

    for (int i = 3; i < argc; i++) {
        string arg = argv[i];
        if (arg == "--bot" || arg == "--manuel") {
            if ((bot && arg == "--bot") || (manuel && arg == "--manuel")) {
                display_warning("Un même argument a été écrit deux fois (" + arg + ")");
            } else {
                arg == "--bot" ? bot = true : manuel = true;
            }
        }
        else {
            display_warning("L'argument " + arg + " n'est pas valide : n'utilisez que --bot et/ou --manuel !");
        }
    }
}
