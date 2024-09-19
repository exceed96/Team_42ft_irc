#include "Header.hpp"

#define THROW_EXCEPT(str) throw std::runtime_error(str)

#define ERROR_ARGUS THROW_EXCEPT("ERROR!! ./ircserv <port> <password>")
#define ERROR_PORTNUMBER THROW_EXCEPT("Port number must be between 1025 ~ 65535")
#define ERROR_PASSWORD THROW_EXCEPT("<password> shall not be empty")
#define ERRCODE_EXCEPT(code) THROW_EXCEPT(std::strerror(code))
#define ERRNO_EXCEPT ERRCODE_EXCEPT(errno)
#define NUMERIC_REPLY(num, nick) \
    COLON + ft::Server::_hostAddr + SPACE + num + SPACE + nick + SPCL
#define NUMERIC_REPLY_NOCL(num, nick) \
    COLON + ft::Server::_hostAddr + SPACE + num + SPACE + nick + SPACE

#define RPL_BASE(client_, base_) COLON + client_._nickName + "!" + client_._userName + "@" + client_._hostAddr + SPACE + \
                                     base_ + SPACE
#define ERR_REPLY_BASE(num, client_) \
    NUMERIC_REPLY(num, client_._nickName) + client_._message._command + SPCL

#define RPL_WELCOME(client_)                                                             \
    NUMERIC_REPLY("001", client_._nickName) + "Welcome to the Internet Relay Network " + \
        RPL_BASE(client_, "") + CRLF
#define RPL_YOURHOST(client_)                                                       \
    NUMERIC_REPLY("002", client_._nickName) + "Your host is " + client_._hostAddr + \
        ", running version 1.0 " + CRLF
#define RPL_CREATED(client_)                                               \
    NUMERIC_REPLY("003", client_._nickName) + "This server was created " + \
        ft::Server::_createServerTime + " UTC+1 " + CRLF
#define RPL_MYINFO(client_)                                                             \
    NUMERIC_REPLY("004", client_._nickName) + ft::Server::_hostAddr + " v1.0" + SPACE + \
        CLIENTMODE_VAILABLE + SPACE + CHANNELMODE_VAILABLE + CRLF
#define RPL_LUSERCLIENT(client_)                                                            \
    NUMERIC_REPLY("251", client_._nickName) + "There are " +                                \
        ft::Server::_server->getClientsCount() + " users and " +                            \
        ft::Server::_server->getChannelsCount() + " channels on " + ft::Server::_hostAddr + \
        " Max: " + MAXUSERS + CRLF
#define RPL_LUSEROP(client_)                                                        \
    NUMERIC_REPLY("252", client_._nickName) + ft::Server::_server->getOperCount() + \
        " :operator(s) online" + CRLF
#define RPL_LUSERCHANNELS(client_)                                                      \
    NUMERIC_REPLY("254", client_._nickName) + ft::Server::_server->getChannelsCount() + \
        " :channels formed" + CRLF
#define RPL_LUSERME(client_)                              \
    NUMERIC_REPLY("255", client_._nickName) + "I have " + \
        ft::Server::_server->getClientsCount() + " clients and 0 servers" + CRLF
#define RPL_MOTDSTART(client_)                                                              \
    NUMERIC_REPLY("375", client_._nickName) + "-" + SPACE + ft::Server::_hostAddr + SPACE + \
        "Message of the day - " + CRLF
#define RPL_MOTD(client_, motd_) NUMERIC_REPLY("372", client_._nickName) + SPACE + motd_ + CRLF
#define RPL_ENDOFMOTD(client_) \
    NUMERIC_REPLY("376", client_._nickName) + "End of MOTD command" + CRLF
#define RPL_UMODEIS(client_) \
    NUMERIC_REPLY_NOCL("221", client_._nickName) + client_.getClientModes() + CRLF

#define RPL_QUIT(client_) RPL_BASE(client_, "QUIT") + COLON + client_._messageAway + CRLF

#define ERR_CHANNELISFULL(client_, channel_name) \
    ERR_REPLY_BASE("471", client_) + channel_name + " :Cannot join channel (+l)" + CRLF
#define ERR_BADCHANNELKEY(client_, channel_name) \
    ERR_REPLY_BASE("475", client_) + channel_name + " :Cannot join channel (+k)" + CRLF
#define ERR_INVITEONLYCHAN(client_, channel_name) \
    ERR_REPLY_BASE("473", client_) + channel_name + " :Cannot join channel (+i)" + CRLF
#define ERR_NOCOMMANDGIVEN(client_) ERR_REPLY_BASE("421", client_) + "No command given" + CRLF
#define ERR_UNKNOWNCOMMAND(client_) ERR_REPLY_BASE("421", client_) + "Unknown command" + CRLF
#define RPL_CLOSINGLINK(client_) \
    "ERROR :Closing Link: " + client_._hostAddr + " (Quit: " + client_._messageAway + ")" + CRLF
#define NOTICE_MSG(client_, target_, message_) \
    RPL_BASE(client_, "NOTICE") + target_ + SPCL + message_ + CRLF

#define ERR_NEEDMOREPARAMS(client_) \
    ERR_REPLY_BASE("461", client_) + "Not enough parameters" + CRLF
#define ERR_ALREADYREGISTRED(client_) \
    ERR_REPLY_BASE("462", client_) + "Unauthorized command (already registered)" + CRLF
#define ERR_PASSWDMISMATCH(client_) ERR_REPLY_BASE("464", client_) + "Password incorrect" + CRLF
#define ERR_RESTRICTED(client_) \
    ERR_REPLY_BASE("484", client_) + "Your connection is restricted!" + CRLF
#define ERR_NONICKNAMEGIVEN(client_) ERR_REPLY_BASE("431", client_) + "No nickname given" + CRLF
#define ERR_ERRONEUSNICKNAME(client_) \
    ERR_REPLY_BASE("432", client_) + "Erroneous nickname" + CRLF
#define ERR_NICKNAMEINUSE(client_) \
    ERR_REPLY_BASE("433", client_) + "Nickname is already in use" + CRLF
#define ERR_NICKCOLLISION(client_) ERR_REPLY_BASE("436", client_) + "Nickname collision" + CRLF
#define ERR_UNAVAILRESOURCE(client_, time_left)                   \
    ERR_REPLY_BASE("437", client_) + "Please wait " + time_left + \
        " seconds before attempting to change your nickname again." + CRLF
#define RPL_NICKNAME(client_, new_nick_) RPL_BASE(client_, "NICK") + new_nick_ + CRLF
#define REGISTRATION_SUCCESS(client_)                                     \
    RPL_WELCOME(client_) + RPL_YOURHOST(client_) + RPL_CREATED(client_) + \
        RPL_MYINFO(client_)
#define ERR_NOSUCHNICK(client_, nickname_) \
    ERR_REPLY_BASE("401", client_) + nickname_ + " :No such nick/channel" + CRLF
#define RPL_WHOISUSER(client_, target_)                                         \
    NUMERIC_REPLY_NOCL("311", client_._nickName) + target_->_nickName + SPACE + \
        target_->_userName + SPACE + target_->_hostAddr + SPACE + "*" + SPCL +  \
        target_->_realName + CRLF
#define RPL_WHOISSERVER(client_, target_)                                       \
    NUMERIC_REPLY_NOCL("312", client_._nickName) + target_->_nickName + SPACE + \
        ft::Server::_hostAddr + SPCL + "ircserv" + CRLF
#define RPL_WHOISCHANNELS(client_, target_)                                    \
    NUMERIC_REPLY_NOCL("319", client_._nickName) + target_->_nickName + SPCL + \
        target_->getChannels(' ') + CRLF
#define RPL_AWAY(client_, target_nick_, away_msg_) \
    NUMERIC_REPLY_NOCL("301", client_._nickName) + target_nick_ + SPCL + away_msg_ + CRLF
#define RPL_WHOISOPERATOR(client_, target_)                                                      \
    NUMERIC_REPLY_NOCL("313", client_._nickName) + target_->_nickName + " :is an IRC operator" + \
        CRLF
#define RPL_WHOISIDLE(client_, target_)                                         \
    NUMERIC_REPLY_NOCL("317", client_._nickName) + target_->_nickName + SPACE + \
        target_->getIdleTime() + SPACE + target_->getJoinServerTime() + SPCL +  \
        "seconds idle, signon time" + CRLF
#define RPL_ENDOFWHOIS(client_, target_)                                                        \
    NUMERIC_REPLY_NOCL("318", client_._nickName) + target_->_nickName + " :End of WHOIS list" + \
        CRLF

#define ERR_TOOMANYMATCHES(client_) ERR_REPLY_BASE("416", client_) + " :Too many matches" + CRLF
#define RPL_LISTEND(client_) NUMERIC_REPLY_NOCL("323", client_._nickName) + "End of LIST" + CRLF
#define ERR_NOSUCHCHANNEL(client_, channel_name) \
    ERR_REPLY_BASE("403", client_) + channel_name + " :No such channel" + CRLF
#define RPL_LIST(client_, channel_name, users, topic)                                     \
    NUMERIC_REPLY_NOCL("322", client_._nickName) + channel_name + SPACE + users + SPACE + \
        topic + CRLF
#define ERR_USERONCHANNEL(client_, channel_name)                                       \
    NUMERIC_REPLY("443", client_._nickName) + client_._nickName + " " + channel_name + \
        " :is already on channel" + CRLF
