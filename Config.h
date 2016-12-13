/*
  ----------------------------------------------
  SYSTEM SETTINGS
  ----------------------------------------------
 */
#define SD_CHIP_SELECT 4
#define BUZZER_OUTPUT 19
#define TICKET_GATE_OUTPUT 18
#define TICKET_GATE_INPUT 17

#define TIME_AMPULHETA 125
#define TIME_WAITING_CARDS 500
#define TIME_SHOW_NOT_AUTHORIZED_MESSAGE 500

#define PAGE_CONFIG "page.txt"

#define LABEL_ENTER_DATA  "Digite a matricula"
#define LABEL_DATA_INVALID  "Matricula invalida"
#define LABEL_OPENING_MESSAGE "Boa refeicao"
#define LABEL_NOT_AUTHORIZED_MESSAGE  "Nao autorizado"

#define VERTICAL_LABEL_POSITION 16
#define CLEAR_STRING ""

#define BUFFER_SIZE_LABEL 20
#define BUFFER_URL 100

/*
  ----------------------------------------------
  AUTHENTICATION
  ----------------------------------------------
 */
/*
 * Before compiling the code, comment the #define MySettings.h
 * line and uncomment the later by filling in the Oauth server
 * credentials information
 */
#include "MySettings.h"
//#define OAUTH_PARAMS "username=my_user_email&password=my_user_password&client_id=my_oauth_id&client_secret=my_oauth_secret&grant_type=password"
#define HOST "guitter.com.br"
#define PORT 80
#define ROUTE_AUTENTICATION "/api/autorizacao"
#define ROUTE_REGISTRATION "/api/autorizacao"
#define PARAM_ROUTE_AUTENTICATION "card="
#define PARAM_ROUTE_REGISTRATION "matricula="
#define MINIMUM_SIZE_REGISTRATION  13
#define MAXIMO_SIZE_REGISTRATION  14
#define PARAM_PARSE_NAME "name"
#define PARAM_PARSE_AUTH "autorizacao"
#define OAUTH_ROUTE "/oauth/access_token"


/*
 *
 */

