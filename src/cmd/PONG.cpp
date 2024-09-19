#include "../../Header/Header.hpp"

/* "PONG"명령어 실행 메서드 */
void ft::Command::PONG(ft::Client &client)
{
    if (client._message._arguments.empty())
    {
        client._messageOut += ERR_NOORIGIN(client);
    }
    else if (client._message._arguments.front().compare(ft::Server::_hostAddr))
    {
        client._messageOut += ERR_NOSUCHSERVER(client, client._message._arguments.front());
    }
}