#define ERR_CHANNELISFULL(client_, channel_name) \
    ERR_REPLY_BASE("471", client_) + channel_name + " :Cannot join channel (+l)" + CRLF
#define ERR_BADCHANNELKEY(client_, channel_name) \
    ERR_REPLY_BASE("475", client_) + channel_name + " :Cannot join channel (+k)" + CRLF

#define RPL_JOIN(newMember_, channel_name) \
    RPL_BASE(newMember_, "JOIN") + COLON + channel_name + CRLF
#define RPL_TOPIC(client_, channel_name, topic_) \
    NUMERIC_REPLY_NOCL("332", client_._nickName) + channel_name + SPCL + topic_ + CRLF
#define RPL_NAMREPLY(client_, channel_name, users_)                                             \
    NUMERIC_REPLY_NOCL("353", client_._nickName) + "=" + SPACE + channel_name + SPCL + users_ + \
        CRLF
#define RPL_ENDOFNAMES(client_, channel_name) \
    NUMERIC_REPLY_NOCL("366", client_._nickName) + channel_name + " :End of NAMES list" + CRLF
#define RPL_UNAWAY(client_) \
    NUMERIC_REPLY_NOCL("305", client_._nickName) + "You are no longer marked as being away" + CRLF
#define RPL_NOWAWAY(client_) \
    NUMERIC_REPLY_NOCL("306", client_._nickName) + "You have been marked as being away" + CRLF
#define ERR_NORECIPIENT(client_) ERR_REPLY_BASE("411", client_) + "No recipient given" + CRLF
#define ERR_NOTEXTTOSEND(client_) ERR_REPLY_BASE("412", client_) + "No text to send" + CRLF
#define ERR_CHANNOEXT(client_, channel_name)                                                \
    ERR_REPLY_BASE("404", client_) + channel_name +                                         \
        " :Cannot send external messages to this channel whilst the +n (noextmsg) mode is " \
        "set" +                                                                             \
        CRLF
#define ERR_CANNOTSENDTOCHAN(client_, channel_name)                                            \
    ERR_REPLY_BASE("404", client_) + channel_name +                                            \
        " :Cannot send messages to channel whilst the +m (moderated) mode without +v (voice) " \
        "mode" +                                                                               \
        CRLF
#define MSG(client_, target_, message_) \
    RPL_BASE(client_, "PRIVMSG") + target_ + SPCL + message_ + CRLF
#define ERR_NOORIGIN(client_) ERR_REPLY_BASE("409", client_) + "No origin specified" + CRLF
#define ERR_NOSUCHSERVER(client_, server_) \
    ERR_REPLY_BASE("402", client_) + server_ + SPCL + "No such server" + CRLF
#define RPL_YOUREOPER(client_) \
    NUMERIC_REPLY_NOCL("381", client_._nickName) + "You are now an IRC operator" + CRLF
#define ERR_NOTONCHANNEL(client_, channel_name) \
    ERR_REPLY_BASE("442", client_) + channel_name + " :You're not on that channel" + CRLF
#define ERR_CHANOPRIVSNEEDED(client_, channel_name) \
    ERR_REPLY_BASE("482", client_) + channel_name + " :You're not channel operator" + CRLF
#define RPL_INVITATION(client_, channel_name, target_nick_) \
    RPL_BASE(client_, "INVITE") + target_nick_ + " :" + channel_name + CRLF
#define RPL_INVITING(client_, channel_name, target_nick_) \
    NUMERIC_REPLY_NOCL("341", client_._nickName) + target_nick_ + SPACE + channel_name + CRLF
#define RPL_DIE(client_) \
    NUMERIC_REPLY_NOCL("383", client_._nickName) + "ircserv" + SPCL + "Server terminating" + CRLF
#define ERR_NOPRIVILEGES(client_) \
    ERR_REPLY_BASE("481", client_) + "Permission Denied- You're not an IRC operator" + CRLF
#define ERR_USERNOTINCHANNEL(client_, absent_, channel_name)        \
    ERR_REPLY_BASE("441", client_) + absent_ + " " + channel_name + \
        " :They aren't on that channel" + CRLF
#define RPL_PART(client_, channel_name) RPL_BASE(client_, "PART") + channel_name + CRLF
#define RPL_PARTMSG(client_, channel_name, part_msg_) \
    RPL_BASE(client_, "PART") + channel_name + SPCL + part_msg_ + CRLF
#define REPLY_CHANGEADMINISTRATOR(client, channelName)                                    \
    NUMERIC_REPLY_NOCL("240", client->_nickName) + "You became the administrator of [ " + \
        channelName + " ] channel" + "\r\n"
#define RPL_RESTART(client_) \
    NUMERIC_REPLY_NOCL("382", client_._nickName) + "ircserv" + SPCL + "Restarting server" + CRLF
