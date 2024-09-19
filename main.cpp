#include "Header/Header.hpp"

int main(int ac, char **ag)
{
    try
    {
        ft::Server irc(ac, ag);   //"Server"클래스를 이용해서 객체를 만들어주고 입력한 인자를 토대로 서버의 틀을 잡는다.
        irc.createServerSocket(); // 서버와 서버에서 생성하는 소켓을 생성해준다.
        irc.activatingServer();   // 생성한 서버를 실행해준.
        return (EXIT_SUCCESS);
    }
    catch (const std::exception &e) // try문에서 에러가 발생하면 에러를 출력해준다.
    {
        std::cerr << e.what() << '\n';
        return (EXIT_FAILURE);
    }
}