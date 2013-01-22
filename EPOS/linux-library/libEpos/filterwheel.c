/* 

control ATOM filter wheel

*/

#include <string.h>
#include <ctype.h>
#include "epos.h"



#define ZERO_OFFSET 452700    //< steps between reference point and 1st filter
#define FILTER_FILTER 666667  //< steps between 2 filters


void help(char *arg) {
  printf("\n\n   ### filter wheel control ###\n\n");
  printf("usage: %s <devicename>\n", arg);
  printf("valid names are full path to serial device (RS232) and 'IP:tcp port' to connect via rs232 over TCP/IP\n\n");
  printf("example: '%s /dev/ttyS0'\n\n", arg);
  printf("         '%s 192.168.1.100:2572' (serial port A on main camera)\n\n", arg);
  exit(0);
}





int printHelp() {

  printf("\n\n ** filter wheel control ** \n\n");

  printf(" -enter filter position [0..5] to move filter wheel\n");
  printf(" -enter 'r' to search for reference point\n");
  printf(" -enter 'h' to read this help\n");
  printf(" -enter 'q' to quit\n");

  return(0);
}


/*
*********************** 
*** main program *** 
***********************
*/


int main(int argc, char **arg){

  
  char c;
  int  n;
  long int pos;
  char ip[16];
  unsigned short port;
  size_t string_pos;

  if (argc != 2) {
    help( arg[0] );
  }
 
  
#ifdef DEBUG
  printf("\n++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n");
  printf("de-bugging mode enabled -- will echo all EPOS communication.\n");
  printf("lines starting with '>>' is data send to EPOS,\n");
  printf("lines starting with '<<' is data received from EPOS.\n");
  printf("Intermediate 'ACK' are NOT shown!\n\n");
  printf("CAUTION: this will mess-up all formated output!\n");
  printf("++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n");
#endif
  

  
  /* open EPOS device */

  // check wether connection is made via TCP/IP or ser. device

  // is argument starting with a '/'?
  if ( ! strncmp(arg[1], "/", 1) ) {
    printf("trying to open '%s'...", arg[1]);
    if ( openEPOS(arg[1]) < 0) exit(-1);
    printf("done.\n");
  }
  // is argument starting with a digit? => assume IP adress!
  else if ( isdigit( *arg[1] )  ) {
    //    printf("%s\n", arg[1]);
    string_pos = strspn(arg[1], "1234567890.");
    if ( string_pos == strlen(arg[1])  ) {
      fprintf(stderr, "TCP/IP PORT NOT GIVEN!\n");
      return(1);
    }
    //    fprintf(stderr,">>>%d<<<\n", string_pos);
    strncpy(ip, arg[1], string_pos);
    ip[string_pos] = '\0';
    //printf("%s; %d  -- >%s<\n", ip, string_pos, strpbrk(arg[1],":"));
    
    if ( sscanf( strpbrk(arg[1],":"), ":%hu", &port) == EOF) {
      fprintf(stderr, "TCP/IP PORT NOT GIVEN!\n");
      return(1);
    }
    printf("trying to open %s, port %hu...", ip, port);
    if (openTCPEPOS(ip, port) < 0) exit(-1);
    printf("done.\n");
  }

  else {
    fprintf(stderr, "INVALID ARGUMET!\n");
    help(arg[0]);
  }
    

  
  /******************************************************
            switch on  
  ******************************************************/

  printEPOSstate();
  
  
  // 1. check if we are in FAULT state
  n = checkEPOSstate();
  if ( n == 11) {
    printf("EPOS is in FAULT state, doing FAULT RESET... ");
    // should check which fault this is, assume it's a CAN error from
    // power-on
       
    
    // 1a. reset FAULT
    changeEPOSstate(6);
    
    
    // 1b. check status
    if ( checkEPOSstate() == 11 ){
      fprintf(stderr, "\nEPOS still in FAULT state, quit!\n");
      exit(1);
    }
    else printf("success!\n"); // now in state 'switch on disabled'
  }
  
  else {
    printf("EPOS in state %d, issuing 'disable voltage'\n", n);
    changeEPOSstate(2); // now in state 'switch on disabled'
  }

  
  // 2. we should now be in 'switch on disabled' (2)
  n = checkEPOSstate();
  if ( n != 2 ){
    printf("EPOS is NOT in 'switch on disabled' state, but in %d -> quit!\n", n);
    printf("(%s(), %s line %d)\n", __func__, __FILE__, __LINE__);
    exit(1);
  }
  // issue a 'shutdown'
  changeEPOSstate(0);  // should now be in state 'ready to switch on (3)'
    

  // 3. switch on
  n = checkEPOSstate();
  if ( n != 3 ){
    printf("EPOS is NOT in 'ready to switch on' state, but in %d -> quit!\n", n);
    printf("(%s(), %s line %d)\n", __func__, __FILE__, __LINE__);
    exit(1);
  }
  printf("\nswitching on... ");
  changeEPOSstate(1);  // should now be in state 'switched on (4)'
  

  // 4. enable operation
  n = checkEPOSstate();
  if ( n != 4 ){
    printf("\nEPOS is NOT in 'switched on' state, but in state %d -> quit!\n", n);
    printf("( %s(), %s line %d)\n", __func__, __FILE__, __LINE__);
    exit(1);
  }
  printf("\nenable operation " );
  changeEPOSstate(5);

  n = checkEPOSstate();
  if ( n != 7 ){
    printf("\nEPOS is NOT in 'Operation enable' state, but in state %d -> quit!\n", n);
    printf("( %s(), %s line %d)\n", __func__, __FILE__, __LINE__);
    exit(1);
  }
  
  // ok, up and running...
  printf("EPOS is up and running, have fun!\n");
  printf("********************************************\n");


  printHelp();
  
  do {


    scanf("%c", &c);
    getchar();  //dump last char (=='\n') 


    switch(c) {
      
    case 0x30:  // ASCII '0'
      printf("moving to filter 0...\n");
      moveAbsolute(ZERO_OFFSET);
      monitorStatus(); // this will also wait until positoning is finished
      printf("  done!\a\n");
      break;

    case 0x31:  // ASCII '1'
      printf("moving to filter 1...\n");
      moveAbsolute(ZERO_OFFSET + FILTER_FILTER);
      monitorStatus(); // this will also wait until positoning is finished
      printf("  done!\a\n");
      break;

    case 0x32:  // ASCII '2'
      printf("moving to filter 2...\n");
      moveAbsolute(ZERO_OFFSET + 2* FILTER_FILTER);
      monitorStatus(); // this will also wait until positoning is finished
      printf("  done!\a\n");
      break;

    case 0x33:  // ASCII '3'
      printf("moving to filter 3...\n");
      moveAbsolute(ZERO_OFFSET + 3* FILTER_FILTER);
      monitorStatus(); // this will also wait until positoning is finished
      printf("  done!\a\n");
      break;

    case 0x34:  // ASCII '4'
      printf("moving to filter 4...\n");
      moveAbsolute(ZERO_OFFSET + 4* FILTER_FILTER);
      monitorStatus(); // this will also wait until positoning is finished
      printf("  done!\a\n");
      break;

    case 0x35:  // ASCII '5'
      printf("moving to filter 5...\n");
      moveAbsolute(ZERO_OFFSET + 5* FILTER_FILTER);
      monitorStatus(); // this will also wait until positoning is finished
      printf("  done!\a\n");
      break;

    case 0x72:  // 'r'
      printf("searching for reference point. This may take a while...\n");

      // filter wheel home switch is low-active; THIS IS NOT THE DEFAULT!
      if ( setHomePolarity(1) ) {
	fprintf(stderr, "\a\n *** UNABLE TO SET HomeSwitch TO low-active!!! *** \n\n");
      }
      if ( (n = doHoming(7,-700000 )) ) {
	fprintf(stderr, "\a  #### doHoming() returned %d ####\n", n);
      }
      
      readActualPosition(&pos);
      printf("\nEPOS position is %ld.\n", pos);
      break;

    case 0x68:  // 'h'
      printHelp();
      break;
      
    case 0x71: // quit
      break;

    default:
      printf("\aKEY >%c< UNKNOWN!\n", c);
      printHelp();
      break;
    }

  } while (c != 'q');

  


  /*****************************************/
  /*  all done, shutting off               */
  /*****************************************/
  
/*   printf("\ninitiating shutdown "); */
  changeEPOSstate(2);
  printEPOSstate();
  
  closeEPOS();
  printf("****************************************\n");
  return 0;
}
