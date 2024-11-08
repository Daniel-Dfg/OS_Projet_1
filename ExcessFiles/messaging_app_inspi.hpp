#include <string>
#include <iostream>
#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <chrono>
#include <ctime>

/*
TODOs :
- Rename functions to be more explicit
- Figure out if some methods should be grouped in a "generic" class (like MessagingUtility for instance)
*/

using std::string, std::cout, std::shared_ptr, std::vector, std::endl, std::unordered_map, std::unordered_set;

class User;
struct Message;
class Conversation;

namespace  IDGeneration{ //TODO : should it be a struct/class ?
    const short unsigned int shift_to_encode = 2;
    const short unsigned int USER = 0b00;
    const short unsigned int CONV = 0b01;
    inline unsigned int conversation_count = 0;
}

inline time_t GetCurrentTime(){ //TODO : place this in a cohesive place
    auto now = std::chrono::system_clock::now();
    return std::chrono::system_clock::to_time_t(now);
}

class User : public std::enable_shared_from_this<User>{
    static unsigned int user_count;
    string name;
    const unsigned int id = (user_count << IDGeneration::shift_to_encode) + IDGeneration::USER;
    unordered_map<unsigned int, shared_ptr<User>> friends;
    unordered_map<unsigned int, shared_ptr<Conversation>> conversations; //design decision : every user has an empty convo with their friends on add.
    //If a friend is deleted and the convo is empty, it is simply deleted too.

    public:
    static inline unsigned int getUserCount(){return user_count;}
    string getName() const {return name;}
    void setName(const string &&newname){name = newname;}
    const unsigned int getId() const {return id;}

    //constructors
    User(const string &name_):name{name_}{user_count ++;}
    //TODO : add other constructors

    //destructor
    ~User(){user_count--;
        //TODO : remove from all convos and friends (go through each friend, delete user form there) !
    }

    void DisplayAllInfos() const; //shows all attrs values
    bool AddFriend(shared_ptr<User> &to_add);
    bool RemoveFriend(shared_ptr<User> &to_remove);
    inline void RemoveFriend(unsigned int &id){this->friends.erase(id);};
    inline bool HasFriend(shared_ptr<User> &to_find) const {return HasFriend(to_find->getId());};
    inline bool HasFriend(unsigned int &&id) const {auto res = this->friends.find(id); return res != this->friends.end();};
    void SendMessageTo(Message &&message, shared_ptr<Conversation> &destination);
    void SendMessageTo(Message &&message, shared_ptr<User> &destination); //will refer to the convo between 2 users
    shared_ptr<Conversation>& FindConversation(shared_ptr<Conversation> &convo) const;
};

struct Message{
    const std::time_t date;
    const string text;
    const shared_ptr<User> &origin;
    Message(string text_, shared_ptr<User> &from, shared_ptr<User> &to):text{text_}, origin{from}, date{GetCurrentTime()} {}
    inline void DisplayMessage() const {cout << origin->getName() << " said on " << date << " : \n" << text << endl;}
};

class Conversation{
    static unsigned int instance_count;
    string name;
    const unsigned int id = (instance_count << IDGeneration::shift_to_encode) + IDGeneration::CONV;
    const std::time_t start_time = GetCurrentTime();
    unordered_set<unsigned int> participants; //marked by their ID only. Use NetworkHandler to get full access only if necessary.
    vector<Message> messages = {};
    inline void displayTitle() const {cout << name << endl << "Started on " << start_time << " (" << messages.size() << "messages)" << endl;}


    public:
    //getters/setters
    inline string getName() const {return name;}
    inline void setName(const string &newname){name = newname;}
    inline const unsigned int getId() const {return id;}
    inline vector<Message>getMessages() const{return messages;}
    inline unordered_set<unsigned int> getParticipants(){return participants;}

    //constructors
    Conversation(const string &name_, unordered_set<shared_ptr<User>> init_participants = {}):
    name{name_}{for (shared_ptr<User> p : init_participants){this->AddParticipant(p);}}

    //destructor
    ~Conversation(){IDGeneration::conversation_count--;
        //to be triggered if no users are in the conversation
    }

    //methods
    bool AddParticipant(shared_ptr<User> to_add);
    inline bool ParticipantIsIn(shared_ptr<User> to_find) const {return this->ParticipantIsIn(to_find->getId());};
    inline bool ParticipantIsIn(unsigned int to_find) const{auto res = this->participants.find(to_find); return res != this->participants.end();}
    inline bool RemoveParticipant(shared_ptr<User> to_remove){return RemoveParticipant(to_remove->getId());};
    bool RemoveParticipant(unsigned int to_remove);

    inline void NewMessage(Message &&message){messages.push_back(message);};
    inline bool IsEmpty() const {return messages.empty();}
    void DisplayFullConvo() const;
    void DisplayLastNMessages(short unsigned int n) const ;

};

class NetworkHandler{
    /*
   Idea : every new entity (User or Convo) should "signal itself" to the Network as it enters it.
   A bit like workers should signal themselves to one single person (like a secretary) before starting their shift.
   Following that idea, forcing every new entity to "present itself" to the NetworkHandler allows to
   dismiss unknown people who present themselves as employees, avoid duplicates, etc.
    */
    unordered_map<int, shared_ptr<User>> all_users;
    unordered_map<int, shared_ptr<Conversation>> convo_id; //convos by IDs (ideal when looking up stuff from User class ?)
    unordered_map<std::unordered_set<int>, shared_ptr<Conversation>> convo_users; //marked by sets of IDs of user inside ? To update on each removal...
     /*
    Goal : add overloads to manage users in one place for higher level
   operations (that don't need uncoupling anyway) (addTo [Convo, map, etc])
   */
   public:
   //TODO !
   shared_ptr<User> &getUserPerID(int id);
   unordered_set<int> getUsersWithName(string name); //set of IDs after lookup
   shared_ptr<Conversation>& GetConvoByID(int id);
   shared_ptr<Conversation>& GetConvoByUsers(unordered_set<shared_ptr<User>> &users);

   bool AddUserToNetwork(shared_ptr<User> &&user){ //Assuming it's being built on the spot
       if (all_users.find(user->getId()) == all_users.end()){
           cout << "Warning : user already exists" << endl;
           return false;
       }
       else {
           all_users[user->getId()] = user;
           return true;
       }
   }

   bool AddConvoToNetwork(shared_ptr<Conversation> convo){
    //probably called on construction of new convo
    //...
   }


};