#define ERR_TOOMANYMATCHESNAMES(client_, channel_name_) \
    ERR_REPLY_BASE("416", client_) + channel_name_ + " :Too many matches" + CRLF
#define RPL_NAMREPLY(client_, channel_name, users_)                                             \
    NUMERIC_REPLY_NOCL("353", client_._nickName) + "=" + SPACE + channel_name + SPCL + users_ + \
        CRLF

#define RPL_CHANNELMODEIS(client_, channel_name, channel_modes) \
    NUMERIC_REPLY_NOCL("324", client_._nickName) + channel_name + SPACE + channel_modes + CRLF
#define RPL_CHANMODE(client_, channel_name_, channel_modes_) \
    RPL_BASE(client_, "MODE") + channel_name_ + SPACE + channel_modes_ + CRLF
#define ERR_USERSDONTMATCH(client_) \
    ERR_REPLY_BASE("502", client_) + "Can't change mode for other users" + CRLF
#define RPL_UMODE(client_) \
    RPL_BASE(client_, "MODE") + client_._nickName + SPCL + client_.getClientModes() + CRLF
#define ERR_KEYSET(client_, channel_name) \
    ERR_REPLY_BASE("467", client_) + channel_name + " :Channel key already set" + CRLF
#define ERR_MODELIMITNEEDMOREPARAMS(client_) \
    NUMERIC_REPLY_NOCL("461", client_._nickName) + "MODE +l :Not enough parameters" + CRLF
#define ERR_UNKNOWNMODE(client_, mode_) \
    NUMERIC_REPLY_NOCL("472", client_._nickName) + mode_ + " :is unknown mode char to me" + CRLF
#define RPL_NOTOPIC(client_, channel_name) \
    NUMERIC_REPLY_NOCL("331", client_._nickName) + channel_name + " :No topic is set" + CRLF
#define RPL_TOPIC_CHANGE(client_, channel_name, topic_) \
    RPL_BASE(client_, "TOPIC") + channel_name + SPCL + topic_ + CRLF
#define NOTICE_MSG(client_, target_, message_) \
    RPL_BASE(client_, "NOTICE") + target_ + SPCL + message_ + CRLF
#define RPL_BOT(client_, reply_) NUMERIC_REPLY_NOCL("335", client_._nickName) + reply_ + CRLF
#define ERR_NOTOPICMODE(client, channelName) NUMERIC_REPLY_NOCL("440", client._nickName) + channelName + " : This channel mode is not TOPIC " + CRLF
#define ERR_REGISTERPASSWORD(client) NUMERIC_REPLY_NOCL("480", client._nickName) + "Already register Server password" + CRLF
#define ERR_NEEDSERVERPASSWORD(client) NUMERIC_REPLY_NOCL("484", client._nickName) + "Need to register Server password" + "\r\n"
#define RPL_WHOREPLY_PTR(client_, channel_ptr_, other_ptr_)                                     \
    NUMERIC_REPLY_NOCL("352", client_._nickName) + channel_ptr_->_name + SPACE +                \
        other_ptr_->_userName + SPACE + other_ptr_->_hostAddr + SPACE + ft::Server::_hostAddr + \
        SPACE + other_ptr_->_nickName + SPACE + other_ptr_->who(*channel_ptr_) + SPCL + '0' +   \
        SPACE + other_ptr_->_realName + CRLF
#define RPL_ENDOFWHO(client_, name_) \
    NUMERIC_REPLY_NOCL("315", client_._nickName) + name_ + SPCL + "End of WHO list" + CRLF
#define RPL_WHOREPLYNAME(client_, channel_name_, other_)                                      \
    NUMERIC_REPLY_NOCL("352", client_._nickName) + channel_name_ + SPACE + other_._userName + \
        SPACE + other_._hostAddr + SPACE + ft::Server::_hostAddr + SPACE + other_._nickName + \
        SPACE + other_.who(channel_name_) + SPCL + '0' + SPACE + other_._realName + CRLF

#define ERR_NEEDOPERATORMODE(client) NUMERIC_REPLY_NOCL("434", client._nickName) + "Need to channel Operator Mode ( +o )" + CRLF
#define ERR_PRIVATEMODE(client) NUMERIC_REPLY_NOCL("435", client._nickName) + "This channel mode belongs to 'p'" + CRLF
#define ERR_INVITEMODE(client) NUMERIC_REPLY_NOCL("438", client._nickName) + "This channel mode belongs to 'i'" + CRLF
#define ERR_NICKMISMATCH(client) NUMERIC_REPLY_NOCL("439", client._nickName) + "OPER command need your own nickName" + CRLF
#define ERR_OPERATORMODE(client) NUMERIC_REPLY_NOCL("438", client._nickName) + "Client mode 'o' need command [ OPER ]" + CRLF