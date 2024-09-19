#include "../Header/Header.hpp"

/* 클라이언트에게 받은 메세지를 "[]"오버라이딩을 통해서 해당 커맨드를 찾고 커맨드의 함수포인터를 실행시키는 메서드 */
void ft::Command::operator[](ft::Client &client)
{
    client._message.clear();
    client._message.parseMessage(client._messageIn.erase(client._messageIn.size() - 2));
    if (client._message._command.empty())
    {
        client._messageOut += ERR_NOCOMMANDGIVEN(client);
    }
    else
    {
        std::string upperCommand = ft::tools::ft_toUpper(client._message._command);
        commandIterator it = this->_commands.find(upperCommand);
        std::string inputArguments = "";
        for (std::vector<std::string>::iterator stringIt = client._message._arguments.begin(); stringIt != client._message._arguments.end(); stringIt++)
        {
            inputArguments += *stringIt;
            inputArguments += " ";
        }
        std::cout << "=======================" << std::endl;
        std::cout << "Client : " << client._nickName << std::endl;
        std::cout << "Input Command : " << client._message._command << std::endl;
        std::cout << "Input Arguments : " << inputArguments << std::endl;
        std::cout << "=======================" << std::endl;
        std::cout << "- - - - - - - - - - - -" << std::endl;
        commandIterator funcIt = this->_commands.find(client._message._command);
        if (funcIt == this->_commands.end())
        {
            client._messageOut += ERR_UNKNOWNCOMMAND(client);
        }
        else
        {
            if (client._message._command == "PASS")
            {
                if (client._passFlag)
                {
                    client._messageOut += ERR_REGISTERPASSWORD(client);
                }
                else
                {
                    (this->*it->second)(client); // 해당 명령어가 있다면 그에 해당하는 함수를 호출한다.
                }
            }
            else if (client._message._command == "NICK" || client._message._command == "USER")
            {
                if (!client._passFlag)
                {
                    client._messageOut += ERR_NEEDSERVERPASSWORD(client);
                }
                else
                {
                    (this->*it->second)(client); // 해당 명령어가 있다면 그에 해당하는 함수를 호출한다.
                }
                if (client._nickFlag && client._passFlag && client._userFlag && !client._loginFlag)
                {
                    client._messageOut += REGISTRATION_SUCCESS(client);
                    client._messageOut += RPL_LUSERCLIENT(client);
                    client._messageOut += RPL_LUSEROP(client);
                    client._messageOut += RPL_LUSERCHANNELS(client);
                    client._messageOut += RPL_LUSERME(client);
                    client._messageOut += RPL_MOTDSTART(client);
                    client._messageOut += RPL_MOTD(client, "Enjoy our IRC server");
                    client._messageOut += RPL_MOTD(client, "");
                    client._messageOut += RPL_ENDOFMOTD(client);
                    client._messageOut += RPL_UMODEIS(client);
                    std::cout << "Client '"
                              << "\033[32m" << client._nickName << "\033[0m"
                              << "' "
                              << client._hostAddr << " connected" << std::endl;
                    client._loginFlag = true;
                }
            }
            else if (client._loginFlag)
            {
                (this->*it->second)(client); // 해당 명령어가 있다면 그에 해당하는 함수를 호출한다.
            }
            else
            {
                client._messageOut += "You need to register info (1. [PASS] -> 2. [NICK] -> 3. [USER])\r\n";
            }
        }
    }
    client._message.clear();
}

/* "Command"생성자, "_command"맵을 초기화 하고 해당 명령어를 처리하는 함수 포인터를 매핑한다. 즉, 해당 명령어를 처리하는 함수 포인터를 초기화하는 생성자이다.*/
ft::Command::Command(void)
{
    this->_commands["AWAY"] = &Command::AWAY;
    this->_commands["DOWN"] = &Command::DOWN;
    this->_commands["INVITE"] = &Command::INVITE;
    this->_commands["JOIN"] = &Command::JOIN;
    this->_commands["KICK"] = &Command::KICK;
    this->_commands["LIST"] = &Command::LIST;
    this->_commands["MODE"] = &Command::MODE;
    this->_commands["NAMES"] = &Command::NAMES;
    this->_commands["NICK"] = &Command::NICK;
    this->_commands["NOTICE"] = &Command::NOTICE;
    this->_commands["OPER"] = &Command::OPER;
    this->_commands["PART"] = &Command::PART;
    this->_commands["PASS"] = &Command::PASS;
    this->_commands["PRIVMSG"] = &Command::PRIVMSG;
    this->_commands["QUIT"] = &Command::QUIT;
    this->_commands["RESTART"] = &Command::RESTART;
    this->_commands["TOPIC"] = &Command::TOPIC;
    this->_commands["USER"] = &Command::USER;
    this->_commands["WHO"] = &Command::WHO;
    this->_commands["WHOIS"] = &Command::WHOIS;
    this->_commands["BOT"] = &Command::BOT;
    this->_commands["PONG"] = &Command::PONG;
    this->_commands["WALLOPS"] = &Command::WALLOPS;
}