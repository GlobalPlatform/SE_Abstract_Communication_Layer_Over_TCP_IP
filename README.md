# SE Abstraction Communication Layer

This project provides an abstract communication layer between test tools and secure elements
in case the traditional, physical communication interfaces are not available.

## Project Structure

* `client` and `server` folders: Eclipse C++ projects for the core libraries
* `vs` folder: Visual Studio C++ solution and project for the core libraries
* `clientGUI` and `serverGUI` folders: Visual Studio C# projects for the user interface

## Protocol

Messages are exchanged over a TCP socket between the Secure Element (client) and the
Test Tool (server).

### Message Format

Each message consists of a length and a value field. 
The length value is a 32-bit integer (big-endian).
The body content depends on the message.

#### Client Name Message

The first message sent by the client is its name. 
The body consists of the client name as an ASCII string.

#### Command Message

The test tool (server) sends command messages to the client. 
These messages have an ASCII encoded body containing a JSON object.

| JSON property | Value                                                                |
| ------------- | -------------------------------------------------------------------- |
| data          | The command data as a hexadecimal string (optional).                 |
| request       | An integer identifying the request type (See *Request Types* table). |
| timeout       | The maximum allowed time for executing this command in milliseconds. |

##### Request Types

| Value | Name                | Description                                            |
| ----- | ------------------- | ------------------------------------------------------ |
| 0     | REQ_CONNECT         | *Currently not in use*                                 |
| 1     | REQ_DIAG            | Get diagnostic information from the SE.                |
| 2     | REQ_DISCONNECT      | The client shall disconnect. No response expected.     |
| 3     | REQ_ECHO            | Echo the command data.                                 |
| 4     | REQ_INIT            | *Currently not in use*                                 |
| 5     | REQ_RESTART         | *Currently not in use*                                 |
| 6     | REQ_COMMAND         | Send an ISO-7816 command APDU.                         |
| 7     | REQ_COMMAND_A       | Send a type A command.                                 |
| 8     | REQ_COMMAND_B       | Send a type B command.                                 |
| 9     | REQ_COMMAND_F       | Send a type F command.                                 |
| 10    | REQ_COLD_RESET      | Perform a cold reset of the SE.                        |
| 11    | REQ_WARM_RESET      | Perform a warm reset of the SE.                        |
| 12    | REQ_POWER_OFF_FIELD | Power off the CLF.                                     |
| 13    | REQ_POWER_ON_FIELD  | Power on the CLF.                                      |

##### Examples

A request for a cold reset, with a timeout of 30 seconds:

````json
{"data":"","request":10,"timeout":30000}
````
A request to send a SELECT MF command APDU, with a timeout of 5 seconds:

````json
{"data":"00A40004023F00","request":6,"timeout":5000}
````

#### Response message

The secure element (client) sends response messages when receiving a command message from the server.
Like the command messages, they have an ASCII encoded JSON body.

| JSON property          | Value                                                                                    |
| ---------------------- | ---------------------------------------------------------------------------------------- |
| response               | The response data (See *Response Data* table).                                           |
| err_server_code        | An integer identifying error or success on the server layer (See *Error Codes* table).   |
| err_server_description | A string describing the error on the server layer, or "OK" in case of success.           |
| err_client_code        | An integer identifying error or success on the client layer (See *Error Codes* table).   |
| client_description     | A string describing the error on the client layer, or "OK" in case of success.           |
| err_terminal_code      | An integer identifying error or success on the terminal layer (See *Error Codes* table). |
| terminal_description   | A string describing the error on the terminal layer, or "OK" in case of success.         |
| err_card_code          | An integer identifying error or success on the card layer (See *Error Codes* table).     |
| err_card_description   | A string describing the error on the card layer, or "OK" in case of success.             |

The properties `err_server_code` and `err_server_description` are always set to `0` and `"OK"` when transmitted
by the client. When working with response message objects, the server may internally set these properties to 
handle internal server errors.

#### Response Data

| Request             | Data                                        |
| ------------------- | ------------------------------------------- |
| REQ_CONNECT         | N/A                                         |
| REQ_DIAG            | A string containing diagnostic information. |
| REQ_DISCONNECT      | N/A                                         |
| REQ_ECHO            | The data from the command.                  |
| REQ_INIT            | N/A                                         |
| REQ_RESTART         | N/A                                         |
| REQ_COMMAND         | The response APDU as a hexadecimal string.  |
| REQ_COMMAND_A       | The response as a hexadecimal string.       |
| REQ_COMMAND_B       | The response as a hexadecimal string.       |
| REQ_COMMAND_F       | The response as a hexadecimal string.       |
| REQ_COLD_RESET      | The ATR as a hexadecimal string.            |
| REQ_WARM_RESET      | The ATR as a hexadecimal string.            |
| REQ_POWER_OFF_FIELD | N/A                                         |
| REQ_POWER_ON_FIELD  | N/A                                         |

##### Error Codes

| Value | Name                 | Description                                              |
| ----- | -------------------- | -------------------------------------------------------- |
| 0     | SUCCESS              | The command was successfully executed.                   |
| -1    | ERR_TIMEOUT          | The command could not be executed within the given time. |
| -2    | ERR_NETWORK          | A network error occurred.                                |
| -3    | ERR_CLIENT_CLOSED    | The client disconnected.                                 |
| -4    | ERR_INVALID_STATE    | The command cannot currently be executed.                |
| -5    | ERR_INVALID_REQUEST  | The command was not understood by the client.            |
| -6    | ERR_JSON_PARSING     | The command (or response) could not be parsed.           |
| -7    | ERR_INVALID_TERMINAL | The terminal is not available.                           |

##### Examples

A successful response to a request for a cold reset:

```json
{"client_description":"OK","err_card_code":0,"err_card_description":"OK","err_client_code":0,"err_server_code":0,"err_server_description":"OK","err_terminal_code":0,"response":"3B 9F 96 80 3F C7 82 80 31 E0 73 F6 21 57 57 4A 33 05 81 60 61 00 FA","terminal_description":"OK"}
```