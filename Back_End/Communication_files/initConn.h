#ifndef INITCONN_H
#define INITCONN_H


#define PORT_COMM 13225
#define SERVER_WAIT 10000 //In miliseconds

enum waiting_times {MIN_WAIT = 2000, MAX_WAIT = 5000};
enum connection_status {CLIENT = 0, SERVER = 1, ABORTED = 2, CONN_ERROR = 3};
enum {MAX_PACKET_SIZE = 20}; //In bytes

using namespace std;

typedef int connectionStatus;

bool getHostName(string& hostName);
connectionStatus becomeServer(PolonetConn& conn, string hostName);
connectionStatus becomeClient(PolonetConn& conn, string hostName);


#endif // INITCONN_H